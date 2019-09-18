//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/rts.hpp>
#include <yave/obj/frame/frame.hpp>
#include <yave/node/core/get_info.hpp>
#include <yave/node/core/instance_getter.hpp>

namespace yave {

  namespace detail {

    template <class...>
    struct NodeFunctionImpl;

    template <class Derived, class R, class... Ts>
    struct NodeFunctionImpl<Derived, meta_tuple<Ts...>, meta_type<R>>
      : Function<Derived, closure<Frame, Ts>..., Frame, R>
    {
    private:
      using base = Function<Derived, closure<Frame, Ts>..., Frame, R>;

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
    };

  } // namespace detail

  /// Wrapper template of Function for node functions.
  /// \param Derived for CRTP.
  /// \param Ts List of argument types and return type.
  template <class Derived, class... Ts>
  struct NodeFunction : detail::NodeFunctionImpl<
                          Derived,
                          decltype(remove_last(meta_tuple<Ts...> {})),
                          decltype(last(meta_tuple<Ts...> {}))>
  {
  };

  namespace detail {

    template <class...>
    struct node_closure_impl;

    template <class R, class... Ts>
    struct node_closure_impl<meta_tuple<Ts...>, meta_type<R>>
    {
      using type = closure<closure<Frame, Ts>..., Frame, R>;
    };

  } // namespace detail

  /// create closure specifier for node function
  template <class... Ts>
  using node_closure = typename detail::node_closure_impl<
    decltype(remove_last(meta_tuple<Ts...> {})),
    decltype(last(meta_tuple<Ts...> {}))>::type;

} // namespace yave