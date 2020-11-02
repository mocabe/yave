//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <variant>

namespace yave {

  /// variant mixin helper
  template <class... Ts>
  struct variant_mixin : std::variant<Ts...>
  {
    /// base variant type
    using variant_type = std::variant<Ts...>;

    /// inherit ctor
    using variant_type::variant_type;

    /// ref as variant_type
    [[nodiscard]] auto& as_variant() noexcept
    {
      return *static_cast<variant_type*>(this);
    }

    /// ref as variant_type
    [[nodiscard]] auto& as_variant() const noexcept
    {
      return *static_cast<const variant_type*>(this);
    }

    /// self visit
    template <class F>
    [[nodiscard]] auto visit(F&& f)
    {
      return std::visit(std::forward<F>(f), as_variant());
    }

    /// self visit
    template <class F>
    [[nodiscard]] auto visit(F&& f) const
    {
      return std::visit(std::forward<F>(f), as_variant());
    }

    /// self get
    template <class T>
    [[nodiscard]] auto& get()
    {
      return std::get<T>(as_variant());
    }

    /// sefl get
    template <class T>
    [[nodiscard]] auto& get() const
    {
      return std::get<T>(as_variant());
    }

    /// self get_if
    template <class T>
    [[nodiscard]] auto* get_if()
    {
      return std::get_if<T>(&as_variant());
    }

    /// sefl get_if
    template <class T>
    [[nodiscard]] auto* get_if() const
    {
      return std::get_if<T>(&as_variant());
    }
  };
} // namespace yave