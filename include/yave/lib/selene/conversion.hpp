//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/image/image.hpp>
#include <yave/lib/image/image_view.hpp>

#include <selene/img/dynamic/DynImageView.hpp>
#include <selene/img/typed/ImageView.hpp>
#include <selene/img/interop/DynImageToImage.hpp>

namespace yave::sln {

  using namespace ::sln; // silly, but works.

  /// Convert pixel_format to sln::PixelFormat
  [[nodiscard]] inline PixelFormat to_PixelFormat(pixel_format fmt)
  {
    switch (fmt) {
      case pixel_format::Y:
        return PixelFormat::Y;
      case pixel_format::YA:
        return PixelFormat::YA;
      case pixel_format::RGB:
        return PixelFormat::RGB;
      case pixel_format::YCbCr:
        return PixelFormat::YCbCr;
      case pixel_format::CIELab:
        return PixelFormat::CIELab;
      case pixel_format::ICCLab:
        return PixelFormat::ICCLab;
      case pixel_format::RGBA:
        return PixelFormat::RGBA;
      case pixel_format::CMYK:
        return PixelFormat::CMYK;
      case pixel_format::YCCK:
        return PixelFormat::YCCK;
      case pixel_format::Unknown:
        return PixelFormat::Unknown;
    }
    return PixelFormat::Unknown;
  }

  /// Convert sln::PixelFormat to pixel_format
  [[nodiscard]] inline pixel_format to_pixel_format(PixelFormat fmt)
  {
    switch (fmt) {
      case PixelFormat::Y:
        return pixel_format::Y;
      case PixelFormat::YA:
        return pixel_format::YA;
      case PixelFormat::RGB:
        return pixel_format::RGB;
      case PixelFormat::YCbCr:
        return pixel_format::YCbCr;
      case PixelFormat::CIELab:
        return pixel_format::CIELab;
      case PixelFormat::ICCLab:
        return pixel_format::ICCLab;
      case PixelFormat::RGBA:
        return pixel_format::RGBA;
      case PixelFormat::CMYK:
        return pixel_format::CMYK;
      case PixelFormat::YCCK:
        return pixel_format::YCCK;
      // not supported
      case PixelFormat::X:
      case PixelFormat::XX:
      case PixelFormat::XXX:
      case PixelFormat::XXXX:
      case PixelFormat::BGR:
      case PixelFormat::BGRA:
      case PixelFormat::ARGB:
      case PixelFormat::ABGR:
      case PixelFormat::Unknown:
      case PixelFormat::Invalid:
        return pixel_format::Unknown;
    }
    return pixel_format::Unknown;
  }

  /// Convert sample_format to sln::SampleFormat
  [[nodiscard]] inline SampleFormat to_SampleFormat(const sample_format& fmt)
  {
    switch (fmt) {
      case sample_format::UnsignedInteger:
      case sample_format::UnsignedNormalized:
      case sample_format::UnsignedScaled:
      case sample_format::SRGB:
        return SampleFormat::UnsignedInteger;
      case sample_format::SignedInteger:
      case sample_format::SignedNormalized:
      case sample_format::SignedScaled:
        return SampleFormat::SignedInteger;
      case sample_format::FloatingPoint:
        return SampleFormat::FloatingPoint;
      case sample_format::Unknown:
        return SampleFormat::Unknown;
    }
    return SampleFormat::Unknown;
  }

  /// Convert sln::SampleFormat to sample_format
  [[nodiscard]] inline sample_format to_sample_format(SampleFormat fmt)
  {
    switch (fmt) {
      case SampleFormat::UnsignedInteger:
        return sample_format::UnsignedInteger;
      case SampleFormat::SignedInteger:
        return sample_format::SignedInteger;
      case SampleFormat::FloatingPoint:
        return sample_format::FloatingPoint;
      case SampleFormat::Unknown:
        return sample_format::Unknown;
    }
    return sample_format::Unknown;
  }

  /// Convert image to sln::DynImage
  [[nodiscard]] inline DynImage<> to_DynImage(image&& image)
  {
    auto layout = UntypedLayout(
      PixelLength(image.width()),
      PixelLength(image.height()),
      static_cast<int16_t>(image.channels()),
      static_cast<int16_t>(image.byte_per_channel()));

    auto semantics = UntypedImageSemantics(
      to_PixelFormat(image.image_format().pixel_format),
      to_SampleFormat(image.image_format().sample_format));

    return sln::DynImage<>(image.release(), layout, semantics);
  }

  /// Convert image to sln::DynImage
  [[nodiscard]] inline DynImage<> to_DynImage(const image& image)
  {
    auto tmp = image;
    return to_DynImage(std::move(tmp));
  }

  [[nodiscard]] inline image to_image(DynImage<>&& image)
  {
    auto pfmt = to_pixel_format(image.pixel_format());
    auto sfmt = to_sample_format(image.sample_format());
    auto bpc  = image.nr_bytes_per_channel();
    auto data = image.relinquish_data_ownership().transfer_data();

    return yave::image(
      data,
      image.width().value,
      image.height().value,
      image_format {pfmt, sfmt, bpc});
  }

  [[nodiscard]] inline image to_image(const DynImage<>& image)
  {
    auto tmp = image;
    return to_image(std::move(tmp));
  }

  /// Convert const_image_view to sln::ConstantDynImageView
  [[nodiscard]] inline ConstantDynImageView
    to_DynImageView(const const_image_view& image)
  {
    auto layout = UntypedLayout(
      PixelLength(image.width()),
      PixelLength(image.height()),
      static_cast<int16_t>(image.channels()),
      static_cast<int16_t>(image.byte_per_channel()));

    auto semantics = UntypedImageSemantics(
      to_PixelFormat(image.image_format().pixel_format),
      to_SampleFormat(image.image_format().sample_format));

    return ConstantDynImageView(image.data(), layout, semantics);
  }

  /// Convert mutable_image_view to sln::MutableDynImageView
  [[nodiscard]] inline MutableDynImageView
    to_DynImageView(const mutable_image_view& image)
  {
    auto layout = UntypedLayout(
      PixelLength(image.width()),
      PixelLength(image.height()),
      static_cast<int16_t>(image.channels()),
      static_cast<int16_t>(image.byte_per_channel()));

    auto semantics = UntypedImageSemantics(
      to_PixelFormat(image.image_format().pixel_format),
      to_SampleFormat(image.image_format().sample_format));

    return MutableDynImageView(image.data(), layout, semantics);
  }

  /// Convert sln::ConstantDynImageView to const_image_view
  [[nodiscard]] inline const_image_view
    to_image_view(const ConstantDynImageView& image)
  {
    auto fmt =
      image_format {to_pixel_format(image.pixel_format()),
                    to_sample_format(image.sample_format()),
                    static_cast<uint16_t>(image.nr_bytes_per_channel())};

    return const_image_view(
      image.byte_ptr(), image.width().value(), image.height().value(), fmt);
  }

  /// Convert sln::MutableDynImageView to mutable_image_view
  [[nodiscard]] inline mutable_image_view
    to_image_view(const MutableDynImageView& image)
  {
    auto fmt =
      image_format {to_pixel_format(image.pixel_format()),
                    to_sample_format(image.sample_format()),
                    static_cast<uint16_t>(image.nr_bytes_per_channel())};

    return mutable_image_view(
      image.byte_ptr(), image.width().value(), image.height().value(), fmt);
  }

  /// wrapper of ::sln::to_image_view<...>
  template <typename PixelType, typename Allocator>
  [[nodiscard]] MutableImageView<PixelType>
    to_image_view(DynImage<Allocator>& dyn_img)
  {
    return ::sln::to_image_view<PixelType, Allocator>(dyn_img);
  }

  /// wrapper of ::sln::to_image_view<...>
  template <typename PixelType, typename Allocator>
  [[nodiscard]] ConstantImageView<PixelType>
    to_image_view(const DynImage<Allocator>& dyn_img)
  {
    return ::sln::to_image_view<PixelType, Allocator>(dyn_img);
  }

  /// wrapper of ::sln::to_image_view<...>
  template <typename PixelType, ImageModifiability modifiability>
  [[nodiscard]] ImageView<PixelType, modifiability>
    to_image_view(const DynImageView<modifiability>& view)
  {
    return ::sln::to_image_view<PixelType, modifiability>(view);
  }

} // namespace yave