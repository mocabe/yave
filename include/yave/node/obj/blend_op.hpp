//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/lib/blend_operation.hpp>
#include <yave/node/core/get_info.hpp>

namespace yave {

  struct BlendOpSrc;
  struct BlendOpDst;
  struct BlendOpOver;
  struct BlendOpIn;
  struct BlendOpOut;
  struct BlendOpAdd;

  /// Get node info from blend_operation
  node_info get_blend_op_node_info(blend_operation op)
  {
    switch (op) {
      case blend_operation::src:
        return get_node_info<BlendOpSrc>();
      case blend_operation::dst:
        return get_node_info<BlendOpDst>();
      case blend_operation::over:
        return get_node_info<BlendOpOver>();
      case blend_operation::in:
        return get_node_info<BlendOpIn>();
      case blend_operation::out:
        return get_node_info<BlendOpOut>();
      case blend_operation::add:
        return get_node_info<BlendOpAdd>();
    }
    // default: alpha overlay
    return get_node_info<BlendOpOver>();
  }

  /// Get bind info from blend_operation
  template <class BackendTag>
  bind_info get_blend_op_bind_info(blend_operation op)
  {
    switch (op) {
      case blend_operation::src:
        return get_bind_info<BlendOpSrc, BackendTag>();
      case blend_operation::dst:
        return get_bind_info<BlendOpDst, BackendTag>();
      case blend_operation::over:
        return get_bind_info<BlendOpOver, BackendTag>();
      case blend_operation::in:
        return get_bind_info<BlendOpIn, BackendTag>();
      case blend_operation::out:
        return get_bind_info<BlendOpOut, BackendTag>();
      case blend_operation::add:
        return get_bind_info<BlendOpAdd, BackendTag>();
    }
    // default: alpha overlay
    return get_bind_info<BlendOpOver>();
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
  struct node_info_traits<BlendOpSrc>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpSrc", {"src", "dst"}, {"out"});
    }
  };

  template <>
  struct node_info_traits<BlendOpDst>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpDst", {"src", "dst"}, {"out"});
    }
  };

  template <>
  struct node_info_traits<BlendOpOver>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpOver", {"src", "dst"}, {"out"});
    }
  };

  template <>
  struct node_info_traits<BlendOpIn>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpIn", {"src", "dst"}, {"out"});
    }
  };

  template <>
  struct node_info_traits<BlendOpOut>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpOut", {"src", "dst"}, {"out"});
    }
  };

  template <>
  struct node_info_traits<BlendOpAdd>
  {
    static node_info get_node_info()
    {
      return node_info("BlendOpAdd", {"src", "dst"}, {"out"});
    }
  };
} // namespace yave