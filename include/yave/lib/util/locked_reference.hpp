//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>

#include <mutex>

namespace yave {

  /// locked reference wrapper
  template <class T, class M>
  class locked_reference
  {
    T& m_ref;
    std::unique_lock<M> m_lck;

  public:
    locked_reference(T& ref, std::unique_lock<M> lck)
      : m_ref {ref}
      , m_lck {std::move(lck)}
    {
    }

    locked_reference(locked_reference&&) noexcept = default;

  public:
    auto& ref() const&
    {
      return m_ref;
    }

    auto& cref() const&
    {
      return std::as_const(m_ref);
    }

    auto& ref() const&&  = delete;
    auto& cref() const&& = delete;
  };

  /// locked reference wrapper
  template <class T, class M>
  class shared_locked_reference
  {
    std::shared_ptr<T> m_ptr;
    std::unique_lock<M> m_lck;

  public:
    shared_locked_reference(std::shared_ptr<T> ptr, std::unique_lock<M> lck)
      : m_ptr {std::move(ptr)}
      , m_lck {std::move(lck)}
    {
      assert(m_ptr);
    }

    shared_locked_reference(shared_locked_reference&&) noexcept = default;

  public:
    auto& ref() const&
    {
      return *m_ptr;
    }

    auto& cref() const&
    {
      return std::as_const(*m_ptr);
    }

    auto& ref() const&&  = delete;
    auto& cref() const&& = delete;
  };

} // namespace yave