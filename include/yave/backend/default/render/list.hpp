//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/class/list.hpp>

namespace yave {

  namespace backend::default_render {

    class ListNil_X;
    class ListCons_X;
    class ListDecompose_Head_X;
    class ListDecompose_Tail_X;

    struct ListNil : NodeFunction<ListNil, List<forall<ListNil_X>>>
    {
      return_type code() const
      {
        return make_list<forall<ListNil_X>>();
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
        return make_object<List<forall<ListCons_X>>>(arg<0>(), arg<1>());
      }
    };

    struct ListDecompose_Head : NodeFunction<
                                  ListDecompose_Head,
                                  List<forall<ListDecompose_Head_X>>,
                                  forall<ListDecompose_Head_X>>
    {
      return_type code() const
      {
        return eval_arg<0>()->head();
      }
    };

    struct ListDecompose_Tail : NodeFunction<
                                  ListDecompose_Tail,
                                  List<forall<ListDecompose_Tail_X>>,
                                  List<forall<ListDecompose_Tail_X>>>
    {
      return_type code() const
      {
        return eval_arg<0>()->tail();
      }
    };
  } // namespace backend::default_render

  template <>
  struct node_definition_traits<node::ListNil, backend::tags::default_render>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::ListNil>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<
          InstanceGetterFunction<yave::backend::default_render::ListNil>>(),
        info.name())};
    }
  };

  template <>
  struct node_definition_traits<node::ListCons, backend::tags::default_render>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::ListCons>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<
          InstanceGetterFunction<yave::backend::default_render::ListCons>>(),
        info.name())};
    }
  };

  template <>
  struct node_definition_traits<
    node::ListDecompose_Head,
    backend::tags::default_render>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::ListDecompose_Head>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<InstanceGetterFunction<
          yave::backend::default_render::ListDecompose_Head>>(),
        info.name())};
    }
  };

  template <>
  struct node_definition_traits<
    node::ListDecompose_Tail,
    backend::tags::default_render>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::ListDecompose_Tail>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<InstanceGetterFunction<
          yave::backend::default_render::ListDecompose_Tail>>(),
        info.name())};
    }
  };
}