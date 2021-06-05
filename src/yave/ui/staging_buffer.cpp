//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/staging_buffer.hpp>

namespace {

  using namespace yave::ui;

  auto makeCreateInfo(vk::DeviceSize size)
  {
    return vk::BufferCreateInfo()
      .setUsage(
        vk::BufferUsageFlagBits::eTransferSrc | //
        vk::BufferUsageFlagBits::eTransferDst)
      .setSize(size)
      .setSharingMode(vk::SharingMode::eExclusive);
  }

  auto allocateBuffer(
    const vk::BufferCreateInfo& info,
    vulkan_allocator& allocator)
  {
    return allocator.create_buffer(info, VMA_MEMORY_USAGE_CPU_ONLY);
  }

  auto copyBuffer(
    vulkan_buffer& src,
    vulkan_buffer& dst,
    vk::DeviceSize size,
    vulkan_device& device)
  {
    auto stc = yave::vulkan::single_time_command(
      device.device(), device.graphics_queue(), device.graphics_command_pool());

    auto cmd = stc.command_buffer();
    {
      auto info = vk::BufferCopy().setSize(size);
      cmd.copyBuffer(src.buffer(), dst.buffer(), info);
    }
  }

} // namespace

namespace yave::ui {

  staging_buffer::staging_buffer(size_t size, vulkan_allocator& allocator)
    : m_allocator {allocator}
  {
    if (size != 0) {
      auto info     = makeCreateInfo(size);
      m_buffer      = allocateBuffer(info, m_allocator);
      m_create_info = info;
    }
    m_current_size = size;
  }

  void staging_buffer::resize(size_t new_size)
  {
    if (new_size <= m_current_size) {
      m_current_size = new_size;
      return;
    }

    auto info   = makeCreateInfo(new_size);
    auto buffer = allocateBuffer(info, m_allocator);

    if (!m_buffer.empty())
      copyBuffer(m_buffer, buffer, size(), m_allocator.device());

    m_buffer       = std::move(buffer);
    m_create_info  = info;
    m_current_size = new_size;
  }

  void staging_buffer::shrink_to_fit()
  {
    if (size() == capacity())
      return;

    if (size() == 0) {
      m_buffer       = {};
      m_create_info  = vk::BufferCreateInfo();
      m_current_size = 0;
      return;
    }

    auto info   = makeCreateInfo(size());
    auto buffer = allocateBuffer(info, m_allocator);

    copyBuffer(m_buffer, buffer, size(), m_allocator.device());

    m_buffer       = std::move(buffer);
    m_create_info  = info;
    m_current_size = info.size;
  }

} // namespace yave::ui