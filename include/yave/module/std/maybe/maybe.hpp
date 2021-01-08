//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::Maybe {

    class Just;
    class Nothing;
    class IsJust;
    class IsNothing;
    class GetValue;
    class Map;

  } // namespace node::Maybe

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Maybe::Just);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Maybe::Nothing);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Maybe::IsJust);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Maybe::IsNothing);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Maybe::GetValue);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Maybe::Map);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Maybe::Just, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Maybe::Nothing, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Maybe::IsJust, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Maybe::IsNothing, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Maybe::GetValue, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Maybe::Map, modules::_std::tag);
}