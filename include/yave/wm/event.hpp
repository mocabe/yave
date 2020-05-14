//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <memory>
#include <typeinfo>

namespace yave::wm {

  /// base class of window events.
  class event
  {
  public:
    /// dtor
    virtual ~event() noexcept = default;

  public:
    /// accepted?
    [[nodiscard]] bool accepted() const
    {
      return m_accepted;
    }

    /// accept event
    void accept()
    {
      m_accepted = true;
    }

  private:
    bool m_accepted = false;
  };

  /// Allocate new event
  template <class E, class... Args>
  [[nodiscard]] auto make_event(Args&&... args)
  {
    return std::make_unique<E>(std::forward<Args>(args)...);
  }

  /// Cast events
  template <class E>
  [[nodiscard]] auto event_cast_if(event* e) -> E*
  {
    if (typeid(E) == typeid(*e))
      return static_cast<E*>(e);

    return nullptr;
  }

} // namespace yave::wm