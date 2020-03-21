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
  [[nodiscard]] inline sln::PixelFormat to_sln(image_color_type fmt)
  {
    using namespace sln;

    switch (fmt) {
      case image_color_type::y:
        return PixelFormat::Y;
      case image_color_type::ya:
        return PixelFormat::YA;
      case image_color_type::rgb:
        return PixelFormat::RGB;
      case image_color_type::ycbcr:
        return PixelFormat::YCbCr;
      case image_color_type::cielab:
        return PixelFormat::CIELab;
      case image_color_type::icclab:
        return PixelFormat::ICCLab;
      case image_color_type::rgba:
        return PixelFormat::RGBA;
      case image_color_type::cmyk:
        return PixelFormat::CMYK;
      case image_color_type::ycck:
        return PixelFormat::YCCK;
      case image_color_type::unknown:
        return PixelFormat::Unknown;
    }
    return PixelFormat::Unknown;
  }

  /// Convert sln::PixelFormat to image_color_type
  [[nodiscard]] inline image_color_type from_sln(sln::PixelFormat fmt)
  {
    using namespace sln;

    switch (fmt) {
      case PixelFormat::Y:
        return image_color_type::y;
      case PixelFormat::YA:
        return image_color_type::ya;
      case PixelFormat::RGB:
        return image_color_type::rgb;
      case PixelFormat::YCbCr:
        return image_color_type::ycbcr;
      case PixelFormat::CIELab:
        return image_color_type::cielab;
      case PixelFormat::ICCLab:
        return image_color_type::icclab;
      case PixelFormat::RGBA:
        return image_color_type::rgba;
      case PixelFormat::CMYK:
        return image_color_type::cmyk;
      case PixelFormat::YCCK:
        return image_color_type::ycck;
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
        return image_color_type::unknown;
    }
    return image_color_type::unknown;
  }

  /// Convert sample_format to sln::SampleFormat
  [[nodiscard]] inline sln::SampleFormat to_sln(const image_data_type& fmt)
  {
    using namespace sln;

    switch (fmt) {
      case image_data_type::unsigned_integer:
        return SampleFormat::UnsignedInteger;
      case image_data_type::signed_integer:
        return SampleFormat::SignedInteger;
      case image_data_type::floating_point:
        return SampleFormat::FloatingPoint;
      case image_data_type::unknown:
        return SampleFormat::Unknown;
    }
    return SampleFormat::Unknown;
  }

  /// Convert sln::SampleFormat to image_data_type
  [[nodiscard]] inline image_data_type from_sln(sln::SampleFormat fmt)
  {
    using namespace sln;

    switch (fmt) {
      case SampleFormat::UnsignedInteger:
        return image_data_type::unsigned_integer;
      case SampleFormat::SignedInteger:
        return image_data_type::signed_integer;
      case SampleFormat::FloatingPoint:
        return image_data_type::floating_point;
      case SampleFormat::Unknown:
        return image_data_type::unknown;
    }
    return image_data_type::unknown;
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
      to_sln(image.image_format().color_type),
      to_sln(image.image_format().data_type));

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
      to_sln(image.image_format().color_type),
      to_sln(image.image_format().data_type));

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
      to_sln(image.image_format().color_type),
      to_sln(image.image_format().data_type));

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