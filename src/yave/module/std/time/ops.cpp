//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/time/ops.hpp>
#include <yave/signal/function.hpp>
#include <yave/signal/generator.hpp>

#include <functional>

namespace yave {

  namespace modules::_std::time {

    using T = FrameTime;

    using Add       = BinarySignalFunction<T, T, T, std::plus<>>;
    using Sub       = BinarySignalFunction<T, T, T, std::minus<>>;
    using Eq        = BinarySignalFunction<T, T, Bool, std::equal_to<>>;
    using Neq       = BinarySignalFunction<T, T, Bool, std::not_equal_to<>>;
    using Less      = BinarySignalFunction<T, T, Bool, std::less<>>;
    using LessEq    = BinarySignalFunction<T, T, Bool, std::less_equal<>>;
    using Greater   = BinarySignalFunction<T, T, Bool, std::greater<>>;
    using GreaterEq = BinarySignalFunction<T, T, Bool, std::greater_equal<>>;

  } // namespace modules::_std::time

#define TIME_BINOP_DEF(NAME)                                               \
  auto node_definition_traits<node::Ops::NAME, modules::_std::time::tag>:: \
    get_node_definitions()                                                 \
      ->std::vector<node_definition>                                       \
  {                                                                        \
    auto info = get_node_declaration<node::Ops::NAME>();                   \
                                                                           \
    return {node_definition(                                               \
      info.full_name(), 0, make_object<modules::_std::time::##NAME>())};   \
  }

  TIME_BINOP_DEF(Add);
  TIME_BINOP_DEF(Sub);
  TIME_BINOP_DEF(Eq);
  TIME_BINOP_DEF(Neq);
  TIME_BINOP_DEF(Less);
  TIME_BINOP_DEF(LessEq);
  TIME_BINOP_DEF(Greater);
  TIME_BINOP_DEF(GreaterEq);
}