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

namespace yave {

  /// Convert pixel_format to sln::PixelFormat
  [[nodiscard]] inline sln::PixelFormat to_sln(pixel_format fmt)
  {
    using namespace sln;

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
  [[nodiscard]] inline pixel_format from_sln(sln::PixelFormat fmt)
  {
    using namespace sln;

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
  [[nodiscard]] inline sln::SampleFormat to_sln(const sample_format& fmt)
  {
    using namespace sln;

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
  [[nodiscard]] inline sample_format from_sln(sln::SampleFormat fmt)
  {
    using namespace sln;

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
  [[nodiscard]] inline sln::DynImage<> to_sln(const image& image)
  {
    using namespace sln;

    auto layout = UntypedLayout(
      PixelLength(image.width()),
      PixelLength(image.height()),
      static_cast<int16_t>(image.channels()),
      static_cast<int16_t>(image.byte_per_channel()));

    auto semantics = UntypedImageSemantics(
      to_sln(image.image_format().pixel_format),
      to_sln(image.image_format().sample_format));

    sln::DynImage<> ret(layout, semantics);

    assert(ret.total_bytes() == image.byte_size());

    std::memcpy(ret.byte_ptr(), image.data(), ret.total_bytes());

    return ret;
  }

  /// Convert sln::DynImage to image
  [[nodiscard]] inline image from_sln(
    const sln::DynImage<>& image,
    const std::pmr::polymorphic_allocator<std::byte>& alloc)
  {
    auto pfmt   = from_sln(image.pixel_format());
    auto sfmt   = from_sln(image.sample_format());
    auto bpc    = image.nr_bytes_per_channel();
    auto width  = image.width().value();
    auto height = image.height().value();
    auto data   = image.byte_ptr();

    auto ret = yave::image(
      (std::byte*)data,
      width,
      height,
      image_format {pfmt, sfmt, static_cast<uint16_t>(bpc)},
      alloc);

    assert(ret.byte_size() == image.total_bytes());

    std::memcpy(ret.data(), data, ret.byte_size());

    return ret;
  }

  /// Convert const_image_view to sln::ConstantDynImageView
  [[nodiscard]] inline sln::ConstantDynImageView to_sln(
    const const_image_view& image)
  {
    using namespace sln;

    auto layout = UntypedLayout(
      PixelLength(image.width()),
      PixelLength(image.height()),
      static_cast<int16_t>(image.channels()),
      static_cast<int16_t>(image.byte_per_channel()));

    auto semantics = UntypedImageSemantics(
      to_sln(image.image_format().pixel_format),
      to_sln(image.image_format().sample_format));

    return ConstantDynImageView(
      (const uint8_t*)image.data(), layout, semantics);
  }

  /// Convert mutable_image_view to sln::MutableDynImageView
  [[nodiscard]] inline sln::MutableDynImageView to_sln(
    const mutable_image_view& image)
  {
    using namespace sln;

    auto layout = UntypedLayout(
      PixelLength(image.width()),
      PixelLength(image.height()),
      static_cast<int16_t>(image.channels()),
      static_cast<int16_t>(image.byte_per_channel()));

    auto semantics = UntypedImageSemantics(
      to_sln(image.image_format().pixel_format),
      to_sln(image.image_format().sample_format));

    return MutableDynImageView((uint8_t*)image.data(), layout, semantics);
  }

  /// Convert sln::ConstantDynImageView to const_image_view
  [[nodiscard]] inline const_image_view to_image_view(
    const sln::ConstantDynImageView& image)
  {
    auto fmt =
      image_format {from_sln(image.pixel_format()),
                    from_sln(image.sample_format()),
                    static_cast<uint16_t>(image.nr_bytes_per_channel())};

    return const_image_view(
      (const std::byte*)image.byte_ptr(),
      image.width().value(),
      image.height().value(),
      fmt);
  }

  /// Convert sln::MutableDynImageView to mutable_image_view
  [[nodiscard]] inline mutable_image_view to_image_view(
    const sln::MutableDynImageView& image)
  {
    auto fmt =
      image_format {from_sln(image.pixel_format()),
                    from_sln(image.sample_format()),
                    static_cast<uint16_t>(image.nr_bytes_per_channel())};

    return mutable_image_view(
      (std::byte*)image.byte_ptr(),
      image.width().value(),
      image.height().value(),
      fmt);
  }

} // namespace yave