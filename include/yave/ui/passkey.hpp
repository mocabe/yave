//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

namespace yave::ui {

  /// "PassKey" idiom helper.
  template <class T>
  class passkey
  {
    friend T;
    passkey()               = default;
    ~passkey()              = default;
    passkey(const passkey&) = default;
    passkey& operator=(const passkey&) = delete;
    passkey& operator=(passkey&&) = delete;

  public:
    // movable so it can be passed into forwarding functions.
    passkey(passkey&&) = default;
  };
} // namespace yave::ui