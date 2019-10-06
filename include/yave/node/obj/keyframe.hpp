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

    struct KeyframeIntValueExtractor;
    struct KeyframeFloatValueExtractor;
    struct KeyframeBoolValueExtractor;

    struct KeyframeInt;
    struct KeyframeFloat;
    struct KeyframeBool;

  } // namespace node

#define YAVE_DECL_KEYFRAME_EXTRACTOR(TYPE)                 \
  template <>                                              \
  struct node_info_traits<node::TYPE>                      \
  {                                                        \
    static node_info get_node_info()                       \
    {                                                      \
      return node_info(#TYPE, {"kf", "frame"}, {"value"}); \
    }                                                      \
  }

  YAVE_DECL_KEYFRAME_EXTRACTOR(KeyframeIntValueExtractor);
  YAVE_DECL_KEYFRAME_EXTRACTOR(KeyframeFloatValueExtractor);
  YAVE_DECL_KEYFRAME_EXTRACTOR(KeyframeBoolValueExtractor);

  /* Keyframe syntax sugars */

#define YAVE_DECL_KEYFRAME_SUGAR(TYPE)                              \
  template <>                                                       \
  struct node_info_traits<node::TYPE>                               \
  {                                                                 \
    static node_info get_node_info()                                \
    {                                                               \
      return node_info(                                             \
        #TYPE "Sugar", {"frame"}, {"value"}, node_type::primitive); \
    }                                                               \
  }

  YAVE_DECL_KEYFRAME_SUGAR(KeyframeInt);
  YAVE_DECL_KEYFRAME_SUGAR(KeyframeFloat);
  YAVE_DECL_KEYFRAME_SUGAR(KeyframeBool);

  inline std::vector<node_info> get_keyframe_node_info_list()
  {
    return {/* extractors */
            get_node_info<node::KeyframeIntValueExtractor>(),
            get_node_info<node::KeyframeFloatValueExtractor>(),
            get_node_info<node::KeyframeBoolValueExtractor>(),
            /* sugars */
            get_node_info<node::KeyframeInt>(),
            get_node_info<node::KeyframeBool>(),
            get_node_info<node::KeyframeFloat>()};
  }
} // namespace yave