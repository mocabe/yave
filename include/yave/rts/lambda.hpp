//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/support/id.hpp>

namespace yave {

  // ------------------------------------------
  // Variable

  struct variable_object_value
  {
    variable_object_value() noexcept
      : m_id {var_type::random_generate().id}
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
    static constexpr char name[] = "yave::Variable";
    static constexpr char uuid[] = "a2697d3d-3e17-452e-ab95-051797b4cf76";
  };

  template <>
  struct Variable::info_table_initializer
  {
    static auto get_info_table() -> const object_info_table*
    {
      return detail::add_variable_tag(&info_table);
    }

  private:
    alignas(32) inline static const object_info_table info_table {
      object_type<Variable>(),             //
      sizeof(Variable),                    //
      object_type_traits<Variable>::name,  //
      detail::vtbl_destroy_func<Variable>, //
      detail::vtbl_clone_func<Variable>};  //
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
    static constexpr char name[] = "yave::Lambda";
    static constexpr char uuid[] = "8295af91-2224-486f-9f62-c5c519595cf6";
  };

  template <>
  struct Lambda::info_table_initializer
  {
    static auto get_info_table() -> const object_info_table*
    {
      return detail::add_lambda_tag(&info_table);
    }

  private:
    alignas(32) inline static const object_info_table info_table {
      object_type<Lambda>(),             //
      sizeof(Lambda),                    //
      object_type_traits<Lambda>::name,  //
      detail::vtbl_destroy_func<Lambda>, //
      detail::vtbl_clone_func<Lambda>};  //
  };

} // namespace yave