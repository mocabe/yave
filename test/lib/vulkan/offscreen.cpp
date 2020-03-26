//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/offscreen_render_pass.hpp>
#include <yave/lib/vulkan/offscreen_renderer_2D.hpp>
#include <yave/lib/glfw/glfw_context.hpp>
#include <yave/lib/image/image.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/gil/extension/io/bmp.hpp>
#include <catch2/catch.hpp>

#include <iostream>

using namespace yave;
using namespace vulkan;

TEST_CASE("rgba32f compos pass")
{
  vulkan::vulkan_context ctx;
  vulkan::offscreen_context offscreen(ctx);
  vulkan::rgba32f_offscreen_render_pass compos {160, 90, offscreen};

  boost::gil::rgba32f_image_t img {160, 90};

  for (auto&& pix : boost::gil::view(img)) {
    pix[0] = 3.14f;
    pix[1] = 3.14f;
    pix[2] = 3.14f;
    pix[3] = 3.14f;
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

  compos.begin_pass();
  compos.end_pass();
  compos.load_frame(view(img));
  compos.store_frame(const_view(img2));
  compos.begin_pass();
  compos.end_pass();
}

TEST_CASE("rgba32f compos 2D")
{
  vulkan_context ctx;
  uint32_t width  = 1280;
  uint32_t height = 720;
  rgba32f_offscreen_renderer_2D renderer(width, height, ctx);

  SECTION("triangle")
  {
    draw2d_data data;
    draw2d_list list;
    list.idx_buffer = {{0}, {1}, {2}};
    list.vtx_buffer = {{{0, 0}, {}, {1, 0, 0, 1}},
                       {{width, height / 2}, {}, {0, 1, 0, 1}},
                       {{width / 2, height}, {}, {0, 0, 1, 1}}};
    list.cmd_buffer = {
      {3, 0, 0, {}, {{0, 0}, {renderer.width(), renderer.height()}}}};
    data.draw_lists.push_back(list);
    data.draw_lists.push_back(list);

    auto start = std::chrono::high_resolution_clock::now();

    for (auto i = 0; i < 100; ++i)
      renderer.render(data);

    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
    std::cout << "Time elapsed: " << dur.count() << std::endl;

    boost::gil::rgba32f_image_t img(renderer.width(), renderer.height(), 0);
    renderer.load_frame(view(img));

    boost::gil::rgb8_image_t out_image(renderer.width(), renderer.height());
    boost::gil::copy_and_convert_pixels(view(img), view(out_image));

    auto path =
      boost::dll::program_location().remove_filename() / "triangle.bmp";

    boost::gil::write_view(
      path.string(), view(out_image), boost::gil::bmp_tag());
  }

  SECTION("rect")
  {
    draw2d_data data;
    draw2d_list list;

    uint32_t d                = 50;
    std::vector<glm::vec2> ps = {
      {d, d}, {width - d, d}, {width - d, height - d}, {d, height - d}};

    list.add_polygon(
      ps.data(),
      ps.size(),
      {0, 0, 1, 1},
      polygon_flags(),
      {{0, 0}, {width, height}},
      renderer.default_texture());

    list.add_polyline(
      ps.data(),
      ps.size(),
      {0, 1, 0, 1},
      10,
      polyline_flags::closed,
      {{0, 0}, {width, height}},
      renderer.default_texture());

    data.draw_lists.push_back(std::move(list));

    renderer.render(data);

    boost::gil::rgba32f_image_t img(renderer.width(), renderer.height(), 0);
    renderer.load_frame(view(img));

    boost::gil::rgb8_image_t out_image(renderer.width(), renderer.height());
    boost::gil::copy_and_convert_pixels(view(img), view(out_image));

    auto path = boost::dll::program_location().remove_filename() / "rect.bmp";

    boost::gil::write_view(
      path.string(), view(out_image), boost::gil::bmp_tag());
  }
}