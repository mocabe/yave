//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/time/retime.hpp>
#include <yave/signal/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/obj/primitive/property.hpp>

namespace yave {

  auto node_declaration_traits<node::Time::ReTime>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Time.ReTime",
      "Apply new time to input signal",
      node_declaration_visibility::_public,
      {"any", "time"},
      {"any"});
  }

  auto node_declaration_traits<node::Time::Delay>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Time.Delay",
      "Delay time of input signal",
      node_declaration_visibility::_public,
      {"any", "time"},
      {"any"});
  }

  auto node_declaration_traits<node::Time::Scale>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Time.Scale",
      "Scale time of input signal",
      node_declaration_visibility::_public,
      {"any", "scaling"},
      {"any"},
      {{1, make_node_argument<Float>(1.f)}});
  }

  namespace modules::_std::time {

    class X;

    struct ReTime : SignalFunction<ReTime, X, FrameTime, X>
    {
      auto code() const -> return_type
      {
        return arg_signal<0>() << make_object<FrameDemand>(eval_arg<1>());
      }
    };

    struct DelayTime : SignalFunction<DelayTime, X, FrameTime, X>
    {
      auto code() const -> return_type
      {
        // t - delay
        auto t = *arg_demand()->time - *eval_arg<1>();
        return arg_signal<0>()
               << make_object<FrameDemand>(make_object<FrameTime>(t));
      }
    };

    struct ScaleTime : SignalFunction<ScaleTime, X, Float, X>
    {
      auto code() const -> return_type
      {
        // t * scale
        auto t = *arg_time() * *eval_arg<1>();
        return arg_signal<0>()
               << make_object<FrameDemand>(make_object<FrameTime>(t));
      }
    };

  } // namespace modules::_std::time

  auto node_definition_traits<node::Time::ReTime, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Time::ReTime>();
    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::time::ReTime>())};
  }

  auto node_definition_traits<node::Time::Delay, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Time::Delay>();
    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::time::DelayTime>())};
  }

  auto node_definition_traits<node::Time::Scale, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Time::Scale>();
    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::time::ScaleTime>())};
  }
} // namespace yave