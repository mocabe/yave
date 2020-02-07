//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/config.hpp>
#include <yave/module/std/core/decl/list.hpp>

namespace yave {

  namespace modules::_std::core {

    class ListNil_X;
    class ListCons_X;
    class ListHead_X;
    class ListTail_X;

    struct ListNil : NodeFunction<ListNil, List<forall<ListNil_X>>>
    {
      return_type code() const
      {
        return make_list<VarValueProxy<ListNil_X>>();
      }
    };

    struct ListCons : NodeFunction<
                        ListCons,
                        forall<ListCons_X>,
                        List<forall<ListCons_X>>,
                        List<forall<ListCons_X>>>
    {
      return_type code() const
      {
        // arg<0>(): VarValueProxy<ListCons_X>
        // arg<1>(): List<VarValueProxy<ListCons_X>>
        return make_object<List<VarValueProxy<ListCons_X>>>(arg<0>(), arg<1>());
      }
    };

    struct ListHead
      : NodeFunction<ListHead, List<forall<ListHead_X>>, forall<ListHead_X>>
    {
      return_type code() const
      {
        return eval_arg<0>()->head();
      }
    };

    struct ListTail : NodeFunction<
                        ListTail,
                        List<forall<ListTail_X>>,
                        List<forall<ListTail_X>>>
    {
      return_type code() const
      {
        return eval_arg<0>()->tail();
      }
    };
  } // namespace modules::_std::core

  template <>
  struct node_definition_traits<node::ListNil, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::ListNil>();
      return {node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<yave::modules::_std::core::ListNil>>(),
        info.name())};
    }
  };

  template <>
  struct node_definition_traits<node::ListCons, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::ListCons>();
      return {node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<yave::modules::_std::core::ListCons>>(),
        info.name())};
    }
  };

  template <>
  struct node_definition_traits<node::ListDecompose, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::ListDecompose>();

      auto d1 = node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<yave::modules::_std::core::ListHead>>(),
        info.description());

      auto d2 = node_definition(
        info.name(),
        1,
        make_object<
          InstanceGetterFunction<yave::modules::_std::core::ListTail>>(),
        info.description());

      return {std::move(d1), std::move(d2)};
    }
  };
} // namespace yave