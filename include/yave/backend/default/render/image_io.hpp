//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/class/image_io.hpp>
#include <yave/obj/filesystem/path.hpp>
#include <yave/lib/image/image_io.hpp>

namespace yave {

  namespace backend::default_render {

    /// Load image from file
    struct LoadImage : NodeFunction<LoadImage, FilesystemPath, Image>
    {
      return_type code() const
      {
        auto path  = eval_arg<0>();
        auto image = load_image_auto(*path);
        return make_object<Image>(std::move(image));
      }
    };
  } // namespace backend::default_render

  template <>
  struct node_definition_traits<node::LoadImage, backend::tags::default_render>
  {
    static auto get_node_definition() -> node_definition
    {
      auto info = get_node_declaration<node::LoadImage>();
      return node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<
          InstanceGetterFunction<backend::default_render::LoadImage>>(),
        info.name() + ": Load image from file");
    }
  };
}