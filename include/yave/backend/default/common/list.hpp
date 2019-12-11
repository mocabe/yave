//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/decl/list.hpp>

namespace yave {

  namespace backends::default_common {

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
  } // namespace backends::default_common

  template <>
  struct node_definition_traits<node::ListNil, backend_tags::default_common>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::ListNil>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<
          InstanceGetterFunction<yave::backends::default_common::ListNil>>(),
        info.name())};
    }
  };

  template <>
  struct node_definition_traits<node::ListCons, backend_tags::default_common>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::ListCons>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<
          InstanceGetterFunction<yave::backends::default_common::ListCons>>(),
        info.name())};
    }
  };

  template <>
  struct node_definition_traits<node::ListHead, backend_tags::default_common>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::ListHead>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<
          InstanceGetterFunction<yave::backends::default_common::ListHead>>(),
        info.name())};
    }
  };

  template <>
  struct node_definition_traits<node::ListTail, backend_tags::default_common>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::ListTail>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<
          InstanceGetterFunction<yave::backends::default_common::ListTail>>(),
        info.name())};
    }
  };
}