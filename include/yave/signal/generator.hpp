//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/signal/function.hpp>

#include <functional>

namespace yave {

  /// UnarySignalFunction
  template <class T1, class TR, class E>
  struct UnarySignalFunction
    : SignalFunction<UnarySignalFunction<T1, TR, E>, T1, TR>
  {
    typename UnarySignalFunction::return_type code() const
    {
      auto v0 = this->template eval_arg<0>();
      E e;
      return make_object<TR>(e(*v0));
    }
  };

  /// BinarySignalFunction
  template <class T1, class T2, class TR, class E>
  struct BinarySignalFunction
    : SignalFunction<BinarySignalFunction<T1, T2, TR, E>, T1, T2, TR>
  {
    typename BinarySignalFunction::return_type code() const
    {
      auto v0 = this->template eval_arg<0>();
      auto v1 = this->template eval_arg<1>();
      E e;
      return make_object<TR>(e(*v0, *v1));
    }
  };

  /// TernarySignalFunction
  template <class T1, class T2, class T3, class TR, class E>
  struct TernarySignalFunction
    : SignalFunction<TernarySignalFunction<T1, T2, T3, TR, E>, T1, T2, T3, TR>
  {
    typename TernarySignalFunction::return_type code() const
    {
      auto v0 = this->template eval_arg<0>();
      auto v1 = this->template eval_arg<1>();
      auto v2 = this->template eval_arg<2>();
      E e;
      return make_object<TR>(e(*v0, *v1, *v2));
    }
  };

  /// QuaternarySignalFunction
  template <class T1, class T2, class T3, class T4, class TR, class E>
  struct QuaternarySignalFunction //
    : SignalFunction<
        QuaternarySignalFunction<T1, T2, T3, T4, TR, E>,
        T1,
        T2,
        T3,
        T4,
        TR>
  {
    typename QuaternarySignalFunction::return_type code() const
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