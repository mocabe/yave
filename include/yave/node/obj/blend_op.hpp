//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/image/blend_operation.hpp>
#include <yave/node/core/get_info.hpp>

namespace yave {

  namespace node {
    struct BlendOpSrc;
    struct BlendOpDst;
    struct BlendOpOver;
    struct BlendOpIn;
    struct BlendOpOut;
    struct BlendOpAdd;
  } // namespace node

  /// Get node info from blend_operation
  node_info get_blend_op_node_info(blend_operation op)
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

  /// Get list of node info from blend_operation
  std::vector<node_info> get_blend_op_node_info_list()
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

  template <>
  struct node_info_traits<node::BlendOpSrc>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpSrc", {"src", "dst"}, {"out"});
    }
  };

  template <>
  struct node_info_traits<node::BlendOpDst>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpDst", {"src", "dst"}, {"out"});
    }
  };

  template <>
  struct node_info_traits<node::BlendOpOver>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpOver", {"src", "dst"}, {"out"});
    }
  };

  template <>
  struct node_info_traits<node::BlendOpIn>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpIn", {"src", "dst"}, {"out"});
    }
  };

  template <>
  struct node_info_traits<node::BlendOpOut>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpOut", {"src", "dst"}, {"out"});
    }
  };

  template <>
  struct node_info_traits<node::BlendOpAdd>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpAdd", {"src", "dst"}, {"out"});
    }
  };
} // namespace yave