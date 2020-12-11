//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/signal/specifier.hpp>
#include <yave/rts/function.hpp>
#include <yave/rts/eval.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/frame_time/frame_time.hpp>

namespace yave {

  namespace detail {

    template <class...>
    struct SignalFunctionImpl;

    // Lifted signal function
    template <class Derived, class R, class... Ts>
    struct SignalFunctionImpl<Derived, meta_tuple<Ts...>, meta_type<R>>
      : Function<Derived, closure<FrameDemand, Ts>..., FrameDemand, R>
    {
    private:
      using base =
        Function<Derived, closure<FrameDemand, Ts>..., FrameDemand, R>;

    public:
      /// Get argument demand
      [[nodiscard]] auto arg_demand() const noexcept
      {
        return base::template eval_arg<sizeof...(Ts)>();
      }

      /// Get argument time
      [[nodiscard]] auto arg_time() const noexcept
      {
        return arg_demand()->time;
      }

      /// Get argument signal value
      template <uint64_t N>
      [[nodiscard]] auto arg_signal() const noexcept
      {
        static_assert(N < sizeof...(Ts), "Invalid argument index");
        return base::template arg<N>();
      }

      /// Get value of input as thunk
      template <uint64_t N>
      [[nodiscard]] auto arg() const noexcept
      {
        return arg_signal<N>() << arg_demand();
      }

      /// Get value of input by forced evaluation
      template <uint64_t N>
      [[nodiscard]] auto eval_arg() const noexcept
      {
        return eval(this->template arg<N>());
      }
    };
  } // namespace detail

  /// Signal function object
  /// \param Derived for CRTP.
  /// \param Ts List of argument types and return type.
  template <class Derived, class... Ts>
  struct SignalFunction : detail::SignalFunctionImpl<
                            Derived,
                            decltype(remove_last(tuple_c<Ts...>)),
                            decltype(last(tuple_c<Ts...>))>
  {
  };

} // namespace yave