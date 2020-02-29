//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/image_buffer/image_buffer_manager.hpp>

#include <yave/support/overloaded.hpp>
#include <boost/gil.hpp>
#include <boost/gil/extension/dynamic_image/any_image.hpp>
#include <map>
#include <shared_mutex>

namespace yave {

  namespace {

    using namespace boost;
    using allocator = std::pmr::polymorphic_allocator<std::byte>;

#define PMR_IMAGE_TYPE(P) \
  using P##_image_t = gil::image<gil::P##_pixel_t, false, allocator>

    // image types
    PMR_IMAGE_TYPE(rgba8);
    PMR_IMAGE_TYPE(rgba16);
    PMR_IMAGE_TYPE(rgba32f);
    PMR_IMAGE_TYPE(rgb8);
    PMR_IMAGE_TYPE(rgb16);

    // variant of all images
    using any_image_t = gil::any_image<mpl::vector<
      rgba8_image_t,
      rgba16_image_t,
      rgba32f_image_t,
      rgb8_image_t,
      rgb16_image_t>>;

    // info block for image
    struct image_table
    {
      // refcount
      atomic_refcount<uint32_t> refcount;
      // internal ID of this cell
      const uint32_t id;
      // image
      any_image_t image;

      // construct image
      image_table(uint32_t id, any_image_t&& img)
        : refcount {1}
        , id {id}
        , image {std::move(img)}
      {
      }

      image_table(uint32_t id, const image_table& other)
        : refcount {1}
        , id {id}
        , image {other.image} // gil propagetes allocator on copy!!
      {
      }
    };

    auto table_ptr_to_id(image_table* ptr)
    {
      return uid {std::uintptr_t(ptr)};
    }

    auto id_to_table_ptr(uid id)
    {
      return (image_table*)((uintptr_t(id.data)));
    }

  } // namespace

  class image_buffer_manager::impl
  {
  public:
    // memory pool
    std::pmr::unsynchronized_pool_resource memory_resource;
    // map of images
    std::pmr::map<uint32_t, image_table> map;
    // lock
    std::shared_mutex mtx;
    // pool obj
    object_ptr<const ImageBufferPool> m_pool_object;

  public:
    impl(const uuid& backend_id)
      : memory_resource {}
      , map {&memory_resource}
    {
      // create pool object
      // clang-format off
      m_pool_object = make_object<ImageBufferPool>(
        (void*)this,
        backend_id,
        [](void* h, uint32_t wd, uint32_t ht, image_format f) noexcept -> uint64_t     { return ((impl*)h)->create(wd, ht, f).data; },
        [](void* h, uint64_t id)                              noexcept -> uint64_t     { return ((impl*)h)->create_from({id}).data; },
        [](void* h, uint64_t id)                              noexcept -> void         { return ((impl*)h)->ref({id}); },
        [](void* h, uint64_t id)                              noexcept -> void         { return ((impl*)h)->unref({id}); },
        [](void* h, uint64_t id)                              noexcept -> uint64_t     { return ((impl*)h)->use_count({id}); },
        [](void* h, uint64_t id)                              noexcept -> std::byte*   { return ((impl*)h)->data({id}); },
        [](void* h, uint64_t id)                              noexcept -> uint32_t     { return ((impl*)h)->width({id}); },
        [](void* h, uint64_t id)                              noexcept -> uint32_t     { return ((impl*)h)->height({id}); },
        [](void* h, uint64_t id)                              noexcept -> image_format { return ((impl*)h)->format({id}); });
      // clang-format on
    }

    auto gen_id()
    {
      static std::mt19937 rng(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
      return rng();
    }

    uid create(uint32_t width, uint32_t height, image_format format) noexcept
    try {

      std::unique_lock lck {mtx};

      auto id = gen_id();

#define CREATE_IMAGE_TABLE(FMT)                                               \
  if (format == image_format::FMT) {                                          \
    auto [it, succ] = map.emplace(                                            \
      std::piecewise_construct,                                               \
      std::forward_as_tuple(id),                                              \
      std::forward_as_tuple(                                                  \
        id, any_image_t(FMT##_image_t(width, height, 0, &memory_resource)))); \
                                                                              \
    if (!succ)                                                                \
      return {0};                                                             \
                                                                              \
    return table_ptr_to_id(&it->second);                                      \
  }

      CREATE_IMAGE_TABLE(rgba8);
      CREATE_IMAGE_TABLE(rgba16);
      CREATE_IMAGE_TABLE(rgba32f);
      CREATE_IMAGE_TABLE(rgb8);
      CREATE_IMAGE_TABLE(rgb16);

      // invalid format
      return {0};

    } catch (...) {
      return {0};
    }

    uid create_from(uid id) noexcept
    try {

      std::unique_lock lck {mtx};

      auto cell = id_to_table_ptr(id);

      auto id_intern  = gen_id();
      auto [it, succ] = map.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(id_intern),
        std::forward_as_tuple(id_intern, *cell));

      if (!succ)
        return {0};

      return {std::uintptr_t(&it->second)};

    } catch (...) {
      return {0};
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
      // destroy
      if (ptr->refcount.fetch_sub() == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);
        // destroy image
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

      std::byte* ret;

      apply_visitor(
        overloaded {[&](auto& img) {
          ret = reinterpret_cast<std::byte*>(view(img).row_begin(0));
        }},
        ptr->image);

      return ret;

    } catch (...) {
      return nullptr;
    }

    auto width(uid id) noexcept -> uint32_t
    try {

      auto ptr = id_to_table_ptr(id);

      return ptr->image.width();

    } catch (...) {
      return 0;
    }

    auto height(uid id) noexcept -> uint32_t
    try {

      auto ptr = id_to_table_ptr(id);

      return ptr->image.height();

    } catch (...) {
      return 0;
    }

    auto format(uid id) noexcept -> image_format
    try {
      auto ptr = id_to_table_ptr(id);

      image_format ret;

      apply_visitor(
        overloaded {[&](rgba8_image_t&) { ret = image_format::rgba8; },
                    [&](rgba16_image_t&) { ret = image_format::rgba16; },
                    [&](rgba32f_image_t&) { ret = image_format::rgba32f; },
                    [&](rgb8_image_t&) { ret = image_format::rgb8; },
                    [&](rgb16_image_t&) { ret = image_format::rgb16; },
                    [](auto& a) { static_assert(!sizeof(a)); }},
        ptr->image);

      return ret;

    } catch (...) {
      return image_format::unknown;
    }

    auto get_pool_object() noexcept
    {
      return m_pool_object;
    }
  };

  image_buffer_manager::image_buffer_manager(const uuid& backend_id)
    : m_pimpl {std::make_unique<impl>(backend_id)}
  {
  }

  image_buffer_manager::~image_buffer_manager() noexcept
  {
  }

  image_buffer_manager::image_buffer_manager(
    image_buffer_manager&& other) noexcept = default;

  image_buffer_manager& image_buffer_manager::operator=(
    image_buffer_manager&& other) noexcept = default;

  uid image_buffer_manager::create(
    uint32_t width,
    uint32_t height,
    image_format format) noexcept
  {
    return m_pimpl->create(width, height, format);
  }

  uid image_buffer_manager::create_from(uid id) noexcept
  {
    return m_pimpl->create_from(id);
  }

  void image_buffer_manager::ref(uid id) noexcept
  {
    return m_pimpl->ref(id);
  }

  void image_buffer_manager::unref(uid id) noexcept
  {
    return m_pimpl->unref(id);
  }

  auto image_buffer_manager::use_count(uid id) const noexcept -> uint64_t
  {
    return m_pimpl->use_count(id);
  }

  auto image_buffer_manager::data(uid id) noexcept -> std::byte*
  {
    return m_pimpl->data(id);
  }

  auto image_buffer_manager::width(uid id) const noexcept -> uint32_t
  {
    return m_pimpl->width(id);
  }

  auto image_buffer_manager::height(uid id) const noexcept -> uint32_t
  {
    return m_pimpl->height(id);
  }

  auto image_buffer_manager::format(uid id) const noexcept -> image_format
  {
    return m_pimpl->format(id);
  }

  auto image_buffer_manager::get_pool_object() const noexcept
    -> object_ptr<const ImageBufferPool>
  {
    return m_pimpl->get_pool_object();
  }
} // namespace yave