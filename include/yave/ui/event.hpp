//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <memory>
#include <typeinfo>

namespace yave::ui {

  class window;

  /// even phases
  enum event_phase
  {
    /// Capture phase:
    /// Event propagates parent to child.
    capture,
    /// Bubble phase:
    /// Event propagates child to parent.
    bubble,
  };

  /// base class of window events.
  class event
  {
    /// target window
    const window* m_target;
    /// event propagation phase
    event_phase m_phase;
    /// accept property
    bool m_accepted = false;

  public:
    /// ctor
    event(const window& target, event_phase phase);
    /// dtor
    virtual ~event() noexcept;

  public:
    /// target window
    auto target() const -> const window&;
    /// current phase
    auto phase() const -> event_phase;
    /// accepted?
    bool accepted() const;

  protected:
    /// set target
    void set_target(window& w);
    /// set phase
    void set_phase(event_phase p);
    /// set accept state
    void set_accepted(bool b);

  public:
    /// accept event
    void accept();
    /// ignore event
    void ignore();

  public:
    /// dynamic type check
    template <std::derived_from<event> T>
    bool is() const
    {
      if constexpr (std::is_final_v<T>)
        return typeid_cast<const T>(this);
      else
        return dynamic_cast<const T*>(this);
    }

    /// dynamic cast
    template <std::derived_from<event> T>
    auto as() const -> const T&
    {
      if constexpr (std::is_final_v<T>)
        return typeid_cast<const T>(*this);
      else
        return dynamic_cast<const T>(*this);
    }

    /// dynamic cast
    template <std::derived_from<event> T>
    auto as() -> T&
    {
      if constexpr (std::is_final_v<T>)
        return typeid_cast<T>(*this);
      else
        return dynamic_cast<T>(*this);
    }

    /// dynamic cast to get pointer
    template <std::derived_from<event> T>
    auto get_as() const -> const T*
    {
      if constexpr (std::is_final_v<T>)
        return typeid_cast<const T>(this);
      else
        return dynamic_cast<const T*>(this);
    }

    /// dynamic cast to get pointer
    template <std::derived_from<event> T>
    auto get_as() -> T*
    {
      if constexpr (std::is_final_v<T>)
        return typeid_cast<T>(this);
      else
        return dynamic_cast<T*>(this);
    }
  };

} // namespace yave::ui