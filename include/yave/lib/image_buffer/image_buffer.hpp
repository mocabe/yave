//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/image/image_view.hpp>
#include <yave/obj/image_buffer/image_buffer_pool.hpp>
#include <yave/support/id.hpp>

namespace yave {

  /// Image buffer object value
  struct image_buffer
  {
    /// Initialize new buffer
    image_buffer(
      object_ptr<const ImageBufferPool> pool,
      uint32_t width,
      uint32_t height,
      image_format format)
      : m_pool {std::move(pool)}
      , m_id {m_pool->create(width, height, format)}
    {
      if (m_id == 0)
        throw std::runtime_error("Failed to create image");
    }

    /// deleted
    image_buffer() = delete;
    /// deleted
    image_buffer(image_buffer&&) = delete;

    /// Copy Ctor.
    image_buffer(const image_buffer& other)
      : m_pool {other.m_pool}
      , m_id {other.m_pool->create_from(other.m_id)}
    {
      if (m_id == 0)
        throw std::runtime_error("Failed to create image");
    }

    /// Dtor.
    /// Decrements refcount.
    ~image_buffer() noexcept
    {
      m_pool->unref(m_id);
    }

    /// Get current use count
    [[nodiscard]] auto use_count() const -> uint64_t
    {
      auto ret = m_pool->use_count(m_id);

      if (ret == 0)
        throw std::runtime_error("Failed to get use cound of image");

      return ret;
    }

    /// Get ID of buffer
    [[nodiscard]] auto id() const -> uid
    {
      return {m_id};
    }

    /// Get buffer pool object
    [[nodiscard]] auto pool() const -> object_ptr<const ImageBufferPool>
    {
      return m_pool;
    }

    /// Get alter(native) handle of the image
    [[nodiscard]] auto native_handle() const -> uint64_t
    {
      return m_pool->native_handle(m_id);
    }

    [[nodiscard]] auto mutable_view() -> mutable_image_view
    {
      auto d = m_pool->data(m_id);
      auto w = m_pool->width(m_id);
      auto h = m_pool->height(m_id);
      auto f = m_pool->format(m_id);

      if (d == nullptr || f == image_format::unknown)
        throw std::runtime_error("Failed to get image view");

      return mutable_image_view(d, w, h, f);
    }

    /// Get image view.
    [[nodiscard]] auto const_view() const -> const_image_view
    {
      auto d = m_pool->data(m_id);
      auto w = m_pool->width(m_id);
      auto h = m_pool->height(m_id);
      auto f = m_pool->format(m_id);

      if (d == nullptr || f == image_format::unknown)
        throw std::runtime_error("Failed to get image view");

      return const_image_view(d, w, h, f);
    }

    /// Get image view.
    [[nodiscard]] auto view() -> mutable_image_view
    {
      return mutable_view();
    }

    /// Get image view.
    [[nodiscard]] auto view() const -> const_image_view
    {
      return const_view();
    }

  private:
    const object_ptr<const ImageBufferPool> m_pool;
    const uint64_t m_id;
  };
} // namespace yave