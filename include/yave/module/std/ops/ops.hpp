//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::Ops {
    struct Add;
    struct Sub;
    struct Mul;
    struct Div;
    struct Eq;
    struct Neq;
    struct Less;
    struct LessEq;
    struct Greater;
    struct GreaterEq;
  } // namespace node::Ops

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Ops::Add);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Ops::Sub);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Ops::Mul);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Ops::Div);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Ops::Eq);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Ops::Neq);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Ops::Less);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Ops::LessEq);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Ops::Greater);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Ops::GreaterEq);
} // namespace yave