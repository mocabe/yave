//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>

namespace yave {

  // ------------------------------------------
  // apply object storage

  struct apply_object_value_storage
  {
    apply_object_value_storage(
      object_ptr<const Object> app,
      object_ptr<const Object> arg) noexcept
      : m_app {std::move(app)}
      , m_arg {std::move(arg)}
    {
    }

    [[nodiscard]] auto app() const noexcept -> object_ptr<const Object>&
    {
      assert(!is_result());
      return m_app;
    }

    [[nodiscard]] auto arg() const noexcept -> object_ptr<const Object>&
    {
      assert(!is_result());
      return m_arg;
    }

    [[nodiscard]] bool is_result() const noexcept
    {
      return m_app == nullptr;
    }

    /// get cache of object
    [[nodiscard]] auto get_result() const noexcept -> object_ptr<const Object>
    {
      assert(is_result());
      return m_arg;
    }

    /// set cache of object
    void set_result(const object_ptr<const Object>& obj) const noexcept
    {
      assert(!is_result());
      m_app = nullptr;
      m_arg = obj;
    }

  private:
    /// closure
    mutable object_ptr<const Object> m_app;
    /// argument
    mutable object_ptr<const Object> m_arg;
  };

  /// value of Apply
  class apply_object_value
  {
  public:
    template <class App, class Arg>
    apply_object_value(object_ptr<App> app, object_ptr<Arg> arg) noexcept
      : m_storage {std::move(app), std::move(arg)}
    {
    }

    [[nodiscard]] friend inline auto _get_storage(
      const apply_object_value& v) noexcept -> const apply_object_value_storage&
    {
      return v.m_storage;
    }

    [[nodiscard]] friend inline auto _get_storage(
      apply_object_value& v) noexcept -> apply_object_value_storage&
    {
      return v.m_storage;
    }

  private:
    apply_object_value_storage m_storage;
  };

  /// runtime apply object
  using Apply = Box<apply_object_value>;

  template <>
  struct object_type_traits<yave::Apply>
  {
    static constexpr auto info_table_tag = detail::info_table_tags::_2;
    static constexpr char name[]         = "yave::Apply";
    static constexpr char uuid[] = "2db5ddcd-0d6d-4f2f-8fd5-7b30abfc68eb";
  };

  /// apply object with compile time AST info
  template <class App, class Arg>
  struct TApply : Apply
  {
    /// term
    static constexpr auto term =
      make_tm_apply(get_term<App>(), get_term<Arg>());

    TApply(object_ptr<App> ap, object_ptr<Arg> ar) noexcept
      : Apply(std::move(ap), std::move(ar))
    {
    }
  };

  /// apply operator
  template <class T1, class T2>
  [[nodiscard]] auto operator<<(object_ptr<T1> lhs, object_ptr<T2> rhs)
  {
    using newT = decltype(TApply(lhs, rhs));
    return make_object<newT>(std::move(lhs), std::move(rhs));
  }

} // namespace yave