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
    uint64_t id = var_type::random_generate().id;
  };

  /// Generic variable for lambda abstraction
  using Variable = Box<variable_object_value>;

  // ------------------------------------------
  // Lambda

  struct lambda_object_value_storage
  {
    /// pointer to variable object
    object_ptr<const Variable> var;
    /// pointer to body of lambda
    object_ptr<const Object> body;
  };

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

  private:
    lambda_object_value_storage m_storage;
  };

  /// Generic lambda abstraction node
  using Lambda = Box<lambda_object_value>;

} // namespace yave

YAVE_DECL_TYPE(yave::Lambda, "8295af91-2224-486f-9f62-c5c519595cf6");
YAVE_DECL_TYPE(yave::Variable, "a2697d3d-3e17-452e-ab95-051797b4cf76");