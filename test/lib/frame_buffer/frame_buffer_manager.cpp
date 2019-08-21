//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/frame_buffer/frame_buffer_manager.hpp>
#include <yave/backend/default/render/frame_buffer_constructor.hpp>

#include <catch2/catch.hpp>

using namespace yave;
using namespace yave::backend::default_common;
using namespace yave::backend::tags;

TEST_CASE("frame_buffer")
{
  frame_buffer_manager mngr {(uint32_t)-1, (uint32_t)-1, image_format::RGBA32F};

  auto info = get_node_info<FrameBufferConstructor>();
  auto bind = get_bind_info<FrameBufferConstructor, default_render>(mngr);

  REQUIRE(info.name() == bind.name());
  REQUIRE(info.input_sockets() == bind.input_sockets());
  REQUIRE(info.output_sockets().front() == bind.output_socket());
  REQUIRE(info.output_sockets().size() == 1);

  auto pc       = make_object<PrimitiveContainer>();
  auto instance = bind.get_instance(pc);

  REQUIRE(
    same_type(get_type(instance), object_type<closure<Frame, FrameBuffer>>()));

  auto app = instance << new Frame;
  check_type<FrameBuffer>(app);
  auto fb = eval(app);
}