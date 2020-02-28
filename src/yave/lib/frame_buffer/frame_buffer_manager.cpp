//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/frame_buffer/frame_buffer_manager.hpp>
#include <yave/obj/frame_buffer/frame_buffer_pool.hpp>

#include <yave/support/id.hpp>
#include <yave/rts/atomic.hpp>

#include <boost/gil.hpp>
#include <map>

namespace yave {

  namespace {

    using namespace boost;
    using alloc_t       = std::pmr::polymorphic_allocator<std::byte>;
    using frame_image_t = gil::image<gil::rgba32f_pixel_t, false, alloc_t>;

    // table
    struct frame_table
    {
      // refcount
      atomic_refcount<uint32_t> refcount;
      // id (internal)
      const uint32_t id;
      // image representation
      frame_image_t image;

      frame_table(uint32_t id, frame_image_t&& img)
        : refcount {1}
        , id {id}
        , image {std::move(img)}
      {
      }

      frame_table(uint32_t id, const frame_table& other)
        : refcount {1}
        , id {id}
        , image {other.image}
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
  } // namespace

  class frame_buffer_manager::impl
  {
  public:
    // memory pool
    std::pmr::unsynchronized_pool_resource memory_resource;
    // table map
    std::pmr::map<uint32_t, frame_table> map;
    // extent
    uint32_t fb_width, fb_height;
    // format
    image_format fb_format;
    // lock
    std::shared_mutex mtx;
    // pool
    object_ptr<const FrameBufferPool> pool_object;

  public:
    impl(uint32_t width, uint32_t height, image_format format, uuid backend_id)
      : memory_resource {}
      , map {&memory_resource}
      , fb_width {width}
      , fb_height {height}
      , fb_format {format}
    {
      // clang-format off
      // create pool object
      pool_object = make_object<FrameBufferPool>(
        (void*)this,
        backend_id,
        [](void* handle)              noexcept -> uint64_t     { return ((impl*)handle)->create().data; },
        [](void* handle, uint64_t id) noexcept -> uint64_t     { return ((impl*)handle)->create_from({id}).data; },
        [](void* handle, uint64_t id) noexcept -> void         { return ((impl*)handle)->ref({id}); },
        [](void* handle, uint64_t id) noexcept -> void         { return ((impl*)handle)->unref({id}); },
        [](void* handle, uint64_t id) noexcept -> uint64_t     { return ((impl*)handle)->use_count({id}); },
        [](void* handle, uint64_t id) noexcept -> std::byte*   { return ((impl*)handle)->data({id}); },
        [](void* handle)              noexcept -> uint32_t     { return ((impl*)handle)->width(); },
        [](void* handle)              noexcept -> uint32_t     { return ((impl*)handle)->height(); },
        [](void* handle)              noexcept -> image_format { return ((impl*)handle)->format(); });
      // clang-format on
    }

    uid create() noexcept
    try {
      std::unique_lock lck {mtx};

      auto id         = gen_id();
      auto [it, succ] = map.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(id),
        std::forward_as_tuple(
          id, frame_image_t(fb_width, fb_height, 0, &memory_resource)));

      if (!succ)
        return uid();

      return table_ptr_to_id(&it->second);
    } catch (...) {
      return uid();
    }

    uid create_from(uid id) noexcept
    try {
      std::unique_lock lck {mtx};

      auto ptr        = id_to_table_ptr(id);
      auto new_id     = gen_id();
      auto [it, succ] = map.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(new_id),
        std::forward_as_tuple(new_id, *ptr));

      if (!succ)
        return uid();

      return table_ptr_to_id(&it->second);
    } catch (...) {
      return uid();
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
        {
          std::unique_lock lck {mtx};
          map.erase(ptr->id);
        }
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

    auto data(uid id) noexcept -> std::byte*
    try {
      auto ptr = id_to_table_ptr(id);
      return reinterpret_cast<std::byte*>(view(ptr->image).row_begin(0));
    } catch (...) {
      return nullptr;
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
  };

  frame_buffer_manager::frame_buffer_manager(
    uint32_t width,
    uint32_t height,
    image_format format,
    uuid backend_id)
    : m_pimpl {std::make_unique<impl>(width, height, format, backend_id)}
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

  auto frame_buffer_manager::data(uid id) noexcept -> std::byte*
  {
    return m_pimpl->data(id);
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
}