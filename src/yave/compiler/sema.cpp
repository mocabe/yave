//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/compiler/compile.hpp>
#include <yave/compiler/message.hpp>
#include <yave/compiler/executable.hpp>
#include <yave/compiler/location.hpp>
#include <yave/compiler/typecheck.hpp>
#include <yave/compiler/argument_holder.hpp>
#include <yave/support/log.hpp>
#include <yave/node/core/socket_instance_manager.hpp>
#include <yave/node/core/node_definition.hpp>
#include <yave/node/core/node_declaration.hpp>
#include <yave/node/core/properties.hpp>
#include <yave/rts/dynamic_typing.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/rts/value_cast.hpp>
#include <yave/rts/unit.hpp>
#include <yave/obj/node/argument.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/node/core/node_definition_store.hpp>

#include <functional>

#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
#include <boost/uuid/uuid_hash.hpp>
#include <tl/optional.hpp>

YAVE_DECL_G_LOGGER(sema)

namespace yave::compiler {

  namespace {

    namespace rn = ranges;
    namespace rv = ranges::views;

    using namespace std::literals::string_literals;

    using arg_holder_map_t =
      std::map<socket_handle, object_ptr<NodeArgumentHolder>>;

    void set_arg_holder(
      arg_holder_map_t& map,
      socket_handle s,
      object_ptr<NodeArgumentHolder> arg)
    {
      map.emplace(s, std::move(arg));
    }

    auto get_arg_holder(const arg_holder_map_t& map, socket_handle s)
      -> object_ptr<NodeArgumentHolder>
    {
      if (auto it = map.find(s); it != map.end())
        return it->second;
      return nullptr;
    }

    auto desugar(
      structured_node_graph&& ng,
      const socket_handle& os,
      const node_declaration_map& decls,
      arg_holder_map_t& arg_map,
      message_map& /*msgs*/) -> tl::optional<structured_node_graph>
    {
      auto root   = ng.node(os);
      auto rootos = os;

      // omit unsued default socket data
      auto select_arguments = [&](const auto& n, auto& ng) {
        for (auto&& s : ng.input_sockets(n)) {
          if (ng.connections(s).empty()) {
            if (auto arg = get_arg(s, ng, decls))
              set_arg_holder(arg_map, s, make_argument_holder(arg));
          }
        }
      };

      // add variables on empty input sockets
      auto insert_variables = [&](const auto& n, auto& ng) {
        for (auto&& s : ng.input_sockets(n)) {
          if (ng.connections(s).empty() && !get_arg_property(s, ng)) {
            set_arg_holder(
              arg_map, s, make_argument_holder(make_object<Variable>()));
          }
        }
      };

      // for group
      auto rec_g = [&](auto&& rec_n, const auto& g, const auto& os) -> void {
        // args
        select_arguments(g, ng);
        // vars
        insert_variables(g, ng);

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
        // select
        select_arguments(f, ng);

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

        unreachable();
      };

      auto rec = fix_lambda(rec_n);
      rec(root, rootos);

      return std::move(ng);
    }

    auto gen(
      structured_node_graph&& ng,
      const socket_handle& os,
      const node_definition_map& defs,
      arg_holder_map_t& arg_map,
      message_map& msgs)
      -> tl::optional<
        std::tuple<object_ptr<const Object>, class_env, location_map>>
    {
      auto root   = ng.node(os);
      auto rootos = os;

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
          throw no_valid_overloading(ng.get_source_id(os));

        auto insts = ds //
                     | rv::transform([](auto& d) { return d->instance(); })
                     | rn::to_vector;

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

          if (auto arg = get_arg_holder(arg_map, s)) {
            auto v = arg->compile();
            loc.add_location(v, s);
            ins.push_back(v);
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
          if (auto arg = get_arg_holder(arg_map, s)) {
            body = body << arg->compile();
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

      } catch (const message& msg) {
        // forward
        msgs.add(msg);

        // other
      } catch (const std::exception& e) {
        msgs.add(internal_compile_error(
          "Unknown std::exception detected: "s + e.what()));
      } catch (...) {
        msgs.add(internal_compile_error("Unknown exception detected"));
      }
      return tl::nullopt;
    }

    auto type(
      std::tuple<object_ptr<const Object>, class_env, location_map>&& p,
      message_map& msgs) -> tl::optional<executable>
    {
      try {

        auto [app, env, loc] = std::move(p);
        auto [ty, app2] =
          type_of_overloaded(app, std::move(env), std::move(loc));

        return executable(app2, ty);

        // normal errors
      } catch (const message& msg) {
        // forward
        msgs.add(msg);

        // others
      } catch (const std::exception& e) {
        msgs.add(internal_compile_error(
          "Unknown std::exception detected: "s + e.what()));
      } catch (...) {
        msgs.add(internal_compile_error("Unknown exception detected"));
      }
      return tl::nullopt;
    }

    auto output(executable&& exe, pipeline& pipe)
    {
      pipe.add_data("exe", std::move(exe));
      return tl::optional(true);
    }

  } // namespace

  void sema(pipeline& pipe)
  {
    init_logger();

    assert(pipe.get_data_if<message_map>("msg_map"));
    assert(pipe.get_data_if<structured_node_graph>("ng"));
    assert(pipe.get_data_if<socket_handle>("os"));
    assert(pipe.get_data_if<node_definition_map>("defs"));
    assert(pipe.get_data_if<node_declaration_map>("decls"));

    auto& msg_map = pipe.get_data<message_map>("msg_map");
    auto& ng      = pipe.get_data<structured_node_graph>("ng");
    auto& os      = pipe.get_data<socket_handle>("os");
    auto& defs    = pipe.get_data<node_definition_map>("defs");
    auto& decls   = pipe.get_data<node_declaration_map>("decls");

    auto arg_map = arg_holder_map_t();

    // clang-format off
    tl::make_optional(std::move(ng)) //
      .and_then([&](auto arg) { return desugar(std::move(arg), os, decls, arg_map, msg_map); })
      .and_then([&](auto arg) { return gen(std::move(arg), os, defs, arg_map, msg_map); })
      .and_then([&](auto arg) { return type(std::move(arg), msg_map); })
      .and_then([&](auto arg) { return output(std::move(arg), pipe); })
      .or_else([&] { pipe.set_failed(); });
    // clang-format on

    pipe.remove_data("ng");
    pipe.remove_data("os");
    pipe.remove_data("defs");
  }
} // namespace yave::compiler