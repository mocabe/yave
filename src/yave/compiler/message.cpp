//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/compiler/message.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/node/core/structured_node_graph.hpp>

#include <fmt/format.h>

namespace yave::compiler {

  namespace {

    auto default_pp(
      const structured_node_graph& ng,
      const node_handle& n,
      const socket_handle& s,
      const std::string& t)
    {
      auto ret = std::string();

      if (ng.exists(n)) {
        ret += fmt::format("[{}]", *ng.get_path(n));
      }

      if (ng.exists(s)) {
        ret += fmt::format("[{}]", *ng.get_name(s));
      }

      if (ret.empty())
        return t;

      ret += fmt::format(" {}", t);
      return ret;
    }
  } // namespace

  auto missing_input::pretty_print(const structured_node_graph& ng) const
    -> std::string
  {
    auto n = ng.node(m_node_id);
    auto s = ng.socket(m_socket_id);
    auto t = text();
    return default_pp(ng, n, s, t);
  }

  auto missing_output::pretty_print(const structured_node_graph& ng) const
    -> std::string
  {
    auto n = ng.node(m_node_id);
    auto s = ng.socket(m_socket_id);
    auto t = text();
    return default_pp(ng, n, s, t);
  }

  auto no_valid_overloading::pretty_print(const structured_node_graph& ng) const
    -> std::string
  {
    auto s = ng.socket(m_socket_id);
    auto n = ng.node(s);
    auto t = text();
    return default_pp(ng, n, s, t);
  }

  auto type_missmatch::pretty_print(const structured_node_graph& ng) const
    -> std::string
  {
    auto t  = text();
    auto s1 = ng.socket(m_socket_expected_id);
    auto n1 = ng.node(s1);
    auto s2 = ng.socket(m_socket_provided_id);
    auto n2 = ng.node(s2);

    auto ret = fmt::format(
      "{}: T1={}, T2={}", t, to_string(m_expected), to_string(m_provided));

    if (ng.exists(n1)) {
      ret += fmt::format("\n {}", default_pp(ng, n1, s1, "T1 requested here"));
    }
    if (ng.exists(n2)) {
      ret += fmt::format("\n {}", default_pp(ng, n2, s2, "T2 requested here"));
    }
    return ret;
  }

  auto unsolvable_constraints::pretty_print(
    const structured_node_graph& ng) const -> std::string
  {
    auto t  = text();
    auto s1 = ng.socket(m_lhs_id);
    auto n1 = ng.node(s1);
    auto s2 = ng.socket(m_rhs_id);
    auto n2 = ng.node(s2);

    auto ret = fmt::format(
      "{}: T1={}, T2={}", t, to_string(m_lhs_type), to_string(m_rhs_type));

    if (ng.exists(n1)) {
      ret += fmt::format("\n {}", default_pp(ng, n1, s1, "T1 requested here"));
    }
    if (ng.exists(n2)) {
      ret += fmt::format("\n {}", default_pp(ng, n2, s2, "T2 requested here"));
    }
    return ret;
  }

  auto invalid_output_type::pretty_print(const structured_node_graph& ng) const
    -> std::string
  {
    return fmt::format(
      "{}: expected type {} but program returns {}",
      text(),
      to_string(m_expected),
      to_string(m_provided));
  }

  class message_map::impl
  {
    /// messages
    std::vector<message> m_msgs;

  public:
    void add(message msg)
    {
      m_msgs.push_back(std::move(msg));
    }

    bool has_error() const
    {
      for (auto&& msg : m_msgs) {
        if (msg.kind() == message_kind::error)
          return true;
      }
      return false;
    }

    auto get_filtered_result(message_kind k)
    {
      std::vector<message> ret;

      for (auto&& msg : m_msgs) {
        if (msg.kind() == k) {
          ret.push_back(msg);
        }
      }
      return ret;
    }

    auto get_results() const
    {
      return m_msgs;
    }

    auto get_results(const structured_node_graph& ng, const node_handle& n)
      const
    {
      std::vector<message> ret;

      for (auto&& msg : m_msgs) {

        if (auto nid = msg.get_node_id()) {

          auto h = ng.node(*nid);

          // need to check gruop relation
          if (h == n || ng.is_parent_of(n, h)) {
            ret.push_back(msg);
          }
        }
      }
      return ret;
    }

    auto get_results(const structured_node_graph& ng, const socket_handle& s)
    {
      std::vector<message> ret;

      for (auto&& msg : m_msgs) {

        if (auto sid = msg.get_socket_id()) {

          auto h = ng.socket(*sid);

          if (h == s) {
            ret.push_back(msg);
          }
        }
      }
      return ret;
    }
  };

  message_map::message_map()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  message_map::~message_map() noexcept             = default;
  message_map::message_map(message_map&&) noexcept = default;
  message_map& message_map::operator=(message_map&&) noexcept = default;

  void message_map::add(message msg)
  {
    m_pimpl->add(std::move(msg));
  }

  bool message_map::has_error() const
  {
    return m_pimpl->has_error();
  }

  auto message_map::get_errors() const -> std::vector<message>
  {
    return m_pimpl->get_filtered_result(message_kind::error);
  }
  auto message_map::get_warnings() const -> std::vector<message>
  {
    return m_pimpl->get_filtered_result(message_kind::warning);
  }
  auto message_map::get_infos() const -> std::vector<message>
  {
    return m_pimpl->get_filtered_result(message_kind::info);
  }

  auto message_map::get_results() const -> std::vector<message>
  {
    return m_pimpl->get_results();
  }

  auto message_map::get_results(
    const structured_node_graph& ng,
    const node_handle& n) const -> std::vector<message>
  {
    return m_pimpl->get_results(ng, n);
  }
  auto message_map::get_results(
    const structured_node_graph& ng,
    const socket_handle& s) const -> std::vector<message>
  {
    return m_pimpl->get_results(ng, s);
  }
}