//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/lib/image.hpp>
#include <yave/data/lib/image_view.hpp>

#include <selene/img/dynamic/DynImageView.hpp>
#include <selene/img/typed/ImageView.hpp>

namespace yave {

  /// Convert pixel_format to sln::PixelFormat
  sln::PixelFormat to_selene(yave::pixel_format fmt)
  {
    switch (fmt) {
      case pixel_format::Y:
        return sln::PixelFormat::Y;
      case pixel_format::YA:
        return sln::PixelFormat::YA;
      case pixel_format::RGB:
        return sln::PixelFormat::RGB;
      case pixel_format::YCbCr:
        return sln::PixelFormat::YCbCr;
      case pixel_format::CIELab:
        return sln::PixelFormat::CIELab;
      case pixel_format::ICCLab:
        return sln::PixelFormat::ICCLab;
      case pixel_format::RGBA:
        return sln::PixelFormat::RGBA;
      case pixel_format::CMYK:
        return sln::PixelFormat::CMYK;
      case pixel_format::YCCK:
        return sln::PixelFormat::YCCK;
    }
    return sln::PixelFormat::Unknown;
  }

  /// Convert sln::PixelFormat to pixel_format
  yave::pixel_format from_selene(sln::PixelFormat fmt)
  {
    switch (fmt) {
      case sln::PixelFormat::Y:
        return pixel_format::Y;
      case sln::PixelFormat::YA:
        return pixel_format::YA;
      case sln::PixelFormat::RGB:
        return pixel_format::RGB;
      case sln::PixelFormat::YCbCr:
        return pixel_format::YCbCr;
      case sln::PixelFormat::CIELab:
        return pixel_format::CIELab;
      case sln::PixelFormat::ICCLab:
        return pixel_format::ICCLab;
      case sln::PixelFormat::RGBA:
        return pixel_format::RGBA;
      case sln::PixelFormat::CMYK:
        return pixel_format::CMYK;
      case sln::PixelFormat::YCCK:
        return pixel_format::YCCK;
    }
    return pixel_format::Unknown;
  }

  /// Convert sample_format to sln::SampleFormat
  sln::SampleFormat to_selene(const yave::sample_format& fmt)
  {
    switch (fmt) {
      case sample_format::UnsignedInteger:
      case sample_format::UnsignedNormalized:
      case sample_format::UnsignedScaled:
      case sample_format::SRGB:
        return sln::SampleFormat::UnsignedInteger;
      case sample_format::SignedInteger:
      case sample_format::SignedNormalized:
      case sample_format::SignedScaled:
        return sln::SampleFormat::SignedInteger;
      case sample_format::FloatingPoint:
        return sln::SampleFormat::FloatingPoint;
    }
    return sln::SampleFormat::Unknown;
  }

  /// Convert sln::SampleFormat to sample_format
  yave::sample_format from_selene(sln::SampleFormat fmt)
  {
    switch (fmt) {
      case sln::SampleFormat::UnsignedInteger:
        return sample_format::UnsignedInteger;
      case sln::SampleFormat::SignedInteger:
        return sample_format::SignedInteger;
      case sln::SampleFormat::FloatingPoint:
        return sample_format::FloatingPoint;
    }
    return sample_format::Unknown;
  }

  /// Convert const_image_view to sln::ConstantDynImageView
  sln::ConstantDynImageView to_selene(const yave::const_image_view& image)
  {
    auto layout = sln::UntypedLayout(
      sln::PixelLength(image.width()),
      sln::PixelLength(image.height()),
      static_cast<int16_t>(image.channels()),
      static_cast<int16_t>(image.byte_per_channel()));

    auto semantics = sln::UntypedImageSemantics(
      to_selene(image.image_format().pixel_format),
      to_selene(image.image_format().sample_format));

    return sln::ConstantDynImageView(image.data(), layout, semantics);
  }

  /// Convert mutable_image_view to sln::MutableDynImageView
  sln::MutableDynImageView to_selene(const yave::mutable_image_view& image)
  {
    auto layout = sln::UntypedLayout(
      sln::PixelLength(image.width()),
      sln::PixelLength(image.height()),
      static_cast<int16_t>(image.channels()),
      static_cast<int16_t>(image.byte_per_channel()));

    auto semantics = sln::UntypedImageSemantics(
      to_selene(image.image_format().pixel_format),
      to_selene(image.image_format().sample_format));

    return sln::MutableDynImageView(image.data(), layout, semantics);
  }

  /// Convert sln::ConstantDynImageView to const_image_view
  yave::const_image_view from_selene(const sln::ConstantDynImageView& image)
  {
    auto fmt = image_format {from_selene(image.pixel_format()),
                             from_selene(image.sample_format()),
                             image.nr_bytes_per_channel()};

    return yave::const_image_view(
      image.byte_ptr(), image.width().value(), image.height().value(), fmt);
  }

  /// Convert sln::MutableDynImageView to mutable_image_view
  yave::mutable_image_view from_selene(const sln::MutableDynImageView& image)
  {
    auto fmt = image_format {from_selene(image.pixel_format()),
                             from_selene(image.sample_format()),
                             image.nr_bytes_per_channel()};

    return yave::mutable_image_view(
      image.byte_ptr(), image.width().value(), image.height().value(), fmt);
  }
}