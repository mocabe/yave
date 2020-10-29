//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/time/retime.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/obj/primitive/property.hpp>

namespace yave {

  auto node_declaration_traits<node::Time::ReTime>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Time.ReTime",
      "Assign new time to subtree",
      node_declaration_visibility::_public,
      {"any", "time"},
      {"any"});
  }

  auto node_declaration_traits<node::Time::Delay>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Time.Delay",
      "Delay time",
      node_declaration_visibility::_public,
      {"any", "time"},
      {"any"});
  }

  auto node_declaration_traits<node::Time::Scale>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Time.Scale",
      "Scale time",
      node_declaration_visibility::_public,
      {"any", "scaling"},
      {"any"},
      {{1, make_node_argument<Float>(1.f)}});
  }

  namespace modules::_std::time {

    class ReTime_X;
    class Delay_X;
    class Scale_X;

    struct ReTime : Function<
                      ReTime,
                      node_closure<forall<ReTime_X>>,
                      node_closure<FrameTime>,
                      FrameDemand,
                      forall<ReTime_X>>
    {
      return_type code() const
      {
        auto fd = eval_arg<2>();
        auto t  = eval(arg<1>() << fd);
        return arg<0>() << make_object<FrameDemand>(std::move(t));
      }
    };

    struct DelayTime : Function<
                         DelayTime,
                         node_closure<forall<Delay_X>>,
                         node_closure<FrameTime>,
                         FrameDemand,
                         forall<Delay_X>>
    {
      return_type code() const
      {
        auto fd    = eval_arg<2>();
        auto delay = eval(arg<1>() << fd);

        // t - delay
        auto delayed = *fd->time - *delay;

        auto t = make_object<FrameTime>(delayed);
        return arg<0>() << make_object<FrameDemand>(std::move(t));
      }
    };

    struct ScaleTime : Function<
                         ScaleTime,
                         node_closure<forall<Delay_X>>,
                         node_closure<Float>,
                         FrameDemand,
                         forall<Delay_X>>
    {
      return_type code() const
      {
        auto fd    = eval_arg<2>();
        auto scale = eval(arg<1>() << fd);

        auto t = make_object<FrameTime>(*fd->time * *scale);
        return arg<0>() << make_object<FrameDemand>(std::move(t));
      }
    };

  } // namespace modules::_std::time

  auto node_definition_traits<node::Time::ReTime, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Time::ReTime>();
    return {node_definition(
      get_full_name(info), 0, make_object<modules::_std::time::ReTime>())};
  }

  auto node_definition_traits<node::Time::Delay, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Time::Delay>();
    return {node_definition(
      get_full_name(info), 0, make_object<modules::_std::time::DelayTime>())};
  }

  auto node_definition_traits<node::Time::Scale, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Time::Scale>();
    return {node_definition(
      get_full_name(info), 0, make_object<modules::_std::time::ScaleTime>())};
  }
} // namespace yave