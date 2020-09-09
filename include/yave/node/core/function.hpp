//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/rts.hpp>
#include <yave/node/core/get_info.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/frame_time/frame_time.hpp>

namespace yave {

  // ------------------------------------------
  // NodeFunction

  namespace detail {

    template <class...>
    struct NodeFunctionImpl;

    template <class Derived, class R, class... Ts>
    struct NodeFunctionImpl<Derived, meta_tuple<Ts...>, meta_type<R>>
      : Function<Derived, closure<FrameDemand, Ts>..., FrameDemand, R>
    {
    private:
      using base =
        Function<Derived, closure<FrameDemand, Ts>..., FrameDemand, R>;

    public:
      /// Overrided for NodeFunction.
      template <uint64_t N>
      [[nodiscard]] auto arg() const noexcept
      {
        static_assert(N < sizeof...(Ts), "NodeFunction: Index out of range");
        return base::template arg<N>() << base::template arg<sizeof...(Ts)>();
      }

      /// Overrided for NodeFunction.
      template <uint64_t N>
      [[nodiscard]] auto eval_arg() const
      {
        return eval(this->template arg<N>());
      }

      /// Get current time applied to this function
      [[nodiscard]] auto arg_time() const -> object_ptr<const FrameTime>
      {
        return base::template eval_arg<sizeof...(Ts)>()->time;
      }
    };

  } // namespace detail

  /// Wrapper template of Function for node functions.
  /// Given argument type `T1...Tn Tr`,
  /// define closure of type as
  /// `(FrameDemand->T1) ->...-> (FrameDemand->Tn) -> FrameDemand -> Tr`.
  /// It's equavalent of `lift` function in FRP world.
  /// \param Derived for CRTP.
  /// \param Ts List of argument types and return type.
  template <class Derived, class... Ts>
  struct NodeFunction : detail::NodeFunctionImpl<
                          Derived,
                          decltype(remove_last(meta_tuple<Ts...> {})),
                          decltype(last(meta_tuple<Ts...> {}))>
  {
  };

  // ------------------------------------------
  // node_closure

  namespace detail {

    template <class...>
    struct node_closure_impl;

    template <class R, class... Ts>
    struct node_closure_impl<meta_tuple<Ts...>, meta_type<R>>
    {
      using type = closure<closure<FrameDemand, Ts>..., FrameDemand, R>;
    };

  } // namespace detail

  /// create closure specifier for node function
  template <class... Ts>
  using node_closure = typename detail::node_closure_impl<
    decltype(remove_last(meta_tuple<Ts...> {})),
    decltype(last(meta_tuple<Ts...> {}))>::type;

} // namespace yave