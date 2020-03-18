//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/image/image.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/filesystem/path.hpp>
#include <yave/obj/image_buffer/image_buffer.hpp>
#include <yave/lib/image_buffer/image_buffer_manager.hpp>

#include <yave/lib/image/image_io.hpp>
#include <fmt/format.h>

namespace yave {

  auto node_declaration_traits<node::Image>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Image", "std::image", "Load image form file", {"file"}, {"image"});
  }

  namespace modules::_std::image {

    // load image from path
    struct LoadImage : NodeFunction<LoadImage, FilesystemPath, ImageBuffer>
    {
      LoadImage(const object_ptr<const ImageBufferPool>& pool)
        : pool {pool}
      {
      }

      return_type code() const
      {
        auto path = eval_arg<0>();

        auto img = load_image_auto(*path);

        if (img.empty()) {
          throw std::runtime_error(
            fmt::format("Could not open image fild: {}", path->c_str()));
        }

        // create new image buffer
        auto buff = make_object<ImageBuffer>(
          pool, img.width(), img.height(), img.image_format());

        assert(buff->view().width() == img.width());
        assert(buff->view().height() == img.height());

        // copy data into buffer
        std::memcpy(buff->view().data(), img.data(), img.byte_size());

        return buff;
      }

      // ref to backend buffer manager
      object_ptr<const ImageBufferPool> pool;
    };

  } // namespace modules::_std::image

  auto node_definition_traits<node::Image, modules::_std::tag>::
    get_node_definitions(image_buffer_manager& mngr)
      -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Image>();

    return std::vector {node_definition(
      info.name(),
      0,
      make_object<modules::_std::image::LoadImage>(mngr.get_pool_object()),
      info.description())};
  }
} // namespace yave