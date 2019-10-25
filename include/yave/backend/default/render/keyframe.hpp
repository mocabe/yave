//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/obj/keyframe.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/keyframe/keyframe.hpp>
#include <yave/obj/frame/frame.hpp>

namespace yave {

  namespace backend::default_render {

    struct KeyframeIntEvaluator
      : NodeFunction<KeyframeIntEvaluator, KeyframeInt, Frame, Int>
    {
      return_type code() const
      {
        auto kf = eval_arg<0>();
        auto f  = eval_arg<1>();
        auto v  = kf->find_value(f->time_point);
        // FIXME: return 64bit integer
        return make_object<Int>(static_cast<int>(v));
      }
    };

    struct KeyframeBoolEvaluator
      : NodeFunction<KeyframeBoolEvaluator, KeyframeBool, Frame, Bool>
    {
      return_type code() const
      {
        auto kf = eval_arg<0>();
        auto f  = eval_arg<1>();
        auto v  = kf->find_value(f->time_point);
        return make_object<Bool>(v);
      }
    };

    struct KeyframeFloatEvaluator
      : NodeFunction<KeyframeFloatEvaluator, KeyframeFloat, Frame, Float>
    {
      return_type code() const
      {
        auto kf = eval_arg<0>();
        auto f  = eval_arg<1>();
        auto v  = kf->find_value(f->time_point);
        // TODO: Interpolation using control points
        return make_object<Float>(static_cast<float>(v.value));
      }
    };
  } // namespace backend::default_render

#define YAVE_DECL_KEYFRAME_BIND_INFO_DEFAULT_RENDER(TYPE)            \
  template <>                                                        \
  struct bind_info_traits<node::TYPE, backend::tags::default_render> \
  {                                                                  \
    static bind_info get_bind_info()                                 \
    {                                                                \
      auto info = get_node_info<node::TYPE>();                       \
      auto name = info.name();                                       \
      auto is   = info.input_sockets();                              \
      auto os   = info.output_sockets();                             \
                                                                     \
      return bind_info(                                              \
        name,                                                        \
        {is[0], is[1]},                                              \
        os[0],                                                       \
        make_object<backend::default_render::TYPE>(),                \
        "Calculate value of keyframe at current frame.",             \
        true);                                                       \
    }                                                                \
  }

  YAVE_DECL_KEYFRAME_BIND_INFO_DEFAULT_RENDER(KeyframeIntEvaluator);
  YAVE_DECL_KEYFRAME_BIND_INFO_DEFAULT_RENDER(KeyframeFloatEvaluator);
  YAVE_DECL_KEYFRAME_BIND_INFO_DEFAULT_RENDER(KeyframeBoolEvaluator);

} // namespace yave