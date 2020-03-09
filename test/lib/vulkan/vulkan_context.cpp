//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>
#include <yave/lib/vulkan/vulkan_context.hpp>
#include <yave/lib/vulkan/window_context.hpp>

using namespace yave;

TEST_CASE("vulkan_context without validation")
{
  vulkan::vulkan_context vulkan_ctx();
}

TEST_CASE("vulkan_context with validation")
{
  vulkan::vulkan_context vulkan_ctx(
    vulkan::vulkan_context::init_flags::enable_validation);
}

TEST_CASE("unique context")
{
  using init_flags = vulkan::vulkan_context::init_flags;
  auto flags       = init_flags::enable_validation | init_flags::enable_logging;
  vulkan::vulkan_context vulkan_ctx(flags);

  glfw::glfw_context glfw_ctx;
  auto window     = glfw_ctx.create_window(1280, 720, "test window");
  auto window_ctx = vulkan::window_context(vulkan_ctx, window);

  for (size_t i = 0; i < 5; ++i) {
    glfw_ctx.poll_events();
    if (window_ctx.resized()) {
      window_ctx.rebuild_frame_buffers();
    }
  }
}