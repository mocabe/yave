//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/time/time.hpp>
#include <yave/signal/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/obj/primitive/property.hpp>

namespace yave {

  auto node_declaration_traits<node::Time::Time>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Time.Time",
      "Constructor of time value. \n"
      "Creates FrameTime object which represents current time applied.",
      node_declaration_visibility::_public,
      {},
      {"time"});
  }

  auto node_declaration_traits<node::Time::Seconds>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Time.ToSeconds",
      "Get float seconds from time value",
      node_declaration_visibility::_public,
      {"time"},
      {"seconds"});
  }

  auto node_declaration_traits<node::Time::FromSeconds>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Time.FromSeconds",
      "Make time from seconds",
      node_declaration_visibility::_public,
      {"time"},
      {"seconds"},
      {{0, make_node_argument<Float>()}});
  }

  namespace modules::_std::time {

    struct TimeConstructor : SignalFunction<TimeConstructor, FrameTime>
    {
      auto code() const -> return_type
      {
        return arg_time();
      }
    };

    struct ToSeconds : SignalFunction<ToSeconds, FrameTime, Float>
    {
      auto code() const -> return_type
      {
        return make_object<Float>(eval_arg<0>()->seconds().count());
      }
    };

    struct FromSeconds : SignalFunction<FromSeconds, Float, FrameTime>
    {
      auto code() const -> return_type
      {
        return make_object<FrameTime>(yave::media::time::seconds(*eval_arg<0>()));
      }
    };
  } // namespace modules::_std::time

  auto node_definition_traits<node::Time::Time, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Time::Time>();

    return {node_definition(
      info.full_name(),
      0,
      make_object<modules::_std::time::TimeConstructor>())};
  }

  auto node_definition_traits<node::Time::Seconds, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Time::Seconds>();

    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::time::ToSeconds>())};
  }

  auto node_definition_traits<node::Time::FromSeconds, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Time::FromSeconds>();

    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::time::FromSeconds>())};
  }
} // namespace yave