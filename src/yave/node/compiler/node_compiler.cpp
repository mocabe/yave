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
#include <yave/obj/node/argument.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/node/core/function.hpp>

#include <functional>

#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
#include <boost/uuid/uuid_hash.hpp>
#include <tl/optional.hpp>

YAVE_DECL_G_LOGGER(node_compiler)

using namespace std::string_literals;

namespace yave {

  namespace {

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

    struct FrameBufferOutput
      : NodeFunction<FrameBufferOutput, FrameBuffer, FrameBuffer>
    {
      auto code() const -> return_type
      {
        return arg<0>();
      }

      static auto declaration()
      {
        return node_declaration(
          "FrameBufferOutput", "/_", "internal", {""}, {""});
      }

      static auto definition()
      {
        auto decl = declaration();

        return node_definition(
          decl.qualified_name(), 0, make_object<FrameBufferOutput>(), "");
      }
    };

  } // namespace

  class node_compiler_result::impl
  {
    /// executable
    std::optional<executable> m_exe;
    /// results
    std::vector<compile_result> m_results;

  public:
    impl() = default;

  public:
    bool success() const
    {
      return m_exe.has_value();
    }

    auto clone_executable() const
    {
      return m_exe ? m_exe->clone() : std::optional<executable>();
    }

    void set_executable(executable&& exe)
    {
      m_exe = std::move(exe);
    }

    void add_result(compile_result r)
    {
      m_results.push_back(std::move(r));
    }

    auto get_filtered_result(compile_result_type ty)
    {
      std::vector<compile_result> ret;

      for (auto&& r : m_results) {
        if (type(r) == ty) {
          ret.push_back(r);
        }
      }
      return ret;
    }

    auto get_results(const structured_node_graph& ng, const node_handle& n)
      const
    {
      std::vector<compile_result> ret;

      for (auto&& r : m_results) {

        auto h = ng.node(node_id(r));

        // need to check gruop relation
        if (h == n || ng.is_parent_of(n, h)) {
          ret.push_back(r);
        }
      }
      return ret;
    }

    auto get_results(const structured_node_graph& ng, const socket_handle& s)
    {
      std::vector<compile_result> ret;

      for (auto&& r : m_results) {

        auto h = ng.socket(socket_id(r));

        if (h == s) {
          ret.push_back(r);
        }
      }
      return ret;
    }
  };

  node_compiler_result::node_compiler_result()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  // clang-format off
  node_compiler_result::~node_compiler_result() noexcept = default;
  node_compiler_result::node_compiler_result(node_compiler_result&&) noexcept = default;
  node_compiler_result& node_compiler_result::operator=(node_compiler_result&&) noexcept = default;
  // clang-format on

  bool node_compiler_result::success() const
  {
    return m_pimpl->success();
  }

  auto node_compiler_result::clone_executable() const
    -> std::optional<executable>
  {
    return m_pimpl->clone_executable();
  }

  auto node_compiler_result::get_errors() const -> std::vector<compile_result>
  {
    return m_pimpl->get_filtered_result(compile_result_type::error);
  }

  auto node_compiler_result::get_warnings() const -> std::vector<compile_result>
  {
    return m_pimpl->get_filtered_result(compile_result_type::warning);
  }

  auto node_compiler_result::get_infos() const -> std::vector<compile_result>
  {
    return m_pimpl->get_filtered_result(compile_result_type::info);
  }

  auto node_compiler_result::get_results(
    const structured_node_graph& ng,
    const node_handle& n) const -> std::vector<compile_result>
  {
    return m_pimpl->get_results(ng, n);
  }

  auto node_compiler_result::get_results(
    const structured_node_graph& ng,
    const socket_handle& s) const -> std::vector<compile_result>
  {
    return m_pimpl->get_results(ng, s);
  }

  void node_compiler_result::add_result(compile_result r)
  {
    m_pimpl->add_result(std::move(r));
  }

  void node_compiler_result::set_executable(executable&& exe)
  {
    m_pimpl->set_executable(std::move(exe));
  }

  class node_compiler::impl
  {
    auto verbose_check(executable&& exe, node_compiler_result& res)
      -> tl::optional<executable>;

    auto desugar(
      structured_node_graph&& ng,
      node_definition_store& defs,
      node_compiler_result& res) -> tl::optional<structured_node_graph>;

    auto gen(
      structured_node_graph&& ng,
      const node_definition_store& defs,
      node_compiler_result& res)
      -> tl::optional<
        std::tuple<object_ptr<const Object>, class_env, location_map>>;

    auto type(
      std::tuple<object_ptr<const Object>, class_env, location_map>&& p,
      node_compiler_result& res) -> tl::optional<executable>;

    auto optimize(executable&& exe, node_compiler_result& res)
      -> tl::optional<executable>;

    auto set_result(executable&& exe, node_compiler_result& res)
      -> tl::optional<nullptr_t>;

  public:
    auto compile(structured_node_graph&& ng, node_definition_store&& defs)
      -> node_compiler_result
    {
      Info(g_logger, "Start compiling node tree:");
      Info(g_logger, "  Total {} node definitions detected", defs.size());

      node_compiler_result result;

      // clang-format off
      tl::make_optional(std::move(ng)) //
        .and_then([&](auto arg) { return desugar(std::move(arg) ,defs, result); })
        .and_then([&](auto arg) { return gen(std::move(arg), defs, result); })
        .and_then([&](auto arg) { return type(std::move(arg), result); })
        .and_then([&](auto arg) { return optimize(std::move(arg), result); })
        .and_then([&](auto arg) { return verbose_check(std::move(arg), result); })
        .and_then([&](auto arg) { return set_result(std::move(arg), result); });
      // clang-format on

      return result;
    }
  };

  node_compiler::node_compiler()
    : m_pimpl {std::make_unique<impl>()}
  {
    init_logger();
  }

  node_compiler::~node_compiler() noexcept = default;

  auto node_compiler::compile(params p) -> node_compiler_result
  {
    return m_pimpl->compile(std::move(p.node_graph), std::move(p.node_defs));
  }

  auto node_compiler::impl::verbose_check(
    executable&& exe,
    node_compiler_result& res) -> tl::optional<executable>
  {
    using namespace compile_results;

    try {

      auto tp = type_of(exe.object());

      if (!same_type(tp, exe.type())) {
        res.add_result(unexpected_error(
          "Verbose type check failed: result type does not match"));
        return tl::nullopt;
      }

      return std::move(exe);

    } catch (const std::exception& e) {
      res.add_result(unexpected_error(
        "Exception thrown while checking result: "s + e.what()));
    } catch (...) {
      res.add_result(
        unexpected_error("Exception thrown while checking result: "));
    }
    return tl::nullopt;
  }

  auto node_compiler::impl::desugar(
    structured_node_graph&& ng,
    node_definition_store& defs,
    node_compiler_result & /*res*/) -> tl::optional<structured_node_graph>
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

    // Insert frame buffer output
    {
      auto decl = FrameBufferOutput::declaration();
      auto def  = FrameBufferOutput::definition();

      defs.add(def);

      auto func = ng.create_function(decl);
      auto call = ng.create_copy(root, func);

      auto is = ng.input_sockets(ng.get_group_output(root))[0];
      assert(ng.connections(is).size() == 1);

      auto c     = ng.connections(is)[0];
      auto cinfo = ng.get_info(c);
      ng.disconnect(c);

      ng.connect(cinfo->src_socket(), ng.input_sockets(call)[0]);
      ng.connect(ng.output_sockets(call)[0], cinfo->dst_socket());
    }

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
    const node_definition_store& defs,
    node_compiler_result& res)
    -> tl::optional<
      std::tuple<object_ptr<const Object>, class_env, location_map>>
  {
    using namespace compile_results;

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
        throw no_valid_overloading(os);

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

        // default arg value
        if (auto data = ng.get_data(s)) {

          // TODO: remove this branch
          if (auto arg = value_cast_if<NodeArgument>(data))
            body = body << arg->get_data_constructor(arg);
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

    } catch (const compile_result& r) {
      // forward
      res.add_result(r);

      // other
    } catch (const std::exception& e) {
      res.add_result(unexpected_error(
        "Internal error: Unknown std::exception detected: "s + e.what()));
    } catch (...) {
      res.add_result(
        unexpected_error("Internal error: Unknown exception detected"));
    }
    return tl::nullopt;
  }

  /*
    Type check prime trees and generate apply graph.
    Assumes prime tree is already successfully parsed by node_parser.

    This function is depending on overloading resolution extension in RTS
    module. After typing over gneralized types on each overloaded functions,
    type checker checks existance of appropriate function to call, then replace
    all occurence of overloaded call with the selected closure object. If type
    checking or overloading selection failed, it throws type_error so we need to
    trap them to report to frontend.
   */
  auto node_compiler::impl::type(
    std::tuple<object_ptr<const Object>, class_env, location_map>&& p,
    node_compiler_result& res) -> tl::optional<executable>
  {
    using namespace compile_results;

    try {

      auto [app, env, loc] = std::move(p);
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      return executable(app2, ty);

      // normal errors
    } catch (const compile_result& r) {
      // forward
      res.add_result(r);

      // others
    } catch (const std::exception& e) {
      res.add_result(unexpected_error(
        "Internal error: Unknown std::exception detected: "s + e.what()));
    } catch (...) {
      res.add_result(
        unexpected_error("Internal error: Unknown exception detected"));
    }
    return tl::nullopt;
  }

  auto node_compiler::impl::optimize(
    executable&& exe,
    node_compiler_result & /*res*/) -> tl::optional<executable>
  {
    return std::move(exe);
  }

  auto node_compiler::impl::set_result(
    executable&& exe,
    node_compiler_result& res) -> tl::optional<nullptr_t>
  {
    res.set_executable(std::move(exe));
    return {};
  }
} // namespace yave