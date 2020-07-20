//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/node_compiler.hpp>
#include <yave/node/compiler/location.hpp>
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

    auto verbose_check(executable&& exe, int) -> tl::optional<executable>;

    auto desugar(structured_node_graph&& ng, int)
      -> tl::optional<structured_node_graph>;

    auto gen(structured_node_graph&& ng, const node_definition_store& defs)
      -> tl::optional<
        std::tuple<object_ptr<const Object>, class_env, location_map>>;

    auto type(
      std::tuple<object_ptr<const Object>, class_env, location_map>&& p,
      int) -> tl::optional<executable>;

    auto optimize(executable&& exe, int) -> tl::optional<executable>;

  public:
    auto get_errors() const
    {
      return errors.clone();
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
    structured_node_graph&& graph,
    const node_definition_store& defs) -> std::optional<executable>
  {
    return to_std(m_pimpl->compile(std::move(graph), defs));
  }

  auto node_compiler::impl::verbose_check(executable&& exe, int)
    -> tl::optional<executable>
  {
    try {

      auto tp = type_of(exe.object());

      if (!same_type(tp, exe.type())) {
        errors.push_back(make_error<compile_error::unexpected_error>(
          socket_handle(),
          "Verbose type check failed: result type does not match"));
        return tl::nullopt;
      }

      return std::move(exe);

    } catch (const std::exception& e) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        socket_handle(),
        "Exception thrown while checking result: "s + e.what()));
    } catch (...) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        socket_handle(), "Exception thrown while checking result: "));
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
      unreachable();
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
    -> tl::optional<
      std::tuple<object_ptr<const Object>, class_env, location_map>>
  {
    auto roots = ng.search_path("/");

    auto root = [&] {
      for (auto&& r : roots)
        if (ng.get_name(r) == "root")
          return r;
      unreachable();
    }();

    assert(ng.output_sockets(root).size() == 1);
    auto rootos = ng.output_sockets(root)[0];

    // class overload environment
    class_env env;
    // location
    location_map loc;

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
        throw compile_error::no_valid_overloading(os);

      auto insts = ds //
                   | rv::transform([](auto& d) { return d->instance(); })
                   | rs::to_vector;

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

      Info(g_logger, "rec_g: {}, os={}", *ng.get_name(g), to_string(os.id()));

      // inputs
      std::vector<object_ptr<const Object>> ins;
      for (auto&& s : ng.input_sockets(g)) {

        // variable
        if (auto data = ng.get_data(s)) {
          assert(ng.connections(s).empty());
          loc.add_location(data, s);
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
      for (auto&& i : ins | rv::reverse) {
        if (auto var = value_cast_if<Variable>(i)) {
          ret = make_object<Lambda>(var, ret);
          loc.add_location(ret, os);
        }
      }

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

      Info(g_logger, "rec_f: {}, os={}", *ng.get_name(f), to_string(os.id()));

      auto body = get_function_body(f, os);
      loc.add_location(body, os);

      for (auto&& s : ng.input_sockets(f)) {

        // default value
        if (auto data = ng.get_data(s)) {

          // FIXME: Remove this branch
          if (auto holder = value_cast_if<DataTypeHolder>(data))
            body = body << holder->get_data_constructor(holder);
          else
            body = body << data;

          loc.add_location(body, os);
          continue;
        }

        auto cs = ng.connections(s);

        // lambda
        if (cs.empty())
          return body;

        auto ci = ng.get_info(cs[0]);
        body    = body << rec_n(ci->src_node(), ci->src_socket(), in);
        loc.add_location(body, os);
      }

      return body;
    };

    // group input
    auto rec_i = [&](const auto& os, const auto& in) {
      auto idx = *ng.get_index(os);
      auto ret = in[idx];
      loc.add_location(ret, os);
      return ret;
    };

    // general
    auto rec_n =
      [&](auto&& self, const auto& n, const auto& os, const auto& in) {
        if (ng.is_group(n))
          return rec_g(self, n, os, in);

        if (ng.is_function(n))
          return rec_f(self, n, os, in);

        if (ng.is_group_input(n))
          return rec_i(os, in);

        unreachable();
      };

    try {

      auto rec = fix_lambda(rec_n);
      auto app = rec(root, rootos, std::vector<object_ptr<const Object>>());
      return std::make_tuple(std::move(app), std::move(env), std::move(loc));

      // forward
    } catch (const error_info_base& e) {
      errors.push_back(error(e.clone()));

      // other
    } catch (const std::exception& e) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        socket_handle(),
        "Internal error: Unknown std::exception detected: "s + e.what()));
    } catch (...) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        socket_handle(), "Internal error: Unknown exception detected"));
    }
    return tl::nullopt;
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
    std::tuple<object_ptr<const Object>, class_env, location_map>&& p,
    int) -> tl::optional<executable>
  {
    try {

      auto [app, env, loc] = std::move(p);
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      return executable(app2, ty);

      // normal errors
    } catch (const error_info_base& e) {
      errors.push_back(error(e.clone()));

      // internal type errors
    } catch (const type_error::type_error& e) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        socket_handle(), "Internal type error: "s + e.what()));

      // others
    } catch (const std::exception& e) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        socket_handle(),
        "Internal error: Unknown std::exception detected: "s + e.what()));
    } catch (...) {
      errors.push_back(make_error<compile_error::unexpected_error>(
        socket_handle(), "Internal error: Unknown exception detected"));
    }
    return tl::nullopt;
  }

  auto node_compiler::impl::optimize(executable&& exe, int)
    -> tl::optional<executable>
  {
    return std::move(exe);
  }
} // namespace yave