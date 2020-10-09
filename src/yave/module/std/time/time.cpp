//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/time/time.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/obj/primitive/property.hpp>

namespace yave {

  auto node_declaration_traits<node::Time::Time>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Std.Time.Time",
      "Constructor of time value. This node construct new FrameTime object "
      "from argument, or creates FrameTime object which represents current "
      "time applied to the tree.",
      {"value"},
      {"value"},
      {{0, make_object<modules::_std::core::GetCurrentTime>()}});
  }

  auto node_declaration_traits<node::Time::Seconds>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Std.Time.ToSeconds",
      "Get float seconds from time",
      {"time"},
      {"seconds"},
      {{0, make_object<modules::_std::core::GetCurrentTime>()}});
  }

  auto node_declaration_traits<node::Time::FromSeconds>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Std.Time.FromSeconds",
      "Make time from seconds",
      {"time"},
      {"seconds"},
      {{0, make_node_argument<Float>()}});
  }

  namespace modules::_std::time {
    /// FrameTime -> FrameTime
    struct TimeConstructor : NodeFunction<TimeConstructor, FrameTime, FrameTime>
    {
      auto code() const -> return_type
      {
        return arg<0>();
      }
    };

    struct Seconds : NodeFunction<Seconds, FrameTime, Float>
    {
      auto code() const -> return_type
      {
        return make_object<Float>(
          static_cast<float>(eval_arg<0>()->seconds().count()));
      }
    };

    struct FromSeconds : NodeFunction<FromSeconds, Float, FrameTime>
    {
      auto code() const -> return_type
      {
        return make_object<FrameTime>(yave::time::seconds(*eval_arg<0>()));
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
      info.full_name(), 0, make_object<modules::_std::time::Seconds>())};
  }

  auto node_definition_traits<node::Time::FromSeconds, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Time::FromSeconds>();

    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::time::FromSeconds>())};
  }
} // namespace yave