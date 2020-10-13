//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/compiler/compile.hpp>
#include <yave/compiler/message.hpp>
#include <yave/support/log.hpp>

#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
#include <tl/optional.hpp>

#include <map>

YAVE_DECL_G_LOGGER(parse)

namespace yave::compiler {

  namespace {

    namespace rn = ranges;
    namespace rv = ranges::views;

    using monad = tl::optional<int>;

    [[nodiscard]] auto pass() -> monad
    {
      return tl::optional(42);
    }

    [[nodiscard]] auto fail() -> monad
    {
      return tl::nullopt;
    }

    auto check(
      structured_node_graph& ng,
      const socket_handle& out_socket,
      message_map& msgs) -> monad
    {
      auto out_node = ng.node(out_socket);

      if (!ng.is_group(out_node) && !ng.is_function(out_node)) {
        msgs.add(
          unexpected_parse_error("Out socket is not of a function or a group"));
        return fail();
      }

      // node-id -> [socket-id] mapping
      using memo = std::map<uid, std::vector<uid>>;
      auto m     = memo();

      auto marked = [&](const node_handle& n, const socket_handle& s) -> bool {
        if (auto it = m.find(n.id()); it != m.end())
          return rn::find(it->second, s.id()) != it->second.end();
        return false;
      };

      auto mark = [&](const node_handle& n, const socket_handle& s) -> void {
        assert(!marked(n, s));
        if (auto it = m.find(n.id()); it != m.end())
          it->second.push_back(s.id());
        else
          m.insert({n.id(), {s.id()}});
      };

      // general node check
      auto check_n = [&](
                       const node_handle& n,
                       const socket_handle& os,
                       const structured_node_graph& ng) -> void {
        // IO will be checked by rec_g
        assert(ng.is_group_member(n));

        if (marked(n, os))
          return;

        auto ics = ng.input_connections(n);
        auto iss = ng.input_sockets(n);
        auto oss = ng.output_sockets(n);

        // socket has input connection
        auto has_connection = [&](auto&& s) -> bool {
          return !ng.connections(s).empty();
        };

        // socket has default argument
        auto has_default = [&](auto&& s) -> bool {
          return ng.get_data(s) != nullptr;
        };

        // missing socket connection
        auto missing = [&](auto&& s) {
          return !has_connection(s) && !has_default(s);
        };

        size_t n_missing = rn::count_if(iss, missing);

        if (n_missing != iss.size()) {
          for (auto&& s : iss) {
            if (has_default(s))
              msgs.add(has_default_argument(n, s));

            if (has_connection(s))
              msgs.add(has_input_connection(n, s));

            if (missing(s))
              msgs.add(missing_input(n, s));
          }
        }

        if (!iss.empty() && n_missing == iss.size()) {
          msgs.add(is_lambda_node(n));
        }

        for (auto&& s : oss) {
          if (has_connection(s))
            msgs.add(has_output_connection(n, s));
        }

        mark(n, os);
      };

      // check io handler
      auto check_io = [&](
                        const node_handle& n,
                        const socket_handle& os,
                        const structured_node_graph& ng) -> void {
        assert(!ng.is_group_member(n));

        if (ng.is_group_input(n)) {
          auto idx = *ng.get_index(os);
          auto s   = ng.output_sockets(n)[idx];

          if (!ng.connections(s).empty())
            msgs.add(has_input_connection(n, s));
        }

        if (ng.is_group_output(n)) {
          auto idx = *ng.get_index(os);
          auto s   = ng.input_sockets(n)[idx];

          if (ng.connections(s).empty())
            msgs.add(missing_input(n, s));
          else
            msgs.add(has_input_connection(n, s));
        }
      };

      // check group and its inside
      auto rec_g = [&](
                     auto&& rec_n,
                     const node_handle& g,
                     const socket_handle& os,
                     const structured_node_graph& ng) -> void {
        assert(ng.is_group(g));

        // check interface
        check_n(g, os, ng);

        // check input
        for (auto&& c : ng.input_connections(g)) {
          auto ci = ng.get_info(c);
          rec_n(ci->src_node(), ci->src_socket(), ng);
        }

        // check inside
        {
          auto go  = ng.get_group_output(g);
          auto idx = *ng.get_index(os);
          auto s   = ng.input_sockets(go)[idx];

          check_io(go, os, ng);

          for (auto&& c : ng.connections(s)) {
            auto ci = ng.get_info(c);
            rec_n(ci->src_node(), ci->src_socket(), ng);
          }
        }
      };

      // check function
      auto rec_f = [&](
                     auto&& rec_n,
                     const node_handle& f,
                     const socket_handle& s,
                     const structured_node_graph& ng) -> void {
        check_n(f, s, ng);

        // check input
        for (auto&& c : ng.input_connections(f)) {
          auto ci = ng.get_info(c);
          rec_n(ci->src_node(), ci->src_socket(), ng);
        }
      };

      // check node recursively
      auto rec_n = [&](
                     auto&& self,
                     const node_handle& n,
                     const socket_handle& os,
                     const structured_node_graph& ng) -> void {
        if (ng.is_group(n))
          return rec_g(self, n, os, ng);

        if (ng.is_function(n))
          return rec_f(self, n, os, ng);

        if (ng.is_group_input(n))
          return check_io(n, os, ng);

        unreachable();
      };

      auto rec = fix_lambda(rec_n);
      rec(out_node, out_socket, ng);

      if (!msgs.has_error())
        return pass();

      return fail();
    }

  } // namespace

  void parse(pipeline& pipe)
  {
    init_logger();

    assert(pipe.get_data_if<message_map>("msg_map"));
    assert(pipe.get_data_if<structured_node_graph>("ng"));
    assert(pipe.get_data_if<socket_handle>("os"));

    auto& msg_map = pipe.get_data<message_map>("msg_map");
    auto& ng      = pipe.get_data<structured_node_graph>("ng");
    auto& os      = pipe.get_data<socket_handle>("os");

    pass() //
      .and_then([&](auto) { return check(ng, os, msg_map); })
      .or_else([&] { pipe.set_failed(); });
  }
}