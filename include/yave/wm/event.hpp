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
    // internal state
    int m_status = 0;

  public:
    /// ctor
    event() noexcept;
    /// dtor
    virtual ~event() noexcept;

  public:
    /// accepted?
    [[nodiscard]] bool accepted() const;
    /// ignored?
    [[nodiscard]] bool ignored() const;

    /// accept event
    void accept();
    /// ignore event
    void ignore();
  };

  /// Cast events
  /// \param e non-null pointer to event object
  template <class E>
  [[nodiscard]] auto event_cast_if(event* e) -> E*
  {
    if (event& ref = *e; typeid(ref) == typeid(E))
      return static_cast<E*>(e);

    return nullptr;
  }

} // namespace yave::wm