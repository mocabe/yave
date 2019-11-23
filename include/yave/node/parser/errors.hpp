//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/rts.hpp>
#include <yave/node/core/node_handle.hpp>
#include <yave/support/error.hpp>

#include <vector>
#include <memory>

namespace yave {

  namespace parse_error {

    /// No sufficient inputs for non-primitive node
    struct no_sufficient_input : error_info<no_sufficient_input>
    {
      no_sufficient_input(const uid& id)
        : m_id {id}
      {
      }

      /// Error message
      [[nodiscard]] auto message() const -> std::string override;

      /// Get node ID
      [[nodiscard]] auto id() const -> const uid&
      {
        return m_id;
      }

    private:
      uid m_id ;
    };

    /// Unexpected parser error
    struct unexpected_error : error_info<unexpected_error>
    {
      unexpected_error(const std::string& msg)
        : m_msg {msg}
      {
      }

      /// Error message
      [[nodiscard]] auto message() const -> std::string override;

    private:
      std::string m_msg;
    };

  } // namespace parse_error
} // namespace yave
