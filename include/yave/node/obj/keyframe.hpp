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

  struct KeyframeIntValueExtractor;
  struct KeyframeFloatValueExtractor;
  struct KeyframeBoolValueExtractor;

#define YAVE_DECL_KEYFRAME_EXTRACTOR(TYPE)                 \
  template <>                                              \
  struct node_info_traits<TYPE>                            \
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

#define YAVE_DECL_KEYFRAME_SUGAR(TYPE)                                  \
  template <>                                                           \
  struct node_info_traits<TYPE>                                         \
  {                                                                     \
    static node_info get_node_info()                                    \
    {                                                                   \
      return node_info(#TYPE, {"frame"}, {"value"}, /*.is_prim=*/true); \
    }                                                                   \
  }

  YAVE_DECL_KEYFRAME_SUGAR(KeyframeInt);
  YAVE_DECL_KEYFRAME_SUGAR(KeyframeFloat);
  YAVE_DECL_KEYFRAME_SUGAR(KeyframeBool);

  std::vector<node_info> get_keyframe_node_info_list()
  {
    return {/* extractors */
            get_node_info<KeyframeIntValueExtractor>(),
            get_node_info<KeyframeFloatValueExtractor>(),
            get_node_info<KeyframeBoolValueExtractor>(),
            /* sugars */
            get_node_info<KeyframeInt>(),
            get_node_info<KeyframeBool>(),
            get_node_info<KeyframeFloat>()};
  }
} // namespace yave