//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/math/ops.hpp>
#include <yave/signal/function.hpp>
#include <yave/signal/generator.hpp>

#include <functional>

namespace yave {

  namespace modules::_std::math {

    template <class T>
    using TAdd = BinarySignalFunction<T, T, T, std::plus<>>;
    template <class T>
    using TSub = BinarySignalFunction<T, T, T, std::minus<>>;
    template <class T>
    using TMul = BinarySignalFunction<T, T, T, std::multiplies<>>;
    template <class T>
    using TDiv = BinarySignalFunction<T, T, T, std::divides<>>;
    template <class T>
    using TEq = BinarySignalFunction<T, T, Bool, std::equal_to<>>;
    template <class T>
    using TNeq = BinarySignalFunction<T, T, Bool, std::not_equal_to<>>;
    template <class T>
    using TLess = BinarySignalFunction<T, T, Bool, std::less<>>;
    template <class T>
    using TLessEq = BinarySignalFunction<T, T, Bool, std::less_equal<>>;
    template <class T>
    using TGreater = BinarySignalFunction<T, T, Bool, std::greater<>>;
    template <class T>
    using TGreaterEq = BinarySignalFunction<T, T, Bool, std::greater_equal<>>;

  } // namespace modules::_std::math

#define NUM_BINOP_DEF(NAME)                                                \
  auto node_definition_traits<node::Ops::NAME, modules::_std::math::tag>:: \
    get_node_definitions()                                                 \
      ->std::vector<node_definition>                                       \
  {                                                                        \
    auto info = get_node_declaration<node::Ops::NAME>();                   \
                                                                           \
    return {                                                               \
      node_definition(                                                     \
        info.full_name(),                                                  \
        0,                                                                 \
        make_object<modules::_std::math::T##NAME<Float>>()),               \
      node_definition(                                                     \
        info.full_name(),                                                  \
        0,                                                                 \
        make_object<modules::_std::math::T##NAME<Int>>())};                \
  }

  NUM_BINOP_DEF(Add);
  NUM_BINOP_DEF(Sub);
  NUM_BINOP_DEF(Mul);
  NUM_BINOP_DEF(Div);
  NUM_BINOP_DEF(Eq);
  NUM_BINOP_DEF(Neq);
  NUM_BINOP_DEF(Less);
  NUM_BINOP_DEF(LessEq);
  NUM_BINOP_DEF(Greater);
  NUM_BINOP_DEF(GreaterEq);

} // namespace yave