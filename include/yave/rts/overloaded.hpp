//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>

namespace yave {

  struct overloaded_object_value_storage
  {
    overloaded_object_value_storage(
      uint64_t id = var_type::random_generate().id) noexcept
      : m_id {id}
    {
    }

    [[nodiscard]] auto class_id() const noexcept
    {
      return m_id;
    }

  private:
    uint64_t m_id;
  };

  struct overloaded_object_value
  {
    [[nodiscard]] friend inline auto _get_storage(
      const overloaded_object_value& v) noexcept
      -> const overloaded_object_value_storage&
    {
      return v.m_storage;
    }

    [[nodiscard]] friend inline auto _get_storage(
      overloaded_object_value& v) noexcept -> overloaded_object_value_storage&
    {
      return v.m_storage;
    }

  private:
    overloaded_object_value_storage m_storage;
  };

  /// Overloaded node
  using Overloaded = Box<overloaded_object_value>;

} // namespace yave

YAVE_DECL_TYPE(yave::Overloaded, "a1d282e3-e68e-49ce-ab8f-afe90cfe7a78");