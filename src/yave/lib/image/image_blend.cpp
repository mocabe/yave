//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/image/image_blend.hpp>
#include <yave/lib/selene/conversion.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>
#include <selene/img/interop/DynImageToImage.hpp>

namespace yave {

  // Simple CPU alpha blending implementation for FrameBuffer.
  // TODO: Add light weight version for preview.
  // TODO: Implement with Vulkan shader.

  template <class PixelT>
  PixelT
    alpha_blend_PixelUI(const PixelT& A, const PixelT& B, blend_operation op)
  {
    using value_type = typename PixelT::value_type;
    static_assert(std::is_integral_v<value_type>);
    static_assert(std::is_unsigned_v<value_type>);
    static_assert(std::numeric_limits<value_type>::min() == 0);
    static_assert(PixelT::pixel_format == sln::PixelFormat::RGBA);

    // Use gamma=2.2 for correction.
    // We can use 2.0 for better performance, or ignore gamma for preview.
    // For 8bit colors, we can use look up table.
    const auto gamma = 2.2f;
    auto Ar          = std::pow((float)A[0], gamma);
    auto Ag          = std::pow((float)A[1], gamma);
    auto Ab          = std::pow((float)A[2], gamma);
    auto Br          = std::pow((float)B[0], gamma);
    auto Bg          = std::pow((float)B[1], gamma);
    auto Bb          = std::pow((float)B[2], gamma);

    const float scale = std::numeric_limits<value_type>::max();
    const auto Aa     = A[3] / scale; // Alpha of A [0,1]
    const auto Ba     = B[3] / scale; // Alpha of B [0,1]

    if (op == blend_operation::src) {
      return A;
    }
    if (op == blend_operation::dst) {
      return B;
    }

    auto FA = 0.f;
    auto FB = 0.f;

    if (op == blend_operation::over) {
      FA = 1.f;      // FA=1
      FB = 1.f - Aa; // FB=1-Aa
    }
    if (op == blend_operation::in) {
      FA = Ba; // FA=Ba
      FB = 0;  // FB=0
    }
    if (op == blend_operation::out) {
      FA = 1.f - Ba; // FA=1-Ba
      FB = 0;        // FB=0
    }
    if (op == blend_operation::add) {
      FA = 1.f; // FA=1
      FB = 1.f; // FB=1
    }

    // aO = aA*FA + aB*FB
    // CO = (aA*FA*CA + aB*FB*CB) / ao;
    auto aAFA = Aa * FA;
    auto aBFB = Ba * FB;
    auto Oa   = aAFA + aBFB;
    auto Or   = Oa != 0 ? (aAFA * Ar + aBFB * Br) / Oa : 0;
    auto Og   = Oa != 0 ? (aAFA * Ag + aBFB * Bg) / Oa : 0;
    auto Ob   = Oa != 0 ? (aAFA * Ab + aBFB * Bb) / Oa : 0;

    PixelT ret;

    // clang-format off
    ret[0] = static_cast<value_type>(0.5f + std::clamp(std::pow(Or, 1.f / gamma), 0.f, scale));
    ret[1] = static_cast<value_type>(0.5f + std::clamp(std::pow(Og, 1.f / gamma), 0.f, scale));
    ret[2] = static_cast<value_type>(0.5f + std::clamp(std::pow(Ob, 1.f / gamma), 0.f, scale));
    ret[3] = static_cast<value_type>(0.5f + std::clamp(Oa * scale, 0.f, scale)); // scale back alpha
    // clang-format on

    return ret;
  }

  template <class PixelT>
  PixelT
    alpha_blend_PixelF(const PixelT& A, const PixelT& B, blend_operation op)
  {
    using value_type = typename PixelT::value_type;
    static_assert(std::is_floating_point_v<value_type>);
    static_assert(PixelT::pixel_format == sln::PixelFormat::RGBA);

    // use gamma=2.2 for correction
    const auto gamma = value_type(2.2);
    auto Ar          = std::pow(A[0], gamma);
    auto Ag          = std::pow(A[1], gamma);
    auto Ab          = std::pow(A[2], gamma);
    auto Br          = std::pow(B[0], gamma);
    auto Bg          = std::pow(B[1], gamma);
    auto Bb          = std::pow(B[2], gamma);

    const auto Aa = A[3]; // Alpha of A [0,1]
    const auto Ba = B[3]; // Alpha of B [0,1]

    if (op == blend_operation::src) {
      return A;
    }
    if (op == blend_operation::dst) {
      return B;
    }

    value_type FA {};
    value_type FB {};

    if (op == blend_operation::over) {
      FA = value_type(1);      // FA=1
      FB = value_type(1) - Aa; // FB=1-Aa
    }
    if (op == blend_operation::in) {
      FA = Ba;            // FA=Ba
      FB = value_type(0); // FB=0
    }
    if (op == blend_operation::out) {
      FA = value_type(1) - Ba; // FA=1-Ba
      FB = value_type(0);      // FB=0
    }
    if (op == blend_operation::add) {
      FA = value_type(1); // FA=1
      FB = value_type(1); // FB=1
    }

    // aO = aA*FA + aB*FB
    // CO = (aA*FA*CA + aB*FB*CB) / ao;
    auto aAFA = Aa * FA;
    auto aBFB = Ba * FB;
    auto Oa   = aAFA + aBFB;
    auto Or   = Oa != 0 ? (aAFA * Ar + aBFB * Br) / Oa : 0;
    auto Og   = Oa != 0 ? (aAFA * Ag + aBFB * Bg) / Oa : 0;
    auto Ob   = Oa != 0 ? (aAFA * Ab + aBFB * Bb) / Oa : 0;

    PixelT ret;

    ret[0] = std::clamp(std::pow(Or, 1 / gamma), value_type(0), value_type(1));
    ret[1] = std::clamp(std::pow(Og, 1 / gamma), value_type(0), value_type(1));
    ret[2] = std::clamp(std::pow(Ob, 1 / gamma), value_type(0), value_type(1));
    ret[3] = std::clamp(Oa, value_type(0), value_type(1));

    return ret;
  }

  void alpha_blend_RGBA8UI(
    const const_image_view& src,
    const mutable_image_view& dst,
    blend_operation op)
  {
    if (dst.width() != src.width() || dst.height() != src.height())
      throw std::runtime_error(
        "Alpha Composition: src/dst should have same image size");

    auto src_view =
      ::sln::to_image_view<sln::PixelRGBA_8u>(sln::to_DynImageView(src));
    auto dst_view =
      ::sln::to_image_view<sln::PixelRGBA_8u>(sln::to_DynImageView(dst));

    for (sln::PixelIndex j = {}; j < dst_view.height(); ++j) {
      for (sln::PixelIndex i = {}; i < dst_view.width(); ++i) {
        auto& A = *src_view.data(i, j);
        auto& B = *dst_view.data(i, j);
        B       = alpha_blend_PixelUI(A, B, op);
      }
    }
  }

  void alpha_blend_RGBA16UI(
    const const_image_view& src,
    const mutable_image_view& dst,
    blend_operation op)
  {
    if (dst.width() != src.width() || dst.height() != src.height())
      throw std::runtime_error(
        "Alpha Composition: src/dst should have same image size");

    auto src_view =
      sln::to_image_view<sln::PixelRGBA_16u>(sln::to_DynImageView(src));
    auto dst_view =
      sln::to_image_view<sln::PixelRGBA_16u>(sln::to_DynImageView(dst));

    for (sln::PixelIndex j = {}; j < dst_view.height(); ++j) {
      for (sln::PixelIndex i = {}; i < dst_view.width(); ++i) {
        auto& A = *src_view.data(i, j);
        auto& B = *dst_view.data(i, j);
        B       = alpha_blend_PixelUI(A, B, op);
      }
    }
  }

  void alpha_blend_RGBA32F(
    const const_image_view& src,
    const mutable_image_view& dst,
    blend_operation op)
  {
    if (dst.width() != src.width() || dst.height() != src.height())
      throw std::runtime_error(
        "Alpha Composition: src/dst should have same image size");

    auto src_view =
      sln::to_image_view<sln::PixelRGBA_32f>(sln::to_DynImageView(src));
    auto dst_view =
      sln::to_image_view<sln::PixelRGBA_32f>(sln::to_DynImageView(dst));

    for (sln::PixelIndex j = {}; j < dst_view.height(); ++j) {
      for (sln::PixelIndex i = {}; i < dst_view.width(); ++i) {
        auto& A = *src_view.data(i, j);
        auto& B = *dst_view.data(i, j);
        B       = alpha_blend_PixelF(A, B, op);
      }
    }
  }

} // namespace yave