//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/specifiers.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>

namespace yave {

  namespace detail {

    template <class...>
    struct signal_impl;

    template <class R, class... Ts>
    struct signal_impl<meta_tuple<Ts...>, meta_type<R>>
    {
      using type = closure<closure<FrameDemand, Ts>..., FrameDemand, R>;
    };
  } // namespace detail

  /// signal closure specifier
  template <class... Ts>
  using signal = typename detail::signal_impl<
    decltype(remove_last(tuple_c<Ts...>)),
    decltype(last(tuple_c<Ts...>))>::type;

} // namespace yave