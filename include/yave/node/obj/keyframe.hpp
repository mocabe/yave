//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/rts.hpp>
#include <yave/node/core/get_info.hpp>
#include <yave/obj/primitive/primitive.hpp>

namespace yave {

  /// Keyframe
  template <class T>
  struct Keyframe;

  template <class T>
  struct node_info_traits<Keyframe<T>>;

#define YAVE_DECL_KEYFRAME(TYPE)                                         \
  template <>                                                            \
  struct node_info_traits<Keyframe<TYPE>>                                \
  {                                                                      \
    static node_info get_node_info()                                     \
    {                                                                    \
      return {std::string("Keyframe<" #TYPE ">"), {"frame"}, {"value"}}; \
    }                                                                    \
  }

  YAVE_DECL_KEYFRAME(Int8);
  YAVE_DECL_KEYFRAME(Int16);
  YAVE_DECL_KEYFRAME(Int32);
  YAVE_DECL_KEYFRAME(Int64);

  YAVE_DECL_KEYFRAME(UInt8);
  YAVE_DECL_KEYFRAME(UInt16);
  YAVE_DECL_KEYFRAME(UInt32);
  YAVE_DECL_KEYFRAME(UInt64);

  YAVE_DECL_KEYFRAME(Float);
  YAVE_DECL_KEYFRAME(Double);

  YAVE_DECL_KEYFRAME(Bool);

  std::vector<node_info> get_keyframe_node_info_list()
  {
    return {get_node_info<Keyframe<Int8>>(),
            get_node_info<Keyframe<Int16>>(),
            get_node_info<Keyframe<Int32>>(),
            get_node_info<Keyframe<Int64>>(),
            get_node_info<Keyframe<UInt8>>(),
            get_node_info<Keyframe<UInt16>>(),
            get_node_info<Keyframe<UInt32>>(),
            get_node_info<Keyframe<UInt64>>(),
            get_node_info<Keyframe<Float>>(),
            get_node_info<Keyframe<Double>>(),
            get_node_info<Keyframe<Bool>>()};
  }

} // namespace yave