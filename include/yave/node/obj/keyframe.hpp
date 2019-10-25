//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/rts.hpp>
#include <yave/node/core/get_info.hpp>
#include <yave/obj/keyframe/keyframe.hpp>

namespace yave {

  /* Keyframe value extractors */

  namespace node {

    struct KeyframeIntEvaluator;
    struct KeyframeFloatEvaluator;
    struct KeyframeBoolEvaluator;

    struct KeyframeInt;
    struct KeyframeFloat;
    struct KeyframeBool;

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

  YAVE_DECL_KEYFRAME_EVALUATOR(KeyframeIntEvaluator);
  YAVE_DECL_KEYFRAME_EVALUATOR(KeyframeFloatEvaluator);
  YAVE_DECL_KEYFRAME_EVALUATOR(KeyframeBoolEvaluator);

  inline std::vector<node_info> get_keyframe_node_info_list()
  {
    return {/* extractors */
            get_node_info<node::KeyframeIntEvaluator>(),
            get_node_info<node::KeyframeFloatEvaluator>(),
            get_node_info<node::KeyframeBoolEvaluator>(),
            /* sugars */
            get_node_info<node::KeyframeInt>(),
            get_node_info<node::KeyframeBool>(),
            get_node_info<node::KeyframeFloat>()};
  }

} // namespace yave