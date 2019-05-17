//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_handle.hpp>
#include <yave/core/rts/object_ptr.hpp>

#include <stdexcept>
#include <string>

namespace yave {

  namespace parse_error {

    /// Base class of parser errors.
    struct parse_error : std::logic_error
    {
      /// Ctor.
      parse_error(const std::string& message, const node_handle& h);
      /// Ctor.
      parse_error(const char* message, const node_handle& h);
      /// Get handle of node throwing this exception.
      [[nodiscard]] node_handle node() const noexcept;

    protected:
      /// Node
      node_handle m_node;
    };

    /// Parser error: No Overloading.
    struct no_overloading : parse_error
    {
      /// Ctor.
      no_overloading(
        const std::string& msg,
        const node_handle& h,
        const object_ptr<const Type>& tp);

      /// Get type of node throwing this exception.
      [[nodiscard]] object_ptr<const Type> type() const;

    private:
      /// Type
      object_ptr<const Type> m_type;
    };

    /// Parser error: Ambiguous Overloading.
    struct ambiguous_overloading : parse_error
    {
      /// Ctor.
      ambiguous_overloading(
        const std::string& msg,
        const object_ptr<const Type>& tp,
        const node_handle& h);

      /// Get type of node throwing this exception.
      [[nodiscard]] object_ptr<const Type> type() const;

    private:
      object_ptr<const Type> m_type;
    };

    /// Parser error: Empty Tree.
    struct empty_tree : parse_error
    {
      using parse_error::parse_error;
    };

    /// Parser error: Root Type Missmatch.
    struct root_type_missmatch : parse_error
    {
      /// Ctor.
      root_type_missmatch(
        const std::string& msg,
        const node_handle& h,
        const object_ptr<const Type>& expected,
        const object_ptr<const Type>& provided);

      /// Get expected root type.
      [[nodiscard]] object_ptr<const Type> expected() const;

      /// Get provided root type.
      [[nodiscard]] object_ptr<const Type> provided() const;

    private:
      /// Expected type
      object_ptr<const Type> m_expected;
      /// Provided type
      object_ptr<const Type> m_provided;
    };

  } // namespace parse_error

} // namespace yave