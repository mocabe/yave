//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/rts.hpp>
#include <yave/node/core/get_info.hpp>
#include <yave/obj/keyframe/keyframe.hpp>

namespace yave {

  namespace node {

    struct KeyframeEvaluatorInt;
    struct KeyframeEvaluatorFloat;
    struct KeyframeEvaluatorBool;

  } // namespace node

#define YAVE_DECL_KEYFRAME_EVALUATOR(TYPE)                        \
  template <>                                                     \
  struct node_info_traits<node::TYPE>                             \
  {                                                               \
    static node_info get_node_info()                              \
    {                                                             \
      return node_info(#TYPE, {"keyframes", "frame"}, {"value"}); \
    }                                                             \
  }

  YAVE_DECL_KEYFRAME_EVALUATOR(KeyframeEvaluatorInt);
  YAVE_DECL_KEYFRAME_EVALUATOR(KeyframeEvaluatorFloat);
  YAVE_DECL_KEYFRAME_EVALUATOR(KeyframeEvaluatorBool);

  inline std::vector<node_info> get_keyframe_node_info_list()
  {
    return {/* evaluators */
            get_node_info<node::KeyframeEvaluatorInt>(),
            get_node_info<node::KeyframeEvaluatorFloat>(),
            get_node_info<node::KeyframeEvaluatorBool>()};
  }

} // namespace yave