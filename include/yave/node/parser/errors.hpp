//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/rts.hpp>
#include <yave/node/core/bind_info.hpp>
#include <yave/node/core/node_handle.hpp>
#include <yave/support/error.hpp>

#include <vector>
#include <memory>

namespace yave {

  namespace parse_error {

    struct no_sufficient_input : error_info<no_sufficient_input>
    {
      no_sufficient_input(const node_handle& node)
        : m_node {node}
      {
      }

      /// Error message.
      [[nodiscard]] virtual std::string message() const override;

      /// Get node.
      [[nodiscard]] const node_handle& node() const
      {
        return m_node;
      }

    private:
      node_handle m_node;
    };

    struct unexpected_error : error_info<unexpected_error>
    {
      unexpected_error(const std::string& msg)
        : m_msg {msg}
      {
      }

      /// Error message.
      [[nodiscard]] virtual std::string message() const override;

    private:
      std::string m_msg;
    };

  } // namespace parse_error
} // namespace yave
