//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/ops/ops.hpp>

namespace yave {

#define BINOP_DECL_DEF(NAME, IS1, IS2, OS, DESC)                        \
  auto node_declaration_traits<node::Ops::NAME>::get_node_declaration() \
    ->node_declaration                                                  \
  {                                                                     \
    return function_node_declaration(                                   \
      "Ops." #NAME,                                                     \
      DESC,                                                             \
      node_declaration_visibility::_public,                             \
      {IS1, IS2},                                                       \
      {OS});                                                            \
  }

  BINOP_DECL_DEF(Add, "x", "y", "x+y", "");
  BINOP_DECL_DEF(Sub, "x", "y", "x-y", "");
  BINOP_DECL_DEF(Mul, "x", "y", "x*y", "");
  BINOP_DECL_DEF(Div, "x", "y", "x/y", "");
  BINOP_DECL_DEF(Eq, "x", "y", "x==y", "");
  BINOP_DECL_DEF(Neq, "x", "y", "x!=y", "");
  BINOP_DECL_DEF(Less, "x", "y", "x<y", "");
  BINOP_DECL_DEF(LessEq, "x", "y", "x<=y", "");
  BINOP_DECL_DEF(Greater, "x", "y", "x>y", "");
  BINOP_DECL_DEF(GreaterEq, "x", "y", "x>=y", "");

} // namespace yave