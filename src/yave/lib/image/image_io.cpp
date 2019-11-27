//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/image/image_io.hpp>
#include <yave/lib/selene/conversion.hpp>
#include <yave/support/log.hpp>

#define SELENE_WITH_LIBTIFF
#include <selene/img_io/png/Read.hpp>
#include <selene/img_io/jpeg/Read.hpp>
#include <selene/img_io/tiff/Read.hpp>

#include <boost/dll/runtime_symbol_info.hpp>

YAVE_DECL_G_LOGGER(image_io);

namespace yave {

  namespace {

    auto normalize_path(const std::filesystem::path& p)
    {
      if (p.is_absolute())
        return p;

      // convert relative path to full path
      auto base = boost::dll::program_location().remove_filename();
      return std::filesystem::path(base.string()) / p;
    }

    void log_sln_logs(const sln::MessageLog& log)
    {
      for (auto&& msg : log.messages()) {
        switch (msg.type) {
          case sln::MessageType::Warning:
            Warning(g_logger, "{}", msg.text);
            break;
          case sln::MessageType::Error:
            Error(g_logger, "{}", msg.text);
            break;
          default:
            Info(g_logger, "{}", msg.text);
        }
      }
    }

    image load_image_png_impl(const std::filesystem::path& file_path)
    {
      assert(file_path.is_absolute());
      assert(std::filesystem::exists(file_path));

      Info(g_logger, "Loading PNG image from {}", file_path.string());

      auto reader = sln::FileReader(file_path.string());

      assert(reader.is_open());

      sln::MessageLog log;
      auto image = sln::read_png(reader, sln::PNGDecompressionOptions(), &log);

      log_sln_logs(log);

      if (image.is_empty()) {
        Error(g_logger, "Failed to load PNG image from {}", file_path.string());
        throw std::runtime_error("Failed to read PNG image");
      }

      auto ret = sln::to_image(std::move(image));

      Info(
        g_logger,
        "Loaded image: width={}, height={}, format={}",
        ret.width(),
        ret.height(),
        to_string(ret.image_format()));

      return ret;
    }

    image load_image_jpeg_impl(const std::filesystem::path& file_path)
    {
      assert(file_path.is_absolute());
      assert(std::filesystem::exists(file_path));

      Info(g_logger, "Loading JPEG image from {}", file_path.string());

      auto reader = sln::FileReader(file_path.string());

      assert(reader.is_open());

      // read JPEG file
      sln::MessageLog log;
      auto image =
        sln::read_jpeg(reader, sln::JPEGDecompressionOptions(), &log);

      log_sln_logs(log);

      if (image.is_empty()) {
        Error(
          g_logger, "Failed to read jpeg image from {}", file_path.string());
        throw std::runtime_error("Failed to read jpeg image");
      }

      auto ret = sln::to_image(std::move(image));

      Info(
        g_logger,
        "Loaded image: width={}, height={}, format={}",
        ret.width(),
        ret.height(),
        to_string(ret.image_format()));

      return ret;
    }

    image load_image_tiff_impl(const std::filesystem::path& file_path)
    {
      assert(file_path.is_absolute());
      assert(std::filesystem::exists(file_path));

      Info(g_logger, "Loading TIFF image from {}", file_path.string());

      auto reader = sln::FileReader(file_path.string());

      assert(reader.is_open());

      // read TIFF file
      sln::MessageLog log;
      auto image = sln::read_tiff(reader, &log);

      log_sln_logs(log);

      if (image.is_empty()) {
        Error(
          g_logger, "Failed to read tiff image from {}", file_path.string());
        throw std::runtime_error("Failed to read tiff image");
      }

      auto ret = sln::to_image(std::move(image));

      Info(
        g_logger,
        "Loaded image: width={}, height={}, format={}",
        ret.width(),
        ret.height(),
        to_string(ret.image_format()));

      return ret;
    }

  } // namespace

  image load_image_auto(const filesystem::path& file)
  {
    init_logger();

    auto file_path = normalize_path(file);

    if (!std::filesystem::exists(file_path)) {
      Error(
        g_logger, "Failed to load image: no such file {}", file_path.string());
      throw std::invalid_argument(
        "Failed to loag image file: File doesn't exist");
    }

    /* switch by extension */

    auto ext = file_path.extension().generic_u8string();

    // png
    if (ext == u8".png") {
      return load_image_png_impl(file_path);
    }

    // jpeg
    if (ext == u8".jpeg" || ext == u8".jpg") {
      return load_image_jpeg_impl(file_path);
    }

    // tiff
    if (ext == u8".tiff") {
      return load_image_tiff_impl(file_path);
    }

    throw std::runtime_error("Failed to read image file: Unsupported format");
  }

  image load_image_png(const filesystem::path& file)
  {
    init_logger();

    auto file_path = normalize_path(file);

    if (!std::filesystem::exists(file_path)) {
      Error(
        g_logger, "Failed to load image: no such file {}", file_path.string());
      throw std::invalid_argument(
        "Failed to loag image file: File doesn't exist");
    }

    return load_image_png_impl(file_path);
  }

  image load_image_jpeg(const filesystem::path& file)
  {
    init_logger();

    auto file_path = normalize_path(file);

    if (!std::filesystem::exists(file_path)) {
      Error(
        g_logger, "Failed to load image: no such file {}", file_path.string());
      throw std::invalid_argument(
        "Failed to loag image file: File doesn't exist");
    }

    return load_image_jpeg_impl(file_path);
  }

  image load_image_tiff(const filesystem::path& file)
  {
    init_logger();

    auto file_path = normalize_path(file);

    if (!std::filesystem::exists(file_path)) {
      Error(
        g_logger, "Failed to load image: no such file {}", file_path.string());
      throw std::invalid_argument(
        "Failed to loag image file: File doesn't exist");
    }

    return load_image_tiff_impl(file_path);
  }
} // namespace yave