//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/composition_pipeline.hpp>
#include <yave/lib/glfw/glfw_context.hpp>
#include <yave/lib/image/image.hpp>

#include <catch2/catch.hpp>

#include <iostream>

using namespace yave;

TEST_CASE("rgba32f_compisotor_pipeline")
{
  glfw::glfw_context glfw;
  vulkan::vulkan_context ctx {glfw};
  vulkan::rgba32f_composition_pipeline compos {160, 90, ctx};

  boost::gil::rgba32f_image_t img {160, 90};

  for (auto&& pix : boost::gil::view(img)) {
    pix[0] = 3.14;
    pix[1] = 3.14;
    pix[2] = 3.14;
    pix[3] = 3.14;
  }

  // store image
  compos.store_frame(boost::gil::const_view(img));

  // load back image
  boost::gil::rgba32f_image_t img2 {160, 90};
  compos.load_frame(boost::gil::view(img2));

  for (auto&& pix : boost::gil::const_view(img2)) {
    REQUIRE(std::abs(pix[0] - 3.14) < 1e-5);
    REQUIRE(std::abs(pix[1] - 3.14) < 1e-5);
    REQUIRE(std::abs(pix[2] - 3.14) < 1e-5);
    REQUIRE(std::abs(pix[3] - 3.14) < 1e-5);
  }

  std::cin.get();
  std::cout << "begin_draw()\n";
  auto cmd = compos.begin_draw();

  std::cin.get();
  std::cout << "end_draw()\n";
  compos.end_draw();
}
