//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/render_buffer.hpp>

namespace {

  using namespace yave::ui;

  auto createBuffer(
    vk::DeviceSize size,
    vk::BufferUsageFlags usage,
    vulkan_allocator& allocator)
  {
    assert(
      usage == vk::BufferUsageFlagBits::eVertexBuffer || //
      usage == vk::BufferUsageFlagBits::eIndexBuffer);

    assert(size > 0);

    auto info = vk::BufferCreateInfo()
                  .setUsage(usage)
                  .setSharingMode(vk::SharingMode::eExclusive)
                  .setSize(size);

    return allocator.create_buffer(info, VMA_MEMORY_USAGE_CPU_TO_GPU);
  }

} // namespace

namespace yave::ui {

  render_buffer::render_buffer(
    vk::BufferUsageFlags usage,
    vulkan_allocator& allocator)
    : m_allocator {allocator}
    , m_buffer {createBuffer(1, usage, allocator)}
    , m_usage {usage}
    , m_size {0}
    , m_capacity {1}
  {
  }

  void render_buffer::resize(size_t new_size)
  {
    if (new_size <= m_capacity) {
      m_size = new_size;
      return;
    }

    auto new_capacity = std::min(size_t(1), new_size);

    m_buffer   = {}; // deallocate
    m_buffer   = createBuffer(new_capacity, m_usage, m_allocator);
    m_capacity = new_capacity;
    m_size     = new_size;
  }

  auto render_buffer::begin_write() -> u8*
  {
    return m_buffer.mapped_data();
  }

  void render_buffer::end_write()
  {
    m_buffer.flush(0, m_size);
  }

} // namespace yave::ui