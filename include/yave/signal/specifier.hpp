//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/specifiers.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>

namespace yave {

  /// signal value
  template <class T>
  using signal = closure<FrameDemand, T>;

  /// signal function
  template <class... Ts>
  using sf = closure<signal<Ts>...>;

  // ------------------------------------------
  // is_signal_v

  template <class T>
  constexpr bool is_signal_v = false;

  template <class T>
  constexpr bool is_signal_v<signal<T>> = true;

  // ------------------------------------------
  // is_sf_v

  template <class T>
  constexpr bool is_sf_v = false;

  template <class... Ts>
  constexpr bool is_sf_v<sf<Ts...>> = true;

} // namespace yave