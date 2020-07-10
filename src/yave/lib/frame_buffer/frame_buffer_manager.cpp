//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/frame_buffer/frame_buffer_manager.hpp>
#include <yave/obj/frame_buffer/frame_buffer_pool.hpp>
#include <yave/lib/vulkan/offscreen_context.hpp>

#include <yave/support/id.hpp>
#include <yave/rts/atomic.hpp>

#include <boost/gil.hpp>
#include <map>
#include <shared_mutex>

namespace yave {

  namespace {

    // table
    struct frame_table
    {
      // refcount
      atomic_refcount<uint32_t> refcount;
      // id (internal)
      const uint32_t id;
      // vulkan texture
      vulkan::texture_data texture;

      frame_table(uint32_t id, vulkan::texture_data&& tex)
        : refcount {1}
        , id {id}
        , texture {std::move(tex)}
      {
      }
    };

    auto table_ptr_to_id(frame_table* ptr)
    {
      return uid {std::uintptr_t(ptr)};
    }

    auto id_to_table_ptr(uid id)
    {
      return (frame_table*)(std::uintptr_t(id.data));
    }

    auto gen_id()
    {
      static std::mt19937 rng(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
      return rng();
    }

    auto create_command_pool(uint32_t graphicsQueueIndex, vk::Device device)
    {
      vk::CommandPoolCreateInfo info;
      info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
      info.queueFamilyIndex = graphicsQueueIndex;
      return device.createCommandPoolUnique(info);
    }

  } // namespace

  class frame_buffer_manager::impl
  {
  public:
    // vulkan context
    vulkan::offscreen_context& offscreen_ctx;
    // vulkan command pool
    vk::UniqueCommandPool command_pool;
    // vulkan image format
    vk::Format vk_format;
    // vulkan staging buffer for data transfer
    vulkan::staging_buffer staging;
    // memory pool
    std::pmr::unsynchronized_pool_resource memory_resource;
    // table map
    std::pmr::map<uint32_t, frame_table> map;
    // extent
    uint32_t fb_width, fb_height;
    // format
    image_format fb_format;
    // pool
    object_ptr<const FrameBufferPool> pool_object;

  public:
    impl(
      uint32_t width,
      uint32_t height,
      image_format format,
      uuid backend_id,
      vulkan::offscreen_context& ctx)
      : offscreen_ctx {ctx}
      , memory_resource {}
      , map {&memory_resource}
      , fb_width {width}
      , fb_height {height}
      , fb_format {format}
    {
      if (format != image_format::rgba32f)
        throw std::runtime_error("Unsupported image format for frame buffer");

      auto queue_idx      = ctx.graphics_queue_index();
      auto device         = ctx.device();
      auto physicalDevice = ctx.vulkan_ctx().physical_device();
      command_pool        = create_command_pool(queue_idx, device);
      vk_format           = vulkan::convert_to_format(format);
      staging = vulkan::create_staging_buffer(1, device, physicalDevice);

      // clang-format off
      pool_object = make_object<FrameBufferPool>(
        (void*)this,
        backend_id,
        [](void* handle)              noexcept -> uint64_t { return ((impl*)handle)->create().data; },
        [](void* handle, uint64_t id) noexcept -> uint64_t { return ((impl*)handle)->create_from({id}).data; },
        [](void* handle, uint64_t id) noexcept -> void     { return ((impl*)handle)->ref({id}); },
        [](void* handle, uint64_t id) noexcept -> void     { return ((impl*)handle)->unref({id}); },
        [](void* handle, uint64_t id) noexcept -> uint64_t { return ((impl*)handle)->use_count({id}); },
        [](void* handle, uint64_t id, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const uint8_t* d) noexcept -> void { return ((impl*)handle)->store_data({id}, x, y, w, h, d); },
        [](void* handle, uint64_t id, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* d)       noexcept -> void { return ((impl*)handle)->read_data({id}, x, y, w, h, d); },
        [](void* handle)              noexcept -> uint32_t     { return ((impl*)handle)->width(); },
        [](void* handle)              noexcept -> uint32_t     { return ((impl*)handle)->height(); },
        [](void* handle)              noexcept -> image_format { return ((impl*)handle)->format(); });
      // clang-format on
    }

    uid create() noexcept
    try {

      auto id  = gen_id();
      auto tex = vulkan::create_texture_data(
        fb_width,
        fb_height,
        vk_format,
        offscreen_ctx.graphics_queue(),
        command_pool.get(),
        offscreen_ctx.device(),
        offscreen_ctx.vulkan_ctx().physical_device());

      vulkan::clear_texture_data(
        tex,
        std::array {0.f, 0.f, 0.f, 0.f},
        offscreen_ctx.graphics_queue(),
        command_pool.get(),
        offscreen_ctx.device(),
        offscreen_ctx.vulkan_ctx().physical_device());

      auto [it, succ] = map.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(id),
        std::forward_as_tuple(id, std::move(tex)));

      if (!succ)
        return uid();

      return table_ptr_to_id(&it->second);

    } catch (...) {
      return uid();
    }

    uid create_from(uid id) noexcept
    try {

      auto ptr    = id_to_table_ptr(id);
      auto new_id = gen_id();

      auto tex = vulkan::clone_texture_data(
        ptr->texture,
        offscreen_ctx.graphics_queue(),
        command_pool.get(),
        offscreen_ctx.device(),
        offscreen_ctx.vulkan_ctx().physical_device());

      auto [it, succ] = map.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(new_id),
        std::forward_as_tuple(new_id, std::move(tex)));

      if (!succ)
        return uid();

      return table_ptr_to_id(&it->second);
    } catch (...) {
      return uid();
    }

    bool exists(uid id) noexcept
    try {
      for (auto&& p : map)
        if (table_ptr_to_id(&p.second) == id)
          return true;
      return false;
    } catch (...) {
      return false;
    }

    void ref(uid id) noexcept
    try {
      auto ptr = id_to_table_ptr(id);
      ptr->refcount.fetch_add();
    } catch (...) {
    }

    void unref(uid id) noexcept
    try {
      auto ptr = id_to_table_ptr(id);
      if (ptr->refcount.fetch_sub() == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);
        map.erase(ptr->id);
      }
    } catch (...) {
    }

    auto use_count(uid id) noexcept -> uint64_t
    try {
      auto ptr = id_to_table_ptr(id);
      return ptr->refcount.load();
    } catch (...) {
      return 0;
    }

    void store_data(
      uid id,
      uint32_t offset_x,
      uint32_t offset_y,
      uint32_t width,
      uint32_t height,
      const uint8_t* data) noexcept
    try {
      auto ptr = id_to_table_ptr(id);
      vulkan::store_texture_data(
        staging,
        ptr->texture,
        vk::Offset2D(offset_x, offset_y),
        vk::Extent2D(width, height),
        data,
        offscreen_ctx.graphics_queue(),
        command_pool.get(),
        offscreen_ctx.device(),
        offscreen_ctx.vulkan_ctx().physical_device());
    } catch (...) {
    }

    void read_data(
      uid id,
      uint32_t offset_x,
      uint32_t offset_y,
      uint32_t width,
      uint32_t height,
      uint8_t* data) noexcept
    try {
      auto ptr = id_to_table_ptr(id);
      vulkan::load_texture_data(
        staging,
        ptr->texture,
        vk::Offset2D(offset_x, offset_y),
        vk::Extent2D(width, height),
        data,
        offscreen_ctx.graphics_queue(),
        command_pool.get(),
        offscreen_ctx.device(),
        offscreen_ctx.vulkan_ctx().physical_device());
    } catch (...) {
    }

    auto width() noexcept -> uint32_t
    {
      return fb_width;
    }

    auto height() noexcept -> uint32_t
    {
      return fb_height;
    }

    auto format() noexcept -> image_format
    {
      return fb_format;
    }

    auto get_texture_data(uid id) -> vulkan::texture_data&
    {
      auto ptr = id_to_table_ptr(id);
      return ptr->texture;
    }
  };

  frame_buffer_manager::frame_buffer_manager(
    uint32_t width,
    uint32_t height,
    image_format format,
    uuid backend_id,
    vulkan::offscreen_context& ctx)
    : m_pimpl {std::make_unique<impl>(width, height, format, backend_id, ctx)}
  {
  }

  frame_buffer_manager::~frame_buffer_manager() noexcept
  {
  }

  frame_buffer_manager::frame_buffer_manager(
    frame_buffer_manager&& other) noexcept = default;

  frame_buffer_manager& frame_buffer_manager::operator=(
    frame_buffer_manager&& other) noexcept = default;

  uid frame_buffer_manager::create() noexcept
  {
    return m_pimpl->create();
  }

  uid frame_buffer_manager::create_from(uid id) noexcept
  {
    return m_pimpl->create_from(id);
  }

  bool frame_buffer_manager::exists(uid id) const noexcept
  {
    return m_pimpl->exists(id);
  }

  void frame_buffer_manager::ref(uid id) noexcept
  {
    m_pimpl->ref(id);
  }

  void frame_buffer_manager::unref(uid id) noexcept
  {
    m_pimpl->unref(id);
  }

  auto frame_buffer_manager::use_count(uid id) const noexcept -> uint64_t
  {
    return m_pimpl->use_count(id);
  }

  auto frame_buffer_manager::width() const noexcept -> uint32_t
  {
    return m_pimpl->width();
  }

  auto frame_buffer_manager::height() const noexcept -> uint32_t
  {
    return m_pimpl->height();
  }

  auto frame_buffer_manager::format() const noexcept -> image_format
  {
    return m_pimpl->format();
  }

  auto frame_buffer_manager::get_pool_object() const noexcept
    -> object_ptr<const FrameBufferPool>
  {
    return m_pimpl->pool_object;
  }

  auto frame_buffer_manager::get_texture_data(uid id) -> vulkan::texture_data&
  {
    return m_pimpl->get_texture_data(id);
  }
} // namespace yave