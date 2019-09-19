//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/obj/buffer/buffer.hpp>
#include <yave/obj/buffer/buffer_pool.hpp>

namespace yave {

  buffer::buffer(const object_ptr<BufferPool>& pool, uint64_t size)
    : m_pool {pool}
  {
    m_id = pool->create(size);

    if (m_id == uid())
      throw std::bad_alloc();
  }

  buffer::buffer(const object_ptr<BufferPool>& pool, uid id)
  {
    m_id = pool->create_from(id);

    if (m_id == uid())
      throw std::bad_alloc();
  }

  buffer::buffer(const buffer& other)
    : m_pool {other.m_pool}
    , m_id {other.m_id}
  {
    m_pool->ref(m_id);
  }

  buffer::~buffer() noexcept
  {
    if (m_pool)
      m_pool->unref(m_id);
  }

  auto buffer::copy() const -> object_ptr<Buffer>
  {
    return make_object<Buffer>(m_pool, m_pool->create_from(m_id));
  }

  auto buffer::data() -> uint8_t*
  {
    return m_pool->get_data(m_id);
  }

  auto buffer::data() const -> const uint8_t*
  {
    return m_pool->get_data(m_id);
  }

  auto buffer::size() const -> uint64_t
  {
    return m_pool->get_size(m_id);
  }

  auto buffer::use_count() const -> uint64_t
  {
    return m_pool->get_use_count(m_id);
  }

  auto buffer::id() const -> uid
  {
    return m_id;
  }

  auto buffer::pool() const -> object_ptr<BufferPool>
  {
    return m_pool;
  }
}