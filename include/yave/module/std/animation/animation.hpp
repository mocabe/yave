//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::Animation {

    class Animation;
    class Blank;
    class IsBlank;
    class GetValue;
    class GetLength;
    class Map;
    class Concat;
    class MergeOr;
    class MergeAnd;
    class Stretch;
    class Extend;

  } // namespace node::Animation

  // clang-format off

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Animation::Animation);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Animation::Blank);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Animation::IsBlank);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Animation::GetValue);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Animation::GetLength);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Animation::Map);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Animation::Concat);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Animation::MergeOr);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Animation::MergeAnd);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Animation::Stretch);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Animation::Extend);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Animation::Animation, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Animation::Blank, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Animation::IsBlank, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Animation::GetValue, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Animation::GetLength, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Animation::Map, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Animation::Concat, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Animation::MergeOr, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Animation::MergeAnd, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Animation::Stretch, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Animation::Extend, modules::_std::tag);

  // clang-format on
}