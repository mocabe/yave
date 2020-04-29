//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/node_compiler.hpp>
#include <yave/support/log.hpp>
#include <yave/node/core/socket_instance_manager.hpp>
#include <yave/node/core/node_definition.hpp>
#include <yave/node/core/node_declaration.hpp>
#include <yave/rts/dynamic_typing.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/rts/value_cast.hpp>
#include <yave/rts/unit.hpp>
#include <yave/module/std/primitive/primitive.hpp>

#include <functional>

#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
#include <boost/uuid/uuid_hash.hpp>
#include <tl/optional.hpp>

YAVE_DECL_G_LOGGER(node_compiler)

using namespace std::string_literals;

// MACROS ARE (NOT) YOUR FRIEND.
#define mem_fn(FN, ...) \
  [&](auto&& arg) { return FN(std::forward<decltype(arg)>(arg), __VA_ARGS__); }

namespace yave {

  namespace rs = ranges;
  namespace rv = ranges::views;

  // tl::optional -> std::optional
  template <class T>
  constexpr auto to_std(tl::optional<T>&& opt) -> std::optional<T>
  {
    if (opt)
      return std::move(*opt);
    else
      return std::nullopt;
  }

  class node_compiler::impl
  {
    error_list errors;

    auto type(
      const managed_node_graph& parsed_graph,
      const node_declaration_store& decls,
      const node_definition_store& defs) -> tl::optional<executable>;

    auto verbose_check(executable&& exe, int) -> tl::optional<executable>;

    auto desugar(structured_node_graph&& ng, int)
      -> tl::optional<structured_node_graph>;

    auto gen(structured_node_graph&& ng, const node_definition_store& defs)
      -> tl::optional<std::pair<object_ptr<const Object>, class_env>>;

    auto type(std::pair<object_ptr<const Object>, class_env>&& p, int)
      -> tl::optional<executable>;

    auto optimize(executable&& exe, int) -> tl::optional<executable>;

  public:
    auto get_errors() const
    {
      return errors.clone();
    }

    auto compile(
      managed_node_graph&& parsed_graph,
      const node_declaration_store& decls,
      const node_definition_store& defs) -> tl::optional<executable>
    {
      errors.clear();

      Info(g_logger, "Start compiling node tree:");
      Info(g_logger, "  Total {} nodes in graph", parsed_graph.nodes().size());
      Info(g_logger, "  Total {} node declarations", decls.size());
      Info(g_logger, "  Total {} node definitions", defs.size());

      return tl::make_optional(std::move(parsed_graph)) //
        .and_then(mem_fn(type, decls, defs))
        .and_then(mem_fn(verbose_check, 0))
        .or_else([&] {
          Error(g_logger, "Failed to compiler node graph");
          for (auto&& e : errors)
            Error(g_logger, "  {}", e.message());
        });
    }

    auto compile(structured_node_graph&& ng, const node_definition_store& defs)
      -> tl::optional<executable>
    {
      errors.clear();

      Info(g_logger, "Start compiling node tree:");
      Info(g_logger, "  Total {} node definitions", defs.size());

      return tl::make_optional(std::move(ng)) //
        .and_then(mem_fn(desugar, 0))
        .and_then(mem_fn(gen, defs))
        .and_then(mem_fn(type, 0))
        .and_then(mem_fn(optimize, 0))
        .and_then(mem_fn(verbose_check, 0))
        .or_else([&] {
          Error(g_logger, "Failed to compile node graph");
          for (auto&& e : errors)
            Error(g_logger, "  {}", e.message());
        });
    }
  };

  node_compiler::node_compiler()
    : m_pimpl {std::make_unique<impl>()}
  {
    init_logger();
  }

  node_compiler::~node_compiler() noexcept = default;

  auto node_compiler::get_errors() const -> error_list
  {
    return m_pimpl->get_errors();
  }

  auto node_compiler::compile(
    managed_node_graph&& parsed_graph,
    const node_declaration_store& decls,
    const node_definition_store& defs) -> std::optional<executable>
  {
    return to_std(m_pimpl->compile(std::move(parsed_graph), decls, defs));
  }

  auto node_compiler::compile(
    structured_node_graph&& graph,
    const node_definition_store& defs) -> std::optional<executable>
  {
    return to_std(m_pimpl->compile(std::move(graph), defs));
  }

  /*
    Type check prime trees and generate apply graph.
    Assumes prime tree is already successfully parsed by node_parser.
    We use socket_instance_manager to collect all node instances attached to
    specific node/socket combination to share instance objects across multiple
    inputs.

    This function is depending on overloading resolution extension in RTS
    module. After typing over gneralized types on each overloaded functions,
    type checker checks existance of appropriate function to call, then replace
    all occurence of overloaded call with the selected closure object. If type
    checking or overloading selection failed, it throws type_error so we need to
    trap them to report to frontend.

    Most of errors thrown from type checker does not relate to node handle, so
    we need to keep track relation between backend object and nodes.
   */
  auto node_compiler::impl::type(
    const managed_node_graph& ng,
    const node_declaration_store& decls,
    const node_definition_store& defs) -> tl::optional<executable>
  {
    auto root_group = ng.root_group();

    assert(ng.output_sockets(root_group).size() == 1);
    assert(ng.input_sockets(root_group).empty());

    auto root_socket = ng.output_sockets(root_group)[0];

    class_env env;
    socket_instance_manager sim;

    auto rec =
      [&](auto&& self, const auto& socket) -> object_ptr<const Object> {
      assert(ng.get_info(socket)->type() == socket_type::output);

      auto node = ng.node(socket);

      size_t socket_index = 0;
      for (auto&& s : ng.output_sockets(node)) {
        if (s == socket)
          break;
        ++socket_index;
      }

      if (auto inst = sim.find(socket)) {
        return inst->instance;
      }

      auto iss = ng.input_sockets(node);

      // group: make lambda if it's lambda form
      if (ng.is_group(node)) {

        // lambda form
        auto isLambda = [&] {
          for (auto&& s : iss) {
            if (ng.connections(s).empty())
              return true;
          }
          return false;
        }();

        auto inside = ng.get_group_socket_inside(socket);

        auto body = self(ng.get_info(ng.connections(inside)[0])->src_socket());

        // apply lambda
        if (isLambda) {
          for (auto iter = iss.rbegin(); iter != iss.rend(); ++iter) {
            auto& s = *iter;
            body =
              make_object<Lambda>(value_cast<Variable>(ng.get_data(s)), body);
          }
        }

        return body;
      }

      // group input
      if (ng.is_group_input(node)) {

        auto outside = ng.get_group_socket_outside(socket);

        // not connected: return variable
        if (ng.connections(outside).empty())
          return value_cast<Variable>(ng.get_data(outside));

        return self(ng.get_info(ng.connections(outside)[0])->src_socket());
      }

      // acquire node object
      assert(decls.find(*ng.get_name(node)));
      auto qualified_name = decls.find(*ng.get_name(node))->qualified_name();
      auto ds             = defs.get_binds(qualified_name, socket_index);

      if (ds.empty())
        throw compile_error::no_valid_overloading(node, socket);

      std::vector<object_ptr<const Object>> insts;
      insts.reserve(defs.size());
      for (auto&& def : ds) {
        insts.push_back(def->instance());
      }

      auto overloaded = insts.size() == 1
                          ? insts[0]
                          : env.add_overloading(uid::random_generate(), insts);

      // apply inputs
      for (auto&& s : iss) {
        if (ng.connections(s).empty()) {

          // lambda form
          if (!ng.get_data(s))
            return overloaded;

          // apply default argument
          if (auto holder = value_cast_if<DataTypeHolder>(ng.get_data(s)))
            overloaded = overloaded << holder->get_data_constructor();
          else
            overloaded = overloaded << ng.get_data(s);

        } else {
          // normal argument
          assert(ng.connections(s).size() == 1);

          overloaded = overloaded
                       << self(ng.get_info(ng.connections(s)[0])->src_socket());
        }
      }
      return overloaded;
    };

    // FIXME: Implement proper error info
    auto srcn = root_group;
    auto srcs = ng.output_sockets(srcn).at(0);

    try {

      // build apply tree
      auto app = fix_lambda(rec)(root_socket);

      // now we can check type and resolve overloadings
      auto [ty, app2] = type_of_overloaded(app, env);

      // return result tree
      if (errors.empty()) {
        Info(
          g_logger, "Successfully type checked node tree! : {}", to_string(ty));
        return executable(app2, ty);
      } else
        return tl::nullopt;

      // common type errors
    } catch (const type_error::no_valid_overloading& e) {
      errors.push_back(
        make_error<compile_error::no_valid_overloading>(srcn, srcs));
    } catch (const type_error::type_missmatch& e) {
      errors.push_back(make_error<compile_error::type_missmatch>(
        srcn, srcs, e.expected(), e.provided()));

      // internal type errors
    } catch (const type_error::unbounded_variable& e) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        srcn, srcs, "Internal type error: unbounded variable detected"));
    } catch (const type_error::circular_constraint& e) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        srcn, srcs, "Internal type error: circular constraint detected"));
    } catch (const type_error::type_error& e) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        srcn, srcs, "Internal type error: "s + e.what()));

      // forward
    } catch (const error_info_base& e) {
      errors.push_back(error(e.clone()));

      // others
    } catch (const std::exception& e) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        srcn,
        srcs,
        "Internal error: Unknown std::exception detected: "s + e.what()));
    } catch (...) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        srcn, srcs, "Internal error: Unknown exception detected"));
    }
    return tl::nullopt;
  }

  auto node_compiler::impl::verbose_check(executable&& exe, int)
    -> tl::optional<executable>
  {
    // FIXME
    auto srcn = node_handle();
    auto srcs = socket_handle();

    try {

      auto tp = type_of(exe.object());

      if (!same_type(tp, exe.type())) {
        errors.push_back(make_error<compile_error::unexpected_error>(
          srcn, srcs, "Verbose type check failed: result type does not match"));
        return tl::nullopt;
      }

      return std::move(exe);

    } catch (const std::exception& e) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        srcn, srcs, "Exception thrown while checking result: "s + e.what()));
    } catch (...) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        srcn, srcs, "Exception thrown while checking result: "));
    }
    return tl::nullopt;
  }

  auto node_compiler::impl::desugar(structured_node_graph&& ng, int)
    -> tl::optional<structured_node_graph>
  {
    auto roots = ng.search_path("/");

    auto root = [&] {
      for (auto&& r : roots)
        if (ng.get_name(r) == "root")
          return r;
      assert(false);
    }();

    assert(ng.output_sockets(root).size() == 1);
    auto rootos = ng.output_sockets(root)[0];

    // Add Variables on empty input socket of lambda calls
    auto fill_variables = [](const auto& n, auto& ng) {
      assert(ng.is_group(n));
      if (ng.input_connections(n).size() < ng.input_sockets(n).size())
        for (auto&& s : ng.input_sockets(n))
          ng.set_data(s, make_object<Variable>());
    };

    // Rmove unsued default socket data
    auto omit_unused_defaults = [](const auto& n, auto& ng) {
      assert(ng.is_function(n));
      for (auto&& s : ng.input_sockets(n))
        if (!ng.connections(s).empty())
          ng.set_data(s, nullptr);
    };

    // for group
    auto rec_g = [&](auto&& rec_n, const auto& g, const auto& os) -> void {
      // fill
      fill_variables(g, ng);

      // inputs
      for (auto&& c : ng.input_connections(g)) {
        auto ci = ng.get_info(c);
        rec_n(ci->src_node(), ci->src_socket());
      }

      // inside
      {
        auto go  = ng.get_group_output(g);
        auto idx = *ng.get_index(os);
        auto s   = ng.input_sockets(go)[idx];

        for (auto&& c : ng.connections(s)) {
          auto ci = ng.get_info(c);
          rec_n(ci->src_node(), ci->src_socket());
        }
      }
    };

    // for function
    auto rec_f = [&](auto&& rec_n, const auto& f) -> void {
      // omit
      omit_unused_defaults(f, ng);

      // inputs
      for (auto&& c : ng.input_connections(f)) {
        auto ci = ng.get_info(c);
        rec_n(ci->src_node(), ci->src_socket());
      }
    };

    // general
    auto rec_n = [&](auto&& self, const auto& n, const auto& os) {
      if (ng.is_group(n))
        return rec_g(self, n, os);

      if (ng.is_function(n))
        return rec_f(self, n);

      if (ng.is_group_input(n))
        return;

      assert(false);
    };

    auto rec = fix_lambda(rec_n);
    rec(root, rootos);

    return std::move(ng);
  }

  auto node_compiler::impl::gen(
    structured_node_graph&& ng,
    const node_definition_store& defs)
    -> tl::optional<std::pair<object_ptr<const Object>, class_env>>
  {
    auto roots = ng.search_path("/");

    auto root = [&] {
      for (auto&& r : roots)
        if (ng.get_name(r) == "root")
          return r;
      assert(false);
    }();

    assert(ng.output_sockets(root).size() == 1);
    auto rootos = ng.output_sockets(root)[0];

    // class overload environment
    class_env env;

    // get overloaded function
    auto get_function_body =
      [&](const auto& f, const auto& os) -> object_ptr<const Object> {
      assert(ng.is_function(f));

      auto defcall = ng.get_definition(f);

      Info(g_logger, "get_function_body(): {}", *ng.get_path(defcall));

      // check if already added
      if (auto overloaded = env.find_overloaded(defcall.id()))
        return overloaded;

      auto ds = defs.get_binds(*ng.get_path(defcall), *ng.get_index(os));

      if (ds.empty())
        throw compile_error::no_valid_overloading(f, os);

      std::vector<object_ptr<const Object>> insts;
      insts.reserve(defs.size());

      for (auto&& d : ds)
        insts.push_back(d->instance());

      return insts.size() == 1 ? insts[0]
                               : env.add_overloading(defcall.id(), insts);
    };

    // group
    auto rec_g = [&](
                   auto&& rec_n,
                   const auto& g,
                   const auto& os,
                   const auto& in) -> object_ptr<const Object> {
      assert(ng.is_group(g));

      Info(g_logger, "rec_g: {}", *ng.get_name(g));

      // inputs
      std::vector<object_ptr<const Object>> ins;
      for (auto&& s : ng.input_sockets(g)) {

        // variable
        if (auto data = ng.get_data(s)) {
          assert(ng.connections(s).empty());
          ins.push_back(data);
          continue;
        }

        assert(ng.connections(s).size() == 1);
        auto ci = ng.get_info(ng.connections(s)[0]);
        ins.push_back(rec_n(ci->src_node(), ci->src_socket(), in));
      }

      // inside
      object_ptr<const Object> ret;
      {
        auto go  = ng.get_group_output(g);
        auto idx = *ng.get_index(os);
        auto s   = ng.input_sockets(go)[idx];

        assert(ng.connections(s).size() == 1);

        auto ci = ng.get_info(ng.connections(s)[0]);
        ret     = rec_n(ci->src_node(), ci->src_socket(), ins);
      }

      // add Lambda
      for (auto&& i : ins | rv::reverse)
        if (auto var = value_cast_if<Variable>(i))
          ret = make_object<Lambda>(var, ret);

      return ret;
    };

    // function
    auto rec_f = [&](
                   auto&& rec_n,
                   const auto& f,
                   const auto& os,
                   const auto& in) -> object_ptr<const Object> {
      (void)os;
      assert(ng.is_function(f));

      Info(g_logger, "rec_f: {}", *ng.get_name(f));

      auto body = get_function_body(f, os);

      for (auto&& s : ng.input_sockets(f)) {

        // default value
        if (auto data = ng.get_data(s)) {

          // FIXME: Remove this branch
          if (auto holder = value_cast_if<DataTypeHolder>(data))
            body = body << holder->get_data_constructor();
          else
            body = body << data;

          continue;
        }

        auto cs = ng.connections(s);

        // lambda
        if (cs.empty())
          return body;

        auto ci = ng.get_info(cs[0]);
        body    = body << rec_n(ci->src_node(), ci->src_socket(), in);
      }

      return body;
    };

    // group input
    auto rec_i = [&](const auto& i, const auto& os, const auto& in) {
      assert(ng.is_group_input(i));
      auto idx = *ng.get_index(os);
      return in[idx];
    };

    // general
    auto rec_n =
      [&](auto&& self, const auto& n, const auto& os, const auto& in) {
        if (ng.is_group(n))
          return rec_g(self, n, os, in);

        if (ng.is_function(n))
          return rec_f(self, n, os, in);

        if (ng.is_group_input(n))
          return rec_i(n, os, in);

        assert(false);
      };

    // FIXME:
    node_handle srcn;
    socket_handle srcs;

    try {

      auto rec = fix_lambda(rec_n);
      auto app = rec(root, rootos, std::vector<object_ptr<const Object>>());
      return std::make_pair(std::move(app), std::move(env));

      // forward
    } catch (const error_info_base& e) {
      errors.push_back(error(e.clone()));
      // other
    } catch (const std::exception& e) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        srcn,
        srcs,
        "Internal error: Unknown std::exception detected: "s + e.what()));
    } catch (...) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        srcn, srcs, "Internal error: Unknown exception detected"));
    }
    return tl::nullopt;
  }

  auto node_compiler::impl::type(
    std::pair<object_ptr<const Object>, class_env>&& p,
    int) -> tl::optional<executable>
  {
    // FIXME
    node_handle srcn;
    socket_handle srcs;

    try {

      auto [app, env] = p;
      auto [ty, app2] = type_of_overloaded(app, env);

      return executable(app2, ty);

      // common type errors
    } catch (const type_error::no_valid_overloading& e) {
      errors.push_back(
        make_error<compile_error::no_valid_overloading>(srcn, srcs));
    } catch (const type_error::type_missmatch& e) {
      errors.push_back(make_error<compile_error::type_missmatch>(
        srcn, srcs, e.expected(), e.provided()));

      // internal type errors
    } catch (const type_error::unbounded_variable& e) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        srcn, srcs, "Internal type error: unbounded variable detected"));
    } catch (const type_error::circular_constraint& e) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        srcn, srcs, "Internal type error: circular constraint detected"));
    } catch (const type_error::type_error& e) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        srcn, srcs, "Internal type error: "s + e.what()));

      // forward
    } catch (const error_info_base& e) {
      errors.push_back(error(e.clone()));

      // others
    } catch (const std::exception& e) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        srcn,
        srcs,
        "Internal error: Unknown std::exception detected: "s + e.what()));
    } catch (...) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        srcn, srcs, "Internal error: Unknown exception detected"));
    }
    return tl::nullopt;
  }

  auto node_compiler::impl::optimize(executable&& exe, int)
    -> tl::optional<executable>
  {
    return std::move(exe);
  }
} // namespace yave