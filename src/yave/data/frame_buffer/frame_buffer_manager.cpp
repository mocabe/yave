//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/data/frame_buffer/frame_buffer_manager.hpp>
#include <yave/obj/frame_buffer/frame_buffer_pool.hpp>
#include <yave/lib/vulkan/offscreen_context.hpp>
#include <yave/core/uid.hpp>
#include <yave/core/log.hpp>

#include <boost/gil.hpp>
#include <map>

YAVE_DECL_LOCAL_LOGGER(frame_buffer_manager);

namespace yave::data {

  namespace {

    // shared frame data table
    struct frame_data
    {
      // vulkan texture
      vulkan::texture_data texture;
      // dirty?
      bool is_dirty;

      frame_data(vulkan::texture_data&& tex)
        : texture {std::move(tex)}
        , is_dirty {false}
      {
      }
    };

    // frame entry
    struct frame_entry
    {
      // ref of table holding actual data
      std::shared_ptr<frame_data> data;

      frame_entry(std::shared_ptr<frame_data> data)
        : data {std::move(data)}
      {
      }
    };

    // create new id
    auto get_new_id()
    {
      return uid::random_generate();
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

  public:
    // memory pool
    std::pmr::unsynchronized_pool_resource memory_resource;
    // entry map
    std::pmr::map<uid, frame_entry> map;
    // cached empty frame
    uid empty_frame;

  public:
    // extent
    uint32_t fb_width, fb_height;
    // format
    image_format fb_format;

  public:
    // pool
    object_ptr<const FrameBufferPool> pool_object;

  private:
    // find entry from id
    auto find_entry(uid id) -> frame_entry*
    {
      if (auto it = map.find(id); it != map.end())
        return &it->second;
      return nullptr;
    }

    // create empty frame
    uid create_empty() noexcept
    {
      try {

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

        auto data = std::make_shared<frame_data>(std::move(tex));

        auto id = get_new_id();

        auto [it, succ] = map.emplace(
          std::piecewise_construct,
          std::forward_as_tuple(id),
          std::forward_as_tuple(std::move(data)));

        if (!succ) {
          log_error( "Failed to insert new frame buffer id");
          return uid();
        }

        return id;

      } catch (...) {
        log_error( "Failed to create empty frame by exception");
        return uid();
      }
    }

    // COW: make entry writable
    void make_entry_writable(frame_entry* entry)
    {
      assert(entry);

      if (entry->data.use_count() == 1)
        return;

      auto tex = vulkan::clone_texture_data(
        entry->data->texture,
        offscreen_ctx.graphics_queue(),
        command_pool.get(),
        offscreen_ctx.device(),
        offscreen_ctx.vulkan_ctx().physical_device());

      entry->data = std::make_shared<frame_data>(std::move(tex));
    }

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
        [](void* handle, uint64_t id) noexcept -> void     { return ((impl*)handle)->destroy({id}); },
        [](void* handle, uint64_t id, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const uint8_t* d) noexcept -> void { return ((impl*)handle)->store_data({id}, x, y, w, h, d); },
        [](void* handle, uint64_t id, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* d)       noexcept -> void { return ((impl*)handle)->read_data({id}, x, y, w, h, d); },
        [](void* handle)              noexcept -> uint32_t     { return ((impl*)handle)->width(); },
        [](void* handle)              noexcept -> uint32_t     { return ((impl*)handle)->height(); },
        [](void* handle)              noexcept -> image_format { return ((impl*)handle)->format(); });
      // clang-format on
    }

    ~impl() noexcept
    {
      destroy(empty_frame);
    }

    uid create() noexcept
    {
      try {
        // cache empty frame
        if (empty_frame == uid())
          empty_frame = create_empty();

        return create_from(empty_frame);

      } catch (...) {
        log_error( "Failed to create new frame by exception");
        return uid();
      }
    }

    uid create_from(uid id) noexcept
    {
      try {
        auto entry = find_entry(id);

        if (!entry) {
          log_error( "Invalid frame id");
          return uid();
        }

        auto new_id = get_new_id();

        auto [it, succ] = map.emplace(
          std::piecewise_construct,
          std::forward_as_tuple(new_id),
          std::forward_as_tuple(entry->data));

        if (!succ) {
          log_error( "Failed to insert new frame buffer id");
          return uid();
        }

        return new_id;
      } catch (...) {
        log_info( "Failed to clone frame by exception");
        return uid();
      }
    }

    void destroy(uid id) noexcept
    {
      if (auto it = map.find(id); it != map.end())
        map.erase(it);
    }

    bool exists(uid id) noexcept
    {
      try {
        return find_entry(id);
      } catch (...) {
        return false;
      }
    }

    void store_data(
      uid id,
      uint32_t offset_x,
      uint32_t offset_y,
      uint32_t width,
      uint32_t height,
      const uint8_t* data) noexcept
    {
      try {

        auto entry = find_entry(id);

        if (!entry) {
          log_error( "Invalid frame id");
          return;
        }

        make_entry_writable(entry);

        vulkan::store_texture_data(
          staging,
          entry->data->texture,
          vk::Offset2D(offset_x, offset_y),
          vk::Extent2D(width, height),
          data,
          offscreen_ctx.graphics_queue(),
          command_pool.get(),
          offscreen_ctx.device(),
          offscreen_ctx.vulkan_ctx().physical_device());

        // mark as dirty
        entry->data->is_dirty = true;

      } catch (...) {
      }
    }

    void read_data(
      uid id,
      uint32_t offset_x,
      uint32_t offset_y,
      uint32_t width,
      uint32_t height,
      uint8_t* data) noexcept
    {
      try {

        auto entry = find_entry(id);

        if (!entry) {
          log_error( "Invalid frame id");
          return;
        }

        vulkan::load_texture_data(
          staging,
          entry->data->texture,
          vk::Offset2D(offset_x, offset_y),
          vk::Extent2D(width, height),
          data,
          offscreen_ctx.graphics_queue(),
          command_pool.get(),
          offscreen_ctx.device(),
          offscreen_ctx.vulkan_ctx().physical_device());

      } catch (...) {
      }
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
      auto entry = find_entry(id);
      make_entry_writable(entry);
      return entry->data->texture;
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

  void frame_buffer_manager::destroy(uid id) noexcept
  {
    return m_pimpl->destroy(id);
  }

  bool frame_buffer_manager::exists(uid id) const noexcept
  {
    return m_pimpl->exists(id);
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
} // namespace yave::data