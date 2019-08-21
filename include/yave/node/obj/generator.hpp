//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/eval.hpp>
#include <yave/obj/frame/frame.hpp>
#include <yave/node/obj/function.hpp>

#include <functional>

namespace yave {

  /// UnaryFunction
  template <class T1, class TR, class E>
  struct UnaryFunction : NodeFunction<UnaryFunction<T1, TR, E>, T1, TR>
  {
    typename UnaryFunction::return_type code() const
    {
      auto v0 = this->template eval_arg<0>();
      E e;
      return make_object<TR>(e(*v0));
    }
  };

  /// BinaryFunction
  template <class T1, class T2, class TR, class E>
  struct BinaryFunction
    : NodeFunction<BinaryFunction<T1, T2, TR, E>, T1, T2, TR>
  {
    typename BinaryFunction::return_type code() const
    {
      auto v0 = this->template eval_arg<0>();
      auto v1 = this->template eval_arg<1>();
      E e;
      return make_object<TR>(e(*v0, *v1));
    }
  };

  /// TernaryFunction
  template <class T1, class T2, class T3, class TR, class E>
  struct TernaryFunction
    : NodeFunction<TernaryFunction<T1, T2, T3, TR, E>, T1, T2, T3, TR>
  {
    typename TernaryFunction::return_type code() const
    {
      auto v0 = this->template eval_arg<0>();
      auto v1 = this->template eval_arg<1>();
      auto v2 = this->template eval_arg<2>();
      E e;
      return make_object<TR>(e(*v0, *v1, *v2));
    }
  };

  /// QuaternaryFunction
  template <class T1, class T2, class T3, class T4, class TR, class E>
  struct QuaternaryFunction //
    : NodeFunction<
        QuaternaryFunction<T1, T2, T3, T4, TR, E>,
        T1,
        T2,
        T3,
        T4,
        TR>
  {
    typename QuaternaryFunction::return_type code() const
    {
      auto v0 = this->template eval_arg<0>();
      auto v1 = this->template eval_arg<1>();
      auto v2 = this->template eval_arg<2>();
      auto v3 = this->template eval_arg<3>();
      E e;
      return make_object<TR>(e(*v0, *v1, *v2, *v3));
    }
  };

} // namespace yave