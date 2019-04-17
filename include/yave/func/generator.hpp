//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/function.hpp>
#include <yave/rts/eval.hpp>
#include <yave/obj/frame.hpp>
#include <functional>

namespace yave {

  /// UnaryFunction
  template <class T1, class TR, class E>
  struct UnaryFunction //
    : Function<
        UnaryFunction<T1, TR, E>,
        closure<Frame::Frame, T1>,
        Frame::Frame,
        TR>
  {
    typename UnaryFunction::return_type code() const
    {
      auto v0 = eval(this->template arg<0>() << this->template arg<1>());
      E e;
      return make_object<TR>(e(*v0));
    }
  };

  /// BinaryFunction
  template <class T1, class T2, class TR, class E>
  struct BinaryFunction //
    : Function<
        BinaryFunction<T1, T2, TR, E>,
        closure<Frame::Frame, T1>,
        closure<Frame::Frame, T2>,
        Frame::Frame,
        TR>
  {
    typename BinaryFunction::return_type code() const
    {
      auto v0 = eval(this->template arg<0>() << this->template arg<2>());
      auto v1 = eval(this->template arg<1>() << this->template arg<2>());
      E e;
      return make_object<TR>(e(*v0, *v1));
    }
  };

  /// TernaryFunction
  template <class T1, class T2, class T3, class TR, class E>
  struct TernaryFunction //
    : Function<
        TernaryFunction<T1, T2, T3, TR, E>,
        closure<Frame::Frame, T1>,
        closure<Frame::Frame, T2>,
        closure<Frame::Frame, T3>,
        Frame::Frame,
        TR>
  {
    typename TernaryFunction::return_type code() const
    {
      auto v0 = eval(this->template arg<0>() << this->template arg<3>());
      auto v1 = eval(this->template arg<1>() << this->template arg<3>());
      auto v2 = eval(this->template arg<2>() << this->template arg<3>());
      E e;
      return make_object<TR>(e(*v0, *v1, *v2));
    }
  };

  /// QuaternaryFunction
  template <class T1, class T2, class T3, class T4, class TR, class E>
  struct QuaternaryFunction //
    : Function<
        QuaternaryFunction<T1, T2, T3, T4, TR, E>,
        closure<Frame::Frame, T1>,
        closure<Frame::Frame, T2>,
        closure<Frame::Frame, T3>,
        closure<Frame::Frame, T4>,
        Frame::Frame,
        TR>
  {
    typename QuaternaryFunction::return_type code() const
    {
      auto v0 = eval(this->template arg<0>() << this->template arg<4>());
      auto v1 = eval(this->template arg<1>() << this->template arg<4>());
      auto v2 = eval(this->template arg<2>() << this->template arg<4>());
      auto v3 = eval(this->template arg<3>() << this->template arg<4>());
      E e;
      return make_object<TR>(e(*v0, *v1, *v2, *v3));
    }
  };

} // namespace yave