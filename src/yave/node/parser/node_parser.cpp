//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/node_parser.hpp>
#include <yave/node/parser/errors.hpp>
#include <yave/node/core/node_group.hpp>
#include <yave/support/log.hpp>

#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
#include <range/v3/action.hpp>
#include <tl/optional.hpp>

YAVE_DECL_G_LOGGER(node_parser)

// cursed, but very useful
#define mem_fn(FN, ...) \
  [&](auto&& arg) { return FN(std::forward<decltype(arg)>(arg), __VA_ARGS__); }

namespace yave {

  namespace rn = ranges;
  namespace rv = ranges::views;
  namespace ra = ranges::actions;

  // tl::optional -> std::optional
  template <class T>
  constexpr auto to_std(tl::optional<T>&& opt) -> std::optional<T>
  {
    if (opt)
      return std::move(*opt);
    else
      return std::nullopt;
  }

  class node_parser::impl
  {
  public:
    impl()           = default;
    ~impl() noexcept = default;

    auto validate(
      structured_node_graph&& ng,
      const socket_handle& out_socket,
      error_list& errors) -> tl::optional<structured_node_graph>
    {
      if (!ng.exists(out_socket)) {
        errors.push_back(make_error<parse_error::unexpected_error>(
          "Out socket does not exist"));
        return tl::nullopt;
      }

      auto out_node = ng.node(out_socket);

      if (!ng.is_group(out_node) && !ng.is_function(out_node)) {
        errors.push_back(make_error<parse_error::unexpected_error>(
          "Out socket is not of a function or a group"));
        return tl::nullopt;
      }

      // node -> [socket]
      using memo = std::map<uid, std::vector<uid>>;

      auto marked =
        [](
          const node_handle& n, const socket_handle& s, const memo& m) -> bool {
        if (auto it = m.find(n.id()); it != m.end())
          return rn::find(it->second, s.id()) != it->second.end();
        return false;
      };

      auto mark =
        [&](const node_handle& n, const socket_handle& s, memo& m) -> void {
        assert(!marked(n, s, m));
        if (auto it = m.find(n.id()); it != m.end())
          it->second.push_back(s.id());
        else
          m.insert({n.id(), {s.id()}});
      };

      // general node check
      auto chk_n = [&](
                     const node_handle& n,
                     const socket_handle& os,
                     const structured_node_graph& ng,
                     error_list& es,
                     memo& m) -> void {

        // IO will be checked by rec_g
        assert(ng.is_group_member(n));

        if (marked(n, os, m))
          return;

        auto ics = ng.input_connections(n);
        auto iss = ng.input_sockets(n);

        auto missing = [&](auto&& s) {
          return !ng.get_data(s) && ng.connections(s).empty();
        };

        size_t cnt = rn::count_if(iss, missing);

        if (cnt != iss.size())
          for (auto&& s : iss)
            if (missing(s))
              es.push_back(
                make_error<parse_error::missing_input>(n.id(), s.id()));

        mark(n, os, m);
      };

      // check group and its inside
      auto rec_g = [&](
                     auto&& rec_n,
                     const node_handle& g,
                     const socket_handle& os,
                     const structured_node_graph& ng,
                     error_list& es,
                     memo& m) -> void {
        assert(ng.is_group(g));

        // check interface
        chk_n(g, os, ng, es, m);

        // check input
        for (auto&& c : ng.input_connections(g)) {
          auto ci = ng.get_info(c);
          rec_n(ci->src_node(), ci->src_socket(), ng, es, m);
        }

        // check inside
        {
          auto go  = ng.get_group_output(g);
          auto idx = *ng.get_index(os);
          auto s   = ng.input_sockets(go)[idx];

          if (ng.connections(s).empty())
            es.push_back(
              make_error<parse_error::missing_input>(go.id(), s.id()));

          for (auto&& c : ng.connections(s)) {
            auto ci = ng.get_info(c);
            rec_n(ci->src_node(), ci->src_socket(), ng, es, m);
          }
        }
      };

      // check function
      auto rec_f = [&](
                     auto&& rec_n,
                     const node_handle& f,
                     const socket_handle& s,
                     const structured_node_graph& ng,
                     error_list& es,
                     memo& m) -> void {
        chk_n(f, s, ng, es, m);

        // check input
        for (auto&& c : ng.input_connections(f)) {
          auto ci = ng.get_info(c);
          rec_n(ci->src_node(), ci->src_socket(), ng, es, m);
        }
      };

      auto rec_n = [&](
                     auto&& self,
                     const node_handle& n,
                     const socket_handle& os,
                     const structured_node_graph& ng,
                     error_list& es,
                     memo& m) -> void {
        if (ng.is_group(n))
          return rec_g(self, n, os, ng, es, m);

        if (ng.is_function(n))
          return rec_f(self, n, os, ng, es, m);

        if (ng.is_group_input(n))
          return;

        unreachable();
      };

      auto m   = memo();
      auto rec = fix_lambda(rec_n);
      rec(out_node, out_socket, ng, errors, m);

      if (errors.empty())
        return std::move(ng);

      return tl::nullopt;
    }

  public:
    auto parse(structured_node_graph&& ng, const socket_handle& out)
      -> node_parser_result
    {
      node_parser_result result;

      result.node_graph =
        to_std(tl::make_optional(std::move(ng))
                 .and_then(mem_fn(validate, out, result.errors))
                 .or_else([&] {
                   Error(g_logger, "Failed to parse node graph");
                   for (auto&& e : result.errors) {
                     Error(g_logger, "error: {}", e.message());
                   }
                 }));

      return result;
    }
  };

  node_parser::node_parser()
    : m_pimpl {std::make_unique<impl>()}
  {
    init_logger();
  }

  node_parser::~node_parser() noexcept = default;

  auto node_parser::parse(structured_node_graph&& ng, const socket_handle& out)
    -> node_parser_result
  {
    return m_pimpl->parse(std::move(ng), out);
  }

} // namespace yave