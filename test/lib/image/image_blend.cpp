//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/image/image_blend.hpp>
#include <yave/lib/selene/conversion.hpp>
#include <catch2/catch.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>
#include <selene/img/typed/ImageTypeAliases.hpp>
#include <selene/img/interop/ImageToDynImage.hpp>

using namespace yave::sln;
using namespace yave;

TEST_CASE("RGBA_8U")
{
  SECTION("src/dst")
  {
    TypedLayout layout(PixelLength(1), PixelLength(1));
    ImageRGBA_8u src(layout);
    ImageRGBA_8u dst(layout);

    auto src_view = to_image_view(to_dyn_image_view(src.constant_view()));
    auto dst_view = to_image_view(to_dyn_image_view(dst.view()));

    for (auto&& row : src) {
      for (auto&& pixel : row) {
        pixel[0] = 's';
      }
    }

    for (auto&& row : dst) {
      for (auto&& pixel : row) {
        pixel[0] = 'd';
      }
    }

    SECTION("src")
    {
      alpha_blend_RGBA8UI(src_view, dst_view, blend_operation::src);
      for (auto&& row : dst) {
        for (auto&& pixel : row) {
          REQUIRE(pixel[0] == 's');
        }
      }
    }
    SECTION("dst")
    {
      alpha_blend_RGBA8UI(src_view, dst_view, blend_operation::dst);
      for (auto&& row : dst) {
        for (auto&& pixel : row) {
          REQUIRE(pixel[0] == 'd');
        }
      }
    }
  }

  SECTION("1on1 uint")
  {
    TypedLayout layout(PixelLength(2), PixelLength(2));
    ImageRGBA_8u src(layout);
    ImageRGBA_8u dst(layout);

    auto src_view = to_image_view(to_dyn_image_view(src.constant_view()));
    auto dst_view = to_image_view(to_dyn_image_view(dst.view()));

    auto& A_0  = *src.data(PixelIndex(0), PixelIndex(0));
    auto& A_A  = *src.data(PixelIndex(1), PixelIndex(0));
    auto& A_B  = *src.data(PixelIndex(0), PixelIndex(1));
    auto& A_AB = *src.data(PixelIndex(1), PixelIndex(1));

    A_0[0] = 0;
    A_0[3] = 0;

    A_A[0] = 'A';
    A_A[3] = 255;

    A_B[0] = 0;
    A_B[3] = 0;

    A_AB[0] = 'A';
    A_AB[3] = 255;

    auto& B_0  = *dst.data(PixelIndex(0), PixelIndex(0));
    auto& B_A  = *dst.data(PixelIndex(1), PixelIndex(0));
    auto& B_B  = *dst.data(PixelIndex(0), PixelIndex(1));
    auto& B_AB = *dst.data(PixelIndex(1), PixelIndex(1));

    B_0[0] = 0;
    B_0[3] = 0;

    B_A[0] = 0;
    B_A[3] = 0;

    B_B[0] = 'B';
    B_B[3] = 255;

    B_AB[0] = 'B';
    B_AB[3] = 255;

    SECTION("over")
    {
      alpha_blend_RGBA8UI(src_view, dst_view, blend_operation::over);
      REQUIRE(B_0[0] == 0);
      REQUIRE(B_A[0] == 'A');
      REQUIRE(B_B[0] == 'B');
      REQUIRE(B_AB[0] == 'A');
    }

    SECTION("in")
    {
      alpha_blend_RGBA8UI(src_view, dst_view, blend_operation::in);
      REQUIRE(B_0[0] == 0);
      REQUIRE(B_A[0] == 0);
      REQUIRE(B_B[0] == 0);
      REQUIRE(B_AB[0] == 'A');
    }

    SECTION("out")
    {
      alpha_blend_RGBA8UI(src_view, dst_view, blend_operation::out);
      REQUIRE(B_0[0] == 0);
      REQUIRE(B_A[0] == 'A');
      REQUIRE(B_B[0] == 0);
      REQUIRE(B_AB[0] == 0);
    }
  }

  SECTION("1on1 float")
  {
    TypedLayout layout(PixelLength(2), PixelLength(2));
    ImageRGBA_32f src(layout);
    ImageRGBA_32f dst(layout);

    auto src_view = to_image_view(to_dyn_image_view(src.constant_view()));
    auto dst_view = to_image_view(to_dyn_image_view(dst.view()));

    auto& A_0  = *src.data(PixelIndex(0), PixelIndex(0));
    auto& A_A  = *src.data(PixelIndex(1), PixelIndex(0));
    auto& A_B  = *src.data(PixelIndex(0), PixelIndex(1));
    auto& A_AB = *src.data(PixelIndex(1), PixelIndex(1));

    A_0[0] = 0.f;
    A_0[3] = 0.f;

    A_A[0] = 0.42f;
    A_A[3] = 1.f;

    A_B[0] = 0.f;
    A_B[3] = 0.f;

    A_AB[0] = 0.42f;
    A_AB[3] = 1.f;

    auto& B_0  = *dst.data(PixelIndex(0), PixelIndex(0));
    auto& B_A  = *dst.data(PixelIndex(1), PixelIndex(0));
    auto& B_B  = *dst.data(PixelIndex(0), PixelIndex(1));
    auto& B_AB = *dst.data(PixelIndex(1), PixelIndex(1));

    B_0[0] = 0.f;
    B_0[3] = 0.f;

    B_A[0] = 0.f;
    B_A[3] = 0.f;

    B_B[0] = 0.24f;
    B_B[3] = 1.f;

    B_AB[0] = 0.24f;
    B_AB[3] = 1.f;

    SECTION("over")
    {
      alpha_blend_RGBA32F(src_view, dst_view, blend_operation::over);
      REQUIRE(B_0[0] == 0);
      REQUIRE(B_A[0] == 0.42f);
      REQUIRE(B_B[0] == 0.24f);
      REQUIRE(B_AB[0] == 0.42f);
    }

    SECTION("in")
    {
      alpha_blend_RGBA32F(src_view, dst_view, blend_operation::in);
      REQUIRE(B_0[0] == 0);
      REQUIRE(B_A[0] == 0);
      REQUIRE(B_B[0] == 0);
      REQUIRE(B_AB[0] == 0.42f);
    }

    SECTION("out")
    {
      alpha_blend_RGBA32F(src_view, dst_view, blend_operation::out);
      REQUIRE(B_0[0] == 0);
      REQUIRE(B_A[0] == 0.42f);
      REQUIRE(B_B[0] == 0);
      REQUIRE(B_AB[0] == 0);
    }
  }
}