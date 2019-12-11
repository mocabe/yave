//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/decl/keyframe.hpp>

namespace yave {

  namespace backends::default_render {

    struct KeyframeEvaluatorInt
      : NodeFunction<KeyframeEvaluatorInt, KeyframeDataInt, FrameTime, Int>
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

    struct KeyframeEvaluatorBool
      : NodeFunction<KeyframeEvaluatorBool, KeyframeDataBool, FrameTime, Bool>
    {
      return_type code() const
      {
        auto kf = eval_arg<0>();
        auto f  = eval_arg<1>();
        auto v  = kf->find_value(f->time_point);
        return make_object<Bool>(v);
      }
    };

    struct KeyframeEvaluatorFloat : NodeFunction<
                                      KeyframeEvaluatorFloat,
                                      KeyframeDataFloat,
                                      FrameTime,
                                      Float>
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
  } // namespace backends::default_render

#define YAVE_DEFAULT_RENDER_DEF_KEYFRAME_EVALUATOR(TYPE)                  \
  template <>                                                             \
  struct node_definition_traits<node::TYPE, backend_tags::default_render> \
  {                                                                       \
    static auto get_node_definitions() -> std::vector<node_definition>    \
    {                                                                     \
      auto info = get_node_declaration<node::TYPE>();                     \
      auto name = info.name();                                            \
      auto os   = info.output_sockets();                                  \
                                                                          \
      return {node_definition(                                            \
        name,                                                             \
        os[0],                                                            \
        make_object<backends::default_render::TYPE>(),                    \
        "Calculate value of keyframe at current frame.")};                \
    }                                                                     \
  }

  YAVE_DEFAULT_RENDER_DEF_KEYFRAME_EVALUATOR(KeyframeEvaluatorInt);
  YAVE_DEFAULT_RENDER_DEF_KEYFRAME_EVALUATOR(KeyframeEvaluatorFloat);
  YAVE_DEFAULT_RENDER_DEF_KEYFRAME_EVALUATOR(KeyframeEvaluatorBool);

} // namespace yave