//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/lib/image/blend_operation.hpp>

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

  inline auto get_blend_op_node_declaration(blend_operation op)
    -> node_declaration
  {
    switch (op) {
      case blend_operation::src:
        return get_node_declaration<node::BlendOpSrc>();
      case blend_operation::dst:
        return get_node_declaration<node::BlendOpDst>();
      case blend_operation::over:
        return get_node_declaration<node::BlendOpOver>();
      case blend_operation::in:
        return get_node_declaration<node::BlendOpIn>();
      case blend_operation::out:
        return get_node_declaration<node::BlendOpOut>();
      case blend_operation::add:
        return get_node_declaration<node::BlendOpAdd>();
    }
    unreachable();
  }

  inline auto get_blend_op_getter_node_declaration(blend_operation op)
    -> node_declaration
  {
    switch (op) {
      case blend_operation::src:
        return get_node_declaration<node::BlendOpSrcGetter>();
      case blend_operation::dst:
        return get_node_declaration<node::BlendOpDstGetter>();
      case blend_operation::over:
        return get_node_declaration<node::BlendOpOverGetter>();
      case blend_operation::in:
        return get_node_declaration<node::BlendOpInGetter>();
      case blend_operation::out:
        return get_node_declaration<node::BlendOpOutGetter>();
      case blend_operation::add:
        return get_node_declaration<node::BlendOpAddGetter>();
    }
    unreachable();
  }

  template <class BackendTag>
  auto get_blend_op_node_definition(blend_operation op) -> node_definition
  {
    switch (op) {
      case blend_operation::src:
        return get_node_definition<node::BlendOpSrc, BackendTag>();
      case blend_operation::dst:
        return get_node_definition<node::BlendOpDst, BackendTag>();
      case blend_operation::over:
        return get_node_definition<node::BlendOpOver, BackendTag>();
      case blend_operation::in:
        return get_node_definition<node::BlendOpIn, BackendTag>();
      case blend_operation::out:
        return get_node_definition<node::BlendOpOut, BackendTag>();
      case blend_operation::add:
        return get_node_definition<node::BlendOpAdd, BackendTag>();
    }
    unreachable();
  }

  template <class BackendTag>
  auto get_blend_op_getter_node_definition(blend_operation op)
    -> node_definition
  {
    switch (op) {
      case blend_operation::src:
        return get_node_definition<node::BlendOpSrcGetter, BackendTag>();
      case blend_operation::dst:
        return get_node_definition<node::BlendOpDstGetter, BackendTag>();
      case blend_operation::over:
        return get_node_definition<node::BlendOpOverGetter, BackendTag>();
      case blend_operation::in:
        return get_node_definition<node::BlendOpInGetter, BackendTag>();
      case blend_operation::out:
        return get_node_definition<node::BlendOpOutGetter, BackendTag>();
      case blend_operation::add:
        return get_node_definition<node::BlendOpAddGetter, BackendTag>();
    }
    unreachable();
  }

  inline auto get_blend_op_node_declaration_list()
    -> std::vector<node_declaration>
  {
    std::vector ret = {
      get_blend_op_node_declaration(blend_operation::src),
      get_blend_op_node_declaration(blend_operation::dst),
      get_blend_op_node_declaration(blend_operation::over),
      get_blend_op_node_declaration(blend_operation::in),
      get_blend_op_node_declaration(blend_operation::out),
      get_blend_op_node_declaration(blend_operation::add),
    };
    return ret;
  }

  inline auto get_blend_op_getter_node_declaration_list()
    -> std::vector<node_declaration>
  {
    std::vector ret = {
      get_blend_op_getter_node_declaration(blend_operation::src),
      get_blend_op_getter_node_declaration(blend_operation::dst),
      get_blend_op_getter_node_declaration(blend_operation::over),
      get_blend_op_getter_node_declaration(blend_operation::in),
      get_blend_op_getter_node_declaration(blend_operation::out),
      get_blend_op_getter_node_declaration(blend_operation::add),
    };
    return ret;
  }

  template <class BackendTag>
  auto get_blend_op_node_definition_list() -> std::vector<node_definition>
  {
    std::vector ret = {
      get_blend_op_node_definition<BackendTag>(blend_operation::src),
      get_blend_op_node_definition<BackendTag>(blend_operation::dst),
      get_blend_op_node_definition<BackendTag>(blend_operation::over),
      get_blend_op_node_definition<BackendTag>(blend_operation::in),
      get_blend_op_node_definition<BackendTag>(blend_operation::out),
      get_blend_op_node_definition<BackendTag>(blend_operation::add),
    };
    return ret;
  }

  template <class BackendTag>
  auto get_blend_op_getter_node_definition_list()
    -> std::vector<node_definition>
  {
    std::vector ret = {
      get_blend_op_getter_node_definition<BackendTag>(blend_operation::src),
      get_blend_op_getter_node_definition<BackendTag>(blend_operation::dst),
      get_blend_op_getter_node_definition<BackendTag>(blend_operation::over),
      get_blend_op_getter_node_definition<BackendTag>(blend_operation::in),
      get_blend_op_getter_node_definition<BackendTag>(blend_operation::out),
      get_blend_op_getter_node_definition<BackendTag>(blend_operation::add),
    };
    return ret;
  }

#define YAVE_DECL_BLEND_OP_NODE(TYPE)                                        \
  template <>                                                                \
  struct node_declaration_traits<node::TYPE>                                 \
  {                                                                          \
    static auto get_node_declaration() -> node_declaration                   \
    {                                                                        \
      return node_declaration(                                               \
        #TYPE,                                                               \
        {"src", "dst"},                                                      \
        {"out"},                                                             \
        node_type::normal,                                                   \
        object_type<node_closure<FrameBuffer, FrameBuffer, FrameBuffer>>()); \
    }                                                                        \
  }

  YAVE_DECL_BLEND_OP_NODE(BlendOpSrc);
  YAVE_DECL_BLEND_OP_NODE(BlendOpDst);
  YAVE_DECL_BLEND_OP_NODE(BlendOpOver);
  YAVE_DECL_BLEND_OP_NODE(BlendOpIn);
  YAVE_DECL_BLEND_OP_NODE(BlendOpOut);
  YAVE_DECL_BLEND_OP_NODE(BlendOpAdd);

#define YAVE_DECL_BLEND_OP_GETTER_NODE(TYPE)                        \
  template <>                                                       \
  struct node_declaration_traits<node::TYPE##Getter>                \
  {                                                                 \
    static auto get_node_declaration() -> node_declaration          \
    {                                                               \
      return node_declaration(                                      \
        #TYPE "Getter",                                             \
        {},                                                         \
        {"op"},                                                     \
        node_type::normal,                                          \
        object_type<node_closure<                                   \
          node_closure<FrameBuffer, FrameBuffer, FrameBuffer>>>()); \
    }                                                               \
  }

  YAVE_DECL_BLEND_OP_GETTER_NODE(BlendOpSrc);
  YAVE_DECL_BLEND_OP_GETTER_NODE(BlendOpDst);
  YAVE_DECL_BLEND_OP_GETTER_NODE(BlendOpOver);
  YAVE_DECL_BLEND_OP_GETTER_NODE(BlendOpIn);
  YAVE_DECL_BLEND_OP_GETTER_NODE(BlendOpOut);
  YAVE_DECL_BLEND_OP_GETTER_NODE(BlendOpAdd);

} // namespace yave