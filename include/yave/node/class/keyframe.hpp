//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/rts.hpp>
#include <yave/node/core/get_info.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/keyframe/keyframe.hpp>
#include <yave/obj/frame_time/frame_time.hpp>

namespace yave {

  namespace node {

    struct KeyframeEvaluatorInt;
    struct KeyframeEvaluatorFloat;
    struct KeyframeEvaluatorBool;

  } // namespace node

#define YAVE_DECL_KEYFRAME_EVALUATOR(TYPE)                                 \
  template <>                                                              \
  struct node_declaration_traits<node::KeyframeEvaluator##TYPE>            \
  {                                                                        \
    static auto get_node_declaration() -> node_declaration                 \
    {                                                                      \
      return node_declaration(                                             \
        "KeyframeEvaluator" #TYPE,                                         \
        {"keyframes", "frame"},                                            \
        {"value"},                                                         \
        node_type::normal,                                                 \
        object_type<node_closure<KeyframeData##TYPE, FrameTime, TYPE>>()); \
    }                                                                      \
  }

  YAVE_DECL_KEYFRAME_EVALUATOR(Int);
  YAVE_DECL_KEYFRAME_EVALUATOR(Float);
  YAVE_DECL_KEYFRAME_EVALUATOR(Bool);

  inline auto get_keyframe_node_info_list() -> std::vector<node_declaration>
  {
    return {/* evaluators */
            get_node_declaration<node::KeyframeEvaluatorInt>(),
            get_node_declaration<node::KeyframeEvaluatorFloat>(),
            get_node_declaration<node::KeyframeEvaluatorBool>()};
  }

} // namespace yave