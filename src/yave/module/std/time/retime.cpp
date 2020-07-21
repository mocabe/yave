//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/time/retime.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/frame_time/frame_time.hpp>

namespace yave {

  auto node_declaration_traits<node::ReTime>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "ReTime",
      "/std/time",
      "Assign new time to subtree",
      {"any", "time"},
      {"any"});
  }

  auto node_declaration_traits<node::DelayTime>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Delay", "/std/time", "Delay time", {"any", "delay"}, {"any"});
  }

  auto node_declaration_traits<node::ScaleTime>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Scale",
      "/std/time",
      "Scale time",
      {"any", "scale"},
      {"any"},
      {{1, make_data_type_holder<Float>(1.f)}});
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
        return arg<0>() << make_object<FrameDemand>(*t);
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
        auto delayed = fd->time - *delay;

        return arg<0>() << make_object<FrameDemand>(delayed);
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
        return arg<0>() << make_object<FrameDemand>(fd->time * *scale);
      }
    };

  } // namespace modules::_std::time

  auto node_definition_traits<node::ReTime, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::ReTime>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::time::ReTime>(),
      info.description())};
  }

  auto node_definition_traits<node::DelayTime, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::DelayTime>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::time::DelayTime>(),
      info.description())};
  }

  auto node_definition_traits<node::ScaleTime, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::ScaleTime>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::time::ScaleTime>(),
      info.description())};
  }
} // namespace yave