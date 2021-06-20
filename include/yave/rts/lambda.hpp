//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/core/id.hpp>

namespace yave {

  // ------------------------------------------
  // Variable

  struct variable_object_value
  {
    variable_object_value() noexcept
      : m_id {tvar_type::random_generate().id}
    {
    }

    variable_object_value(uint64_t id) noexcept
      : m_id {id}
    {
    }

    [[nodiscard]] auto id() const noexcept
    {
      return m_id;
    }

  private:
    uint64_t m_id;
  };

  /// Generic variable for lambda abstraction
  using Variable = Box<variable_object_value>;

  template <>
  struct object_type_traits<yave::Variable>
  {
    static constexpr auto info_table_tag = detail::info_table_tags::_3;
    static constexpr char name[]         = "yave::Variable";
    static constexpr char uuid[] = "a2697d3d-3e17-452e-ab95-051797b4cf76";
  };

  // ------------------------------------------
  // Lambda

  struct lambda_object_value_storage
  {
    /// pointer to variable object
    object_ptr<const Variable> var;
    /// pointer to body of lambda
    object_ptr<const Object> body;
  };

  /// value of Lambda
  struct lambda_object_value
  {
    [[nodiscard]] friend inline auto _get_storage(
      const lambda_object_value& v) noexcept
      -> const lambda_object_value_storage&
    {
      return v.m_storage;
    }

    [[nodiscard]] friend inline auto _get_storage(
      lambda_object_value& v) noexcept -> lambda_object_value_storage&
    {
      return v.m_storage;
    }

    lambda_object_value() noexcept
      : m_storage {}
    {
    }

    lambda_object_value(
      object_ptr<const Variable> var,
      object_ptr<const Object> body) noexcept
      : m_storage {std::move(var), std::move(body)}
    {
    }

  private:
    lambda_object_value_storage m_storage;
  };

  /// Generic lambda abstraction node
  /// TODO: Support templated version like TApply
  using Lambda = Box<lambda_object_value>;

  template <>
  struct object_type_traits<yave::Lambda>
  {
    static constexpr auto info_table_tag = detail::info_table_tags::_4;
    static constexpr char name[]         = "yave::Lambda";
    static constexpr char uuid[] = "8295af91-2224-486f-9f62-c5c519595cf6";
  };

} // namespace yave