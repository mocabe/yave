//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/node_parser.hpp>
#include <yave/support/log.hpp>

#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
#include <range/v3/action.hpp>
#include <tl/optional.hpp>

#include <map>

YAVE_DECL_G_LOGGER(node_parser)

namespace yave {

  namespace rn = ranges;
  namespace rv = ranges::views;
  namespace ra = ranges::actions;

  class node_parser_result::impl
  {
    /// node graph
    std::optional<structured_node_graph> m_ng;
    /// result map
    std::vector<parse_result> m_results;

  public:
    impl() = default;

  public:
    bool success() const
    {
      return m_ng.has_value();
    }

    auto take_node_graph()
    {
      return std::move(m_ng);
    }

    void set_node_graph(structured_node_graph&& g)
    {
      m_ng = std::move(g);
    }

  public:
    void add_result(parse_result r)
    {
      m_results.push_back(std::move(r));
    }

    bool has_error()
    {
      return std::find_if(
               m_results.begin(),
               m_results.end(),
               [](auto&& r) { return type(r) == parse_result_type::error; })
             != m_results.end();
    }

    auto get_filtered_result(parse_result_type ty)
    {
      std::vector<parse_result> ret;

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
      std::vector<parse_result> ret;

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
      std::vector<parse_result> ret;

      for (auto&& r : m_results) {

        auto h = ng.socket(socket_id(r));

        if (h == s) {
          ret.push_back(r);
        }
      }
      return ret;
    }
  };

  node_parser_result::node_parser_result()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  // clang-format off
  node_parser_result::~node_parser_result() noexcept = default;
  node_parser_result::node_parser_result(node_parser_result&&) noexcept = default;
  node_parser_result& node_parser_result::operator=(node_parser_result&&) noexcept = default;
  // clang-format on

  bool node_parser_result::success() const
  {
    return m_pimpl->success();
  }

  auto node_parser_result::take_node_graph()
    -> std::optional<structured_node_graph>
  {
    return m_pimpl->take_node_graph();
  }

  bool node_parser_result::has_error() const
  {
    return m_pimpl->has_error();
  }

  auto node_parser_result::get_errors() const -> std::vector<parse_result>
  {
    return m_pimpl->get_filtered_result(parse_result_type::error);
  }

  auto node_parser_result::get_warnings() const -> std::vector<parse_result>
  {
    return m_pimpl->get_filtered_result(parse_result_type::warning);
  }

  auto node_parser_result::get_infos() const -> std::vector<parse_result>
  {
    return m_pimpl->get_filtered_result(parse_result_type::info);
  }

  auto node_parser_result::get_results(
    const structured_node_graph& ng,
    const node_handle& n) const -> std::vector<parse_result>
  {
    return m_pimpl->get_results(ng, n);
  }

  auto node_parser_result::get_results(
    const structured_node_graph& ng,
    const socket_handle& s) const -> std::vector<parse_result>
  {
    return m_pimpl->get_results(ng, s);
  }

  void node_parser_result::add_result(parse_result r)
  {
    m_pimpl->add_result(std::move(r));
  }

  void node_parser_result::set_node_graph(structured_node_graph&& ng)
  {
    m_pimpl->set_node_graph(std::move(ng));
  }

  class node_parser::impl
  {
  public:
    impl()           = default;
    ~impl() noexcept = default;

    auto check(
      structured_node_graph&& ng,
      const socket_handle& out_socket,
      node_parser_result& result) -> tl::optional<structured_node_graph>
    {
      using namespace parse_results;

      if (!ng.exists(out_socket)) {
        result.add_result(unexpected_error("Out socket does not exists"));
        return tl::nullopt;
      }

      auto out_node = ng.node(out_socket);

      if (!ng.is_group(out_node) && !ng.is_function(out_node)) {
        result.add_result(
          unexpected_error("Out socket is not of a function or a group"));
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
      auto check_n = [&](
                       const node_handle& n,
                       const socket_handle& os,
                       const structured_node_graph& ng,
                       node_parser_result& res,
                       memo& m) -> void {
        // IO will be checked by rec_g
        assert(ng.is_group_member(n));

        if (marked(n, os, m))
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
              res.add_result(has_default_argument(n, s));

            if (has_connection(s))
              res.add_result(has_input_connection(n, s));

            if (missing(s))
              res.add_result(missing_input(n, s));
          }
        }

        if (!iss.empty() && n_missing == iss.size()) {
          res.add_result(is_lambda_node(n));
        }

        for (auto&& s : oss) {
          if (has_connection(s))
            res.add_result(has_output_connection(n, s));
        }

        mark(n, os, m);
      };

      // check io handler
      auto check_io = [&](
                        const node_handle& n,
                        const socket_handle& os,
                        const structured_node_graph& ng,
                        node_parser_result& res) -> void {
        assert(!ng.is_group_member(n));

        if (ng.is_group_input(n)) {
          auto idx = *ng.get_index(os);
          auto s   = ng.output_sockets(n)[idx];

          if (!ng.connections(s).empty())
            res.add_result(has_input_connection(n, s));
        }

        if (ng.is_group_output(n)) {
          auto idx = *ng.get_index(os);
          auto s   = ng.input_sockets(n)[idx];

          if (ng.connections(s).empty())
            res.add_result(missing_input(n, s));
          else
            res.add_result(has_input_connection(n, s));
        }
      };

      // check group and its inside
      auto rec_g = [&](
                     auto&& rec_n,
                     const node_handle& g,
                     const socket_handle& os,
                     const structured_node_graph& ng,
                     node_parser_result& res,
                     memo& m) -> void {
        assert(ng.is_group(g));

        // check interface
        check_n(g, os, ng, res, m);

        // check input
        for (auto&& c : ng.input_connections(g)) {
          auto ci = ng.get_info(c);
          rec_n(ci->src_node(), ci->src_socket(), ng, res, m);
        }

        // check inside
        {
          auto go  = ng.get_group_output(g);
          auto idx = *ng.get_index(os);
          auto s   = ng.input_sockets(go)[idx];

          check_io(go, os, ng, res);

          for (auto&& c : ng.connections(s)) {
            auto ci = ng.get_info(c);
            rec_n(ci->src_node(), ci->src_socket(), ng, res, m);
          }
        }
      };

      // check function
      auto rec_f = [&](
                     auto&& rec_n,
                     const node_handle& f,
                     const socket_handle& s,
                     const structured_node_graph& ng,
                     node_parser_result& res,
                     memo& m) -> void {
        check_n(f, s, ng, res, m);

        // check input
        for (auto&& c : ng.input_connections(f)) {
          auto ci = ng.get_info(c);
          rec_n(ci->src_node(), ci->src_socket(), ng, res, m);
        }
      };

      // check node recursively
      auto rec_n = [&](
                     auto&& self,
                     const node_handle& n,
                     const socket_handle& os,
                     const structured_node_graph& ng,
                     node_parser_result& res,
                     memo& m) -> void {
        if (ng.is_group(n))
          return rec_g(self, n, os, ng, res, m);

        if (ng.is_function(n))
          return rec_f(self, n, os, ng, res, m);

        if (ng.is_group_input(n))
          return check_io(n, os, ng, res);

        unreachable();
      };

      auto m   = memo();
      auto rec = fix_lambda(rec_n);
      rec(out_node, out_socket, ng, result, m);

      if (!result.has_error())
        return std::move(ng);

      return tl::nullopt;
    }

    auto set_result(structured_node_graph&& ng, node_parser_result& res)
    {
      res.set_node_graph(std::move(ng));
      return tl::make_optional(nullptr);
    }

  public:
    auto parse(structured_node_graph&& ng, const socket_handle& out)
      -> node_parser_result
    {
      node_parser_result result;

      // pipeline
      tl::make_optional(std::move(ng))
        .and_then([&](auto arg) { return check(std::move(arg), out, result); })
        .and_then([&](auto arg) { return set_result(std::move(arg), result); });

      return result;
    }
  };

  node_parser::node_parser()
    : m_pimpl {std::make_unique<impl>()}
  {
    init_logger();
  }

  node_parser::~node_parser() noexcept = default;

  auto node_parser::parse(params p) -> node_parser_result
  {
    return m_pimpl->parse(std::move(p.node_graph), p.output_socket);
  }

} // namespace yave