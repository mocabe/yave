//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>

namespace yave {

  // ------------------------------------------
  // helper

  [[nodiscard]] inline object_ptr<const Object>
    add_cache_tag(object_ptr<const Object> obj)
  {
    _get_storage(obj).set_pointer_tag(object_ptr_storage::pointer_tags::cache);
    return obj;
  }

  [[nodiscard]] inline bool has_cache_tag(const object_ptr<const Object>& obj)
  {
    return _get_storage(obj).is_cache();
  }

  // ------------------------------------------
  // apply object storage

  struct apply_object_value_storage
  {
    apply_object_value_storage(
      object_ptr<const Object> app,
      object_ptr<const Object> arg)
      : m_app {std::move(app)}
      , m_arg {std::move(arg)}
    {
    }

    const auto& app() const
    {
      assert(!evaluated());
      return m_app;
    }

    const auto& arg() const
    {
      assert(!evaluated());
      return m_arg;
    }

    bool evaluated() const
    {
      return has_cache_tag(m_arg);
    }

    /// get cache of object
    auto get_cache() const
    {
      assert(evaluated());
      return clear_pointer_tag(m_arg);
    }

    /// set cache of object
    void set_cache(const object_ptr<const Object>& obj) const
    {
      assert(!evaluated());
      m_app = nullptr;
      m_arg = add_cache_tag(obj);
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
    apply_object_value(App&& app, Arg&& arg)
      : m_storage {std::forward<App>(app), std::forward<Arg>(arg)}
    {
    }

    [[nodiscard]] friend inline const apply_object_value_storage&
      _get_storage(const apply_object_value& v)
    {
      return v.m_storage;
    }

    [[nodiscard]] friend inline apply_object_value_storage&
      _get_storage(apply_object_value& v)
    {
      return v.m_storage;
    }

  private:
    apply_object_value_storage m_storage;
  };

  /// runtime apply object
  using Apply = Box<apply_object_value>;

  /// compile time apply object
  template <class App, class Arg>
  struct TApply : Apply
  {
    /// base
    using base = Apply;
    /// term
    static constexpr auto term =
      make_tm_apply(get_term<App>(), get_term<Arg>());

    // clang-format off

      TApply(object_ptr<App> ap, object_ptr<Arg> ar) 
        : base(std::move(ap), std::move(ar)) {}

      TApply(App* ap, Arg* ar) 
        : base(object_ptr<const Object>(ap), object_ptr<const Object>(ar)) {}

      TApply(App* ap, object_ptr<Arg> ar) 
        : base(ap, std::move(ar)) {}

      TApply(object_ptr<App> ap, Arg* ar) 
        : base(std::move(ap), ar) {}

    // clang-format on
  };

  namespace detail {

    template <class T>
    constexpr auto is_valid_app_arg(meta_type<T>)
    {
      return false_c;
    }

    template <class T>
    constexpr auto is_valid_app_arg(meta_type<T*>)
    {
      if constexpr (std::is_base_of_v<Object, T>)
        return true_c;
      else
        return false_c;
    }

    template <class T>
    constexpr auto is_valid_app_arg(meta_type<object_ptr<T>>)
    {
      return true_c;
    }

  } // namespace detail

  /// apply operator
  template <
    class T1,
    class T2,
    class = std::enable_if_t<
      detail::is_valid_app_arg(type_c<std::decay_t<T1>>) &&
      detail::is_valid_app_arg(type_c<std::decay_t<T2>>)>>
  [[nodiscard]] auto operator<<(T1&& lhs, T2&& rhs)
  {
    // use {} to workaround gcc bug (81486?)
    return object_ptr(
      new TApply {std::forward<T1>(lhs), std::forward<T2>(rhs)});
  }

} // namespace yave

// Apply
YAVE_DECL_TYPE(yave::Apply, "2db5ddcd-0d6d-4f2f-8fd5-7b30abfc68eb");