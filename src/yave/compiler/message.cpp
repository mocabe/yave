//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/compiler/message.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/node/core/structured_node_graph.hpp>

#include <fmt/format.h>

namespace yave::compiler {

  std::string missing_input::text() const
  {
    return fmt::format(
      "Missing input connections: n={}, s={}",
      to_string(m_node_id),
      to_string(m_socket_id));
  }

  std::string missing_output::text() const
  {
    return fmt::format(
      "Missing output connections: n={}, s={}",
      to_string(m_node_id),
      to_string(m_socket_id));
  }

  std::string unexpected_parse_error::text() const
  {
    return fmt::format("Unexpected parse error occured: {}", m_str);
  }

  auto no_valid_overloading::text() const -> std::string
  {
    return fmt::format(
      "No valid overloading: socket={}", to_string(m_socket_id));
  }

  auto unexpected_type_error::text() const -> std::string
  {
    return fmt::format("Unexpected type error occured: {}", m_str);
  }

  auto type_missmatch::text() const -> std::string
  {
    return fmt::format(
      "Type missmatch: expected={} s={}, provided={} s={}",
      to_string(m_expected),
      to_string(m_socket_expected_id),
      to_string(m_provided),
      to_string(m_socket_provided_id));
  }

  auto unsolvable_constraints::text() const -> std::string
  {
    return fmt::format(
      "Unsolvable constraint: t1={}, s1={}, t2={}, s2={}",
      to_string(m_lhs_type),
      to_string(m_lhs_id),
      to_string(m_rhs_type),
      to_string(m_rhs_id));
  }

  auto invalid_output_type::text() const -> std::string
  {
    return fmt::format(
      "Invalid program output: should output {}, but the program returns {}",
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
        if (kind(msg) == message_kind::error)
          return true;
      }
      return false;
    }

    auto get_filtered_result(message_kind k)
    {
      std::vector<message> ret;

      for (auto&& msg : m_msgs) {
        if (kind(msg) == k) {
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

        if (auto nid = node_id(msg)) {

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

        if (auto sid = socket_id(msg)) {

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