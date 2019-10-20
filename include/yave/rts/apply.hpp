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

    [[nodiscard]] auto& app() const noexcept
    {
      assert(!is_result());
      return m_app;
    }

    [[nodiscard]] auto& arg() const noexcept
    {
      assert(!is_result());
      return m_arg;
    }

    [[nodiscard]] bool is_result() const noexcept
    {
      return m_app == nullptr;
    }

    /// get cache of object
    [[nodiscard]] auto get_result() const noexcept
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
    template <
      class App,
      class Arg,
      class = std::enable_if_t<
        !std::is_same_v<std::decay_t<App>, apply_object_value>>>
    apply_object_value(App&& app, Arg&& arg) noexcept
      : m_storage {std::forward<App>(app), std::forward<Arg>(arg)}
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

  // type info of Apply
  template <>
  struct object_type_traits<yave::Apply>
  {
    static constexpr char name[] = "yave::Apply";
    static constexpr char uuid[] = "2db5ddcd-0d6d-4f2f-8fd5-7b30abfc68eb";
  };

  // tagged info table
  template <>
  struct Apply::info_table_initializer
  {
    /// get info table pointer
    static const object_info_table* get_info_table()
    {
      // add apply tag
      return detail::add_apply_tag(&info_table);
    }

  private:
    /// static object info table
    alignas(32) inline static const object_info_table info_table {
      object_type<Apply>(),            //
      sizeof(Apply),                   //
      object_type_traits<Apply>::name, //
      vtbl_destroy_func<Apply>,        //
      vtbl_clone_func<Apply>};         //
  };

  /// compile time apply object
  template <class App, class Arg>
  struct TApply : Apply
  {
    /// base
    using base = Apply;
    /// term
    static constexpr auto term =
      make_tm_apply(get_term<App>(), get_term<Arg>());

    TApply(object_ptr<App> ap, object_ptr<Arg> ar) noexcept
      : base(std::move(ap), std::move(ar))
    {
    }

    TApply(App* ap, Arg* ar) noexcept
      : base(object_ptr<const Object>(ap), object_ptr<const Object>(ar))
    {
    }

    TApply(App* ap, object_ptr<Arg> ar) noexcept
      : base(ap, std::move(ar))
    {
    }

    TApply(object_ptr<App> ap, Arg* ar) noexcept
      : base(std::move(ap), ar)
    {
    }
  };

  /// apply operator
  template <
    class T1,
    class T2,
    class =
      std::enable_if_t<is_object_pointer_v<T1> && is_object_pointer_v<T2>>>
  [[nodiscard]] auto operator<<(T1&& lhs, T2&& rhs)
  {
    // use {} to workaround gcc bug (81486?)
    return object_ptr(
      new TApply {std::forward<T1>(lhs), std::forward<T2>(rhs)});
  }

} // namespace yave