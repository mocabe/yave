//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/obj/keyframe.hpp>
#include <yave/node/obj/function.hpp>

#include <yave/obj/keyframe/keyframe.hpp>
#include <yave/obj/frame/frame.hpp>

#include <yave/backend/default/system/config.hpp>

namespace yave {

  namespace backend::default_render {

    struct KeyframeIntValueExtractor
      : NodeFunction<KeyframeIntValueExtractor, KeyframeInt, Frame, Int>
    {
      return_type code() const
      {
        auto kf = eval_arg<0>();
        auto f  = eval_arg<1>();
        auto v  = kf->find_value(f->time);
        return make_object<Int>(v);
      }
    };

    struct KeyframeBoolValueExtractor
      : NodeFunction<KeyframeBoolValueExtractor, KeyframeBool, Frame, Bool>
    {
      return_type code() const
      {
        auto kf = eval_arg<0>();
        auto f  = eval_arg<1>();
        auto v  = kf->find_value(f->time);
        return make_object<Bool>(v);
      }
    };

    struct KeyframeFloatValueExtractor
      : NodeFunction<KeyframeFloatValueExtractor, KeyframeFloat, Frame, Float>
    {
      return_type code() const
      {
        auto kf = eval_arg<0>();
        auto f  = eval_arg<1>();
        auto v  = kf->find_value(f->time);
        // TODO: Interpolation using control points
        return make_object<Float>(v);
      }
    };
  } // namespace backend::default_render

#define YAVE_DECL_KEYFRAME_BIND_INFO(TYPE)                     \
  template <>                                                  \
  struct bind_info_traits<TYPE, backend::tags::default_render> \
  {                                                            \
    static bind_info get_bind_info()                           \
    {                                                          \
      auto info = get_node_info<TYPE>();                       \
      auto name = info.name();                                 \
      auto is   = info.input_sockets();                        \
      auto os   = info.output_sockets();                       \
                                                               \
      return bind_info(                                        \
        name,                                                  \
        {is[0], is[1]},                                        \
        os[0],                                                 \
        make_object<backend::default_render::TYPE>(),          \
        "Calculate value of keyframe at current frame.",       \
        true);                                                 \
    }                                                          \
  }

  YAVE_DECL_KEYFRAME_BIND_INFO(KeyframeIntValueExtractor);
  YAVE_DECL_KEYFRAME_BIND_INFO(KeyframeFloatValueExtractor);
  YAVE_DECL_KEYFRAME_BIND_INFO(KeyframeBoolValueExtractor);

} // namespace yave