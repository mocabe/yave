//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/memory.hpp>

#include <memory>
#include <mutex>
#include <concepts>

#include <boost/signals2.hpp>

namespace yave::ui {

  template <class...>
  class signal;
  template <class...>
  class slot;
  class scoped_connection;

  class connection
  {
    boost::signals2::connection m_c;

    template <class...>
    friend class signal;
    friend class scoped_connection;

    connection(boost::signals2::connection conn)
      : m_c {conn}
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
      return m_c.disconnect();
    }

    bool connected() const
    {
      return m_c.connected();
    }

    void swap(connection& other) noexcept
    {
      return m_c.swap(other.m_c);
    }

    auto operator<=>(const connection&) const -> std::strong_ordering = default;
  };

  class scoped_connection
  {
    boost::signals2::scoped_connection m_c;

  public:
    scoped_connection()                             = default;
    scoped_connection(const scoped_connection&)     = delete;
    scoped_connection(scoped_connection&&) noexcept = default;
    scoped_connection& operator=(const scoped_connection&) = delete;
    scoped_connection& operator=(scoped_connection&&) noexcept = default;

    scoped_connection(const connection& c)
      : m_c {c.m_c}
    {
    }

    scoped_connection& operator=(const connection& c)
    {
      m_c = c.m_c;
      return *this;
    }

    auto release() -> connection
    {
      return connection(m_c.release());
    }

    void swap(scoped_connection& other) noexcept
    {
      return m_c.swap(other.m_c);
    }
  };

  /// Slot type
  template <class... ArgTypes>
  class slot
  {
    boost::signals2::slot<void(ArgTypes...)> m_slot;

    template <class...>
    friend class signal;

    template <class T>
    void track1(const std::shared_ptr<T>& sp)
    {
      m_slot.track_foreign(sp);
    }

    template <class T>
    void track1(const std::weak_ptr<T>& wp)
    {
      m_slot.track_foreign(wp);
    }

    void track1(const trackable& t)
    {
      track(t.get_tracker().get_weak());
    }

  public:
    using result_type    = void;
    using signature_type = void(ArgTypes...);

    slot()                = default;
    slot(const slot&)     = default;
    slot(slot&&) noexcept = default;
    slot& operator=(const slot&) = default;
    slot& operator=(slot&&) noexcept = default;

    template <class F>
    requires (!std::same_as<std::decay_t<F>, slot>) slot(F&& f)
      : m_slot {std::forward<F>(f)}
    {
    }

    template <class... Args>
    void operator()(Args&&... args)
    {
      return m_slot(std::forward<Args>(args)...);
    }

    template <class... Args>
    auto track(Args&&... args) -> slot&
    {
      [](int...) {}((track1(std::forward<Args>(args)), 0)...);
      return *this;
    }
  };

  /// Signal type
  template <class... ArgTypes>
  class signal
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

    auto connect_scoped(const slot_type& slot) -> scoped_connection
    {
      return scoped_connection(connect(slot));
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