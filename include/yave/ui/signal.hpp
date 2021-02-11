//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/memory.hpp>

#include <boost/signals2.hpp>

namespace boost::signals2 {

  template <class T>
  struct weak_ptr_traits<yave::ui::weak<T>>
  {
    using shared_type = yave::ui::shared<T>;
  };

  template <class T>
  struct shared_ptr_traits<yave::ui::shared<T>>
  {
    using weak_type = yave::ui::weak<T>;
  };

} // namespace boost::signals2

namespace yave::ui {

  template <class>
  class signal;
  template <class>
  class slot;

  class connection
  {
    boost::signals2::connection m_conn;

    template <class T>
    friend class signal;

    connection(boost::signals2::connection conn)
      : m_conn {conn}
    {
    }

  public:
    connection()                      = default;
    connection(const connection&)     = default;
    connection(connection&&) noexcept = default;
    connection& operator=(const connection&) = default;
    connection& operator=(connection&&) noexcept = default;

    void disconnect()
    {
      return m_conn.disconnect();
    }

    bool connected() const
    {
      return m_conn.connected();
    }

    void swap(connection& other) noexcept
    {
      return m_conn.swap(other.m_conn);
    }

    auto operator<=>(const connection&) const = default;
  };

  template <class SharedPtr>
  concept slot_trackable_shared_ptr = requires
  {
    typename boost::signals2::shared_ptr_traits<SharedPtr>::weak_type;
  };

  template <class WeakPtr>
  concept slot_trackable_weak_ptr = requires
  {
    typename boost::signals2::weak_ptr_traits<WeakPtr>::shared_type;
  };

  /// Slot type
  template <class... ArgTypes>
  class slot<ArgTypes...>
  {
    boost::signals2::slot<void(ArgTypes...)> m_slot;

    template <class T>
    friend class signal;

  public:
    using result_type    = void;
    using signature_type = void(ArgTypes...);

    template <class... Args>
    void operator()(Args&&... args)
    {
      return m_slot(std::forward<Args>(args)...);
    }

    template <slot_trackable_shared_ptr SharedPtr>
    auto track(const SharedPtr& sp) -> slot&
    {
      return m_slot.track_foreign(sp);
    }

    template <slot_trackable_weak_ptr WeakPtr>
    auto track(const WeakPtr& wp) -> slot&
    {
      return m_slot.track_foreign(wp);
    }
  };

  /// Signal type
  template <class... ArgTypes>
  class signal<ArgTypes...>
  {
    boost::signals2::signal<void(ArgTypes...)> m_signal;

  public:
    using slot_type      = slot<ArgTypes...>;
    using result_type    = void;
    using signature_type = void(ArgTypes...);

    signal()                  = default;
    signal(const signal&)     = delete;
    signal(signal&&) noexcept = default;
    signal& operator=(const signal&) = delete;
    signal& operator=(signal&&) noexcept = default;

    auto connect(const slot_type& slot) -> connection
    {
      return connection(m_signal.connect(slot.m_slot));
    }

    void disconnect(const slot_type& slot)
    {
      return m_signal.disconnect(slot.m_slot);
    }

    void disconnect_all()
    {
      return m_signal.disconnect_all_slots();
    }

    bool empty() const
    {
      return m_signal.empty();
    }

    auto num_slots() const -> size_t
    {
      return m_signal.num_slots();
    }

    template <class... Args>
    void operator()(Args&&... args)
    {
      return m_signal(std::forward<Args>(args)...);
    }
  };
} // namespace yave::ui