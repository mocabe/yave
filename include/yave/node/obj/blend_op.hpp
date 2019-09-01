//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/image/blend_operation.hpp>
#include <yave/node/core/get_info.hpp>

namespace yave {

  namespace node {

    /* BlendOp* functions */
    struct BlendOpSrc;
    struct BlendOpDst;
    struct BlendOpOver;
    struct BlendOpIn;
    struct BlendOpOut;
    struct BlendOpAdd;

    /* BlendOp* getters */
    struct BlendOpSrcGetter;
    struct BlendOpDstGetter;
    struct BlendOpOverGetter;
    struct BlendOpInGetter;
    struct BlendOpOutGetter;
    struct BlendOpAddGetter;

  } // namespace node

  /// Get node info from blend_operation
  inline node_info get_blend_op_node_info(blend_operation op)
  {
    switch (op) {
      case blend_operation::src:
        return get_node_info<node::BlendOpSrc>();
      case blend_operation::dst:
        return get_node_info<node::BlendOpDst>();
      case blend_operation::over:
        return get_node_info<node::BlendOpOver>();
      case blend_operation::in:
        return get_node_info<node::BlendOpIn>();
      case blend_operation::out:
        return get_node_info<node::BlendOpOut>();
      case blend_operation::add:
        return get_node_info<node::BlendOpAdd>();
    }
    // default: alpha overlay
    return get_node_info<node::BlendOpOver>();
  }

  /// Get node info from blend_operation
  inline node_info get_blend_op_getter_node_info(blend_operation op)
  {
    switch (op) {
      case blend_operation::src:
        return get_node_info<node::BlendOpSrcGetter>();
      case blend_operation::dst:
        return get_node_info<node::BlendOpDstGetter>();
      case blend_operation::over:
        return get_node_info<node::BlendOpOverGetter>();
      case blend_operation::in:
        return get_node_info<node::BlendOpInGetter>();
      case blend_operation::out:
        return get_node_info<node::BlendOpOutGetter>();
      case blend_operation::add:
        return get_node_info<node::BlendOpAddGetter>();
    }
    // default: alpha overlay
    return get_node_info<node::BlendOpOverGetter>();
  }

  /// Get bind info from blend_operation
  template <class BackendTag>
  bind_info get_blend_op_bind_info(blend_operation op)
  {
    switch (op) {
      case blend_operation::src:
        return get_bind_info<node::BlendOpSrc, BackendTag>();
      case blend_operation::dst:
        return get_bind_info<node::BlendOpDst, BackendTag>();
      case blend_operation::over:
        return get_bind_info<node::BlendOpOver, BackendTag>();
      case blend_operation::in:
        return get_bind_info<node::BlendOpIn, BackendTag>();
      case blend_operation::out:
        return get_bind_info<node::BlendOpOut, BackendTag>();
      case blend_operation::add:
        return get_bind_info<node::BlendOpAdd, BackendTag>();
    }
    // default: alpha overlay
    return get_bind_info<node::BlendOpOver, BackendTag>();
  }

  /// Get bind info from blend_operation
  template <class BackendTag>
  bind_info get_blend_op_getter_bind_info(blend_operation op)
  {
    switch (op) {
      case blend_operation::src:
        return get_bind_info<node::BlendOpSrcGetter, BackendTag>();
      case blend_operation::dst:
        return get_bind_info<node::BlendOpDstGetter, BackendTag>();
      case blend_operation::over:
        return get_bind_info<node::BlendOpOverGetter, BackendTag>();
      case blend_operation::in:
        return get_bind_info<node::BlendOpInGetter, BackendTag>();
      case blend_operation::out:
        return get_bind_info<node::BlendOpOutGetter, BackendTag>();
      case blend_operation::add:
        return get_bind_info<node::BlendOpAddGetter, BackendTag>();
    }
    // default: alpha overlay
    return get_bind_info<node::BlendOpOverGetter, BackendTag>();
  }

  /// Get list of node info from blend_operation
  inline std::vector<node_info> get_blend_op_node_info_list()
  {
    std::vector<node_info> ret = {
      get_blend_op_node_info(blend_operation::src),
      get_blend_op_node_info(blend_operation::dst),
      get_blend_op_node_info(blend_operation::over),
      get_blend_op_node_info(blend_operation::in),
      get_blend_op_node_info(blend_operation::out),
      get_blend_op_node_info(blend_operation::add),
    };
    return ret;
  }

  /// Get list of node info from blend_operation
  inline std::vector<node_info> get_blend_op_getter_node_info_list()
  {
    std::vector<node_info> ret = {
      get_blend_op_getter_node_info(blend_operation::src),
      get_blend_op_getter_node_info(blend_operation::dst),
      get_blend_op_getter_node_info(blend_operation::over),
      get_blend_op_getter_node_info(blend_operation::in),
      get_blend_op_getter_node_info(blend_operation::out),
      get_blend_op_getter_node_info(blend_operation::add),
    };
    return ret;
  }

  /// Get list of bind info from blend_operation
  template <class BackendTag>
  std::vector<bind_info> get_blend_op_bind_info_list()
  {
    std::vector<bind_info> ret = {
      get_blend_op_bind_info<BackendTag>(blend_operation::src),
      get_blend_op_bind_info<BackendTag>(blend_operation::dst),
      get_blend_op_bind_info<BackendTag>(blend_operation::over),
      get_blend_op_bind_info<BackendTag>(blend_operation::in),
      get_blend_op_bind_info<BackendTag>(blend_operation::out),
      get_blend_op_bind_info<BackendTag>(blend_operation::add),
    };
    return ret;
  }

  /// Get list of bind info from blend_operation
  template <class BackendTag>
  std::vector<bind_info> get_blend_op_getter_bind_info_list()
  {
    std::vector<bind_info> ret = {
      get_blend_op_getter_bind_info<BackendTag>(blend_operation::src),
      get_blend_op_getter_bind_info<BackendTag>(blend_operation::dst),
      get_blend_op_getter_bind_info<BackendTag>(blend_operation::over),
      get_blend_op_getter_bind_info<BackendTag>(blend_operation::in),
      get_blend_op_getter_bind_info<BackendTag>(blend_operation::out),
      get_blend_op_getter_bind_info<BackendTag>(blend_operation::add),
    };
    return ret;
  }

#define YAVE_DECL_BLEND_OP_NODE_INFO(TYPE)              \
  template <>                                           \
  struct node_info_traits<node::TYPE>                   \
  {                                                     \
    static node_info get_node_info()                    \
    {                                                   \
      return node_info(#TYPE, {"src", "dst"}, {"out"}); \
    }                                                   \
  }

  YAVE_DECL_BLEND_OP_NODE_INFO(BlendOpSrc);
  YAVE_DECL_BLEND_OP_NODE_INFO(BlendOpDst);
  YAVE_DECL_BLEND_OP_NODE_INFO(BlendOpOver);
  YAVE_DECL_BLEND_OP_NODE_INFO(BlendOpIn);
  YAVE_DECL_BLEND_OP_NODE_INFO(BlendOpOut);
  YAVE_DECL_BLEND_OP_NODE_INFO(BlendOpAdd);

#define YAVE_DECL_BLEND_OP_GETTER_NODE_INFO(TYPE)         \
  template <>                                             \
  struct node_info_traits<node::TYPE##Getter>             \
  {                                                       \
    static node_info get_node_info()                      \
    {                                                     \
      return node_info(#TYPE "Getter", {}, {"op"}, true); \
    }                                                     \
  }

  YAVE_DECL_BLEND_OP_GETTER_NODE_INFO(BlendOpSrc);
  YAVE_DECL_BLEND_OP_GETTER_NODE_INFO(BlendOpDst);
  YAVE_DECL_BLEND_OP_GETTER_NODE_INFO(BlendOpOver);
  YAVE_DECL_BLEND_OP_GETTER_NODE_INFO(BlendOpIn);
  YAVE_DECL_BLEND_OP_GETTER_NODE_INFO(BlendOpOut);
  YAVE_DECL_BLEND_OP_GETTER_NODE_INFO(BlendOpAdd);
} // namespace yave