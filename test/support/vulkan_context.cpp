//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>
#include <yave/support/vulkan_context.hpp>

using namespace yave;

TEST_CASE("vulkan_context without validation")
{
  glfw_context glfw_ctx;
  vulkan_context vulkan_ctx(glfw_ctx, false);
}

TEST_CASE("vulkan_context with validation")
{
  glfw_context glfw_ctx;
  vulkan_context vulkan_ctx(glfw_ctx, true);
}

TEST_CASE("unique context")
{
  glfw_context glfw_ctx;
  vulkan_context vulkan_ctx(glfw_ctx);

  auto window     = glfw_ctx.create_window(1280, 720, "test window");
  auto window_ctx = vulkan_ctx.create_window_context(window);
}