//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/test/test.hpp>
#include <yave/obj/shape/shape.hpp>
#include <yave/lib/shape/rect.hpp>
#include <yave/lib/shape/circle.hpp>
#include <yave/obj/color/color.hpp>
#include <yave/obj/vec/property.hpp>
#include <yave/obj/primitive/property.hpp>
#include <yave/signal/function.hpp>

namespace yave {

  auto node_declaration_traits<
    node::Test::EventLogPaths>::get_node_declaration() -> node_declaration
  {
    return function_node_declaration(
      "Test.EventLogPaths",
      "",
      node_declaration_visibility::_public,
      {"time"},
      {"shape"});
  }

  namespace modules::_std::test {

    struct EventLogPaths : SignalFunction<EventLogPaths, FrameTime, Shape>
    {
      auto code() const -> return_type
      {
        auto time = eval_arg<0>();


      }
    };
  } // namespace modules::_std::test

  auto node_definition_traits<node::Test::EventLogPaths, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Test::EventLogPaths>();
    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::test::EventLogPaths>())};
  }
}