//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>
#include <yave/lib/vulkan/vulkan_context.hpp>

using namespace yave;

TEST_CASE("vulkan_context without validation")
{
  glfw::glfw_context glfw_ctx;
  vulkan::vulkan_context vulkan_ctx(glfw_ctx, false);
  vulkan_ctx.device().waitIdle();
}

TEST_CASE("vulkan_context with validation")
{
  glfw::glfw_context glfw_ctx;
  vulkan::vulkan_context vulkan_ctx(glfw_ctx, true);
  vulkan_ctx.device().waitIdle();
}

TEST_CASE("unique context")
{
  glfw::glfw_context glfw_ctx;
  vulkan::vulkan_context vulkan_ctx(glfw_ctx);

  auto window     = glfw_ctx.create_window(1280, 720, "test window");
  auto window_ctx = vulkan_ctx.create_window_context(window);

  for (size_t i = 0; i < 5; ++i) {
    glfw_ctx.poll_events();
    if (window_ctx.resized()) {
      window_ctx.rebuild_frame_buffers();
    }
  }

  vulkan_ctx.device().waitIdle();
}