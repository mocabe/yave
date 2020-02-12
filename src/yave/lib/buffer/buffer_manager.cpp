//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/buffer/buffer_manager.hpp>
#include <yave/obj/buffer/buffer_pool.hpp>

#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(buffer_manager)

namespace yave {

  namespace {

    struct buff_header
    {
      /// Reference count.
      uint64_t refcount;
      /// Pointer to allocated memory area on which this object is constructed
      /// by placement new.
      std::byte* mem;
      /// Pointer to head of variable length buffer constructed after header.
      std::byte* buff;
      /// Allocated buffer size (not include header).
      /// Header size can be calculated by buff-mem.
      uint64_t size;
      /// Alignment of allocated buffer.
      uint64_t alignment;
    };

    /// Allocate buffer
    buff_header* allocate_buffer(size_t size, size_t alignment)
    {
      // overflow
      if (std::numeric_limits<size_t>::max() - sizeof(buff_header) < size)
        return nullptr;

      auto aligned_header_size =
        (sizeof(buff_header) + alignment - 1) / alignment * alignment;

      auto alloc_size = aligned_header_size + size;

      auto mem = new (std::nothrow) std::byte[alloc_size];

      if (!mem)
        return nullptr;

      // construct head
      buff_header* head = new (mem)
        buff_header {1U, mem, mem + aligned_header_size, size, alignment};

      // construct rest of buffer
      std::uninitialized_value_construct_n(head->buff, size);

      Info(
        g_logger,
        "Allocated new buffer: size={}, addr={}",
        size,
        std::uintptr_t(mem));

      return head;
    }

    /// Deallocate buffer
    void deallocate_buffer(buff_header* head)
    {
      if (!head)
        return;

      Info(
        g_logger,
        "Deallocate buffer: addr={}, size={}",
        std::uintptr_t(head->mem),
        head->size);

      auto* mem = head->mem;
      head->~buff_header();
      delete[] mem;
    }

  } // namespace

  auto buffer_manager::_lock() const -> std::unique_lock<std::mutex>
  {
    return std::unique_lock {m_mtx};
  }

  auto buffer_manager::_find_data(uid id) const -> void*
  {
    auto lb   = std::lower_bound(m_id.begin(), m_id.end(), id);
    auto dist = std::distance(m_id.begin(), lb);

    if (lb == m_id.end())
      return nullptr;

    if (*lb != id)
      return nullptr;

    return m_data[dist];
  }

  void buffer_manager::_insert(uid id, void* data)
  {
    auto lb   = std::lower_bound(m_id.begin(), m_id.end(), id);
    auto dist = std::distance(m_id.begin(), lb);

    if (lb != m_id.end())
      assert(*lb != id);

    m_id.insert(lb, id);
    m_data.insert(m_data.begin() + dist, data);
  }

  buffer_manager::buffer_manager()
    : buffer_manager(uuid())
  {
  }

  buffer_manager::buffer_manager(const uuid& backend_id)
    : m_backend_id {backend_id}
  {
    init_logger();

    auto lck = _lock();

    // clang-format off

    m_pool = make_object<BufferPool>(
      (void*)this,
      m_backend_id,
      [](void* handle, uint64_t size) noexcept -> uint64_t   { return ((buffer_manager*)handle)->create(size).data; },
      [](void* handle, uint64_t id)   noexcept -> uint64_t   { return ((buffer_manager*)handle)->create_from({id}).data; },
      [](void* handle, uint64_t id)   noexcept -> void       { return ((buffer_manager*)handle)->ref({id}); },
      [](void* handle, uint64_t id)   noexcept -> void       { return ((buffer_manager*)handle)->unref({id}); },
      [](void* handle, uint64_t id)   noexcept -> uint64_t   { return ((buffer_manager*)handle)->use_count({id}); },
      [](void* handle, uint64_t id)   noexcept -> std::byte* { return ((buffer_manager*)handle)->data({id}); },
      [](void* handle, uint64_t id)   noexcept -> uint64_t   { return ((buffer_manager*)handle)->size({id}); });

    // clang-format on
  }

  buffer_manager::buffer_manager(buffer_manager&& other) noexcept
  {
    std::lock_guard lck {other.m_mtx};
    m_id         = std::move(other.m_id);
    m_data       = std::move(other.m_data);
    m_backend_id = std::move(other.m_backend_id);
    m_pool       = std::move(other.m_pool);
  }

  buffer_manager& buffer_manager::operator=(buffer_manager&& other) noexcept
  {
    std::lock_guard lck {other.m_mtx};
    m_id         = std::move(other.m_id);
    m_data       = std::move(other.m_data);
    m_backend_id = std::move(other.m_backend_id);
    m_pool       = std::move(other.m_pool);
    return *this;
  }

  buffer_manager::~buffer_manager() noexcept
  {
    auto lck = _lock();
    for (auto&& data : m_data) {
      deallocate_buffer((buff_header*)data);
    }
  }

  auto buffer_manager::create(uint64_t size, uint64_t alignment) noexcept -> uid
  {
    auto lck = _lock();

    uid id     = uid::random_generate();
    auto* buff = allocate_buffer(size, alignment);

    if (!buff)
      return uid();

    _insert(id, buff);

    return id;
  }

  auto buffer_manager::create_from(uid parent_id) noexcept -> uid
  {
    auto lck = _lock();

    auto parent = (buff_header*)_find_data(parent_id);

    if (!parent)
      return uid();

    auto parent_size = ((buff_header*)parent)->size;

    // allocate new buffer
    auto id   = uid::random_generate();
    auto buff = allocate_buffer(parent_size, parent->alignment);

    if (!buff)
      return uid();

    // copy data
    std::memcpy(buff->buff, parent->buff, parent_size);

    // register
    _insert(id, buff);

    return id;
  }

  void buffer_manager::ref(uid id) noexcept
  {
    auto lck = _lock();

    auto data = _find_data(id);

    if (!data)
      return;

    ((buff_header*)data)->refcount++;
  }

  void buffer_manager::unref(uid id) noexcept
  {
    auto lck = _lock();

    auto lb   = std::lower_bound(m_id.begin(), m_id.end(), id);
    auto dist = std::distance(m_id.begin(), lb);

    if (lb == m_id.end())
      return;

    if (*lb != id)
      return;

    auto data = m_data[dist];

    if (((buff_header*)data)->refcount-- == 1) {
      deallocate_buffer((buff_header*)data);
      m_id.erase(lb);
      m_data.erase(m_data.begin() + dist);
    }
  }

  auto buffer_manager::use_count(uid id) const noexcept -> uint64_t
  {
    auto lck = _lock();

    auto data = _find_data(id);

    if (!data)
      return 0;

    return ((buff_header*)data)->refcount;
  }

  auto buffer_manager::data(uid id) const noexcept -> std::byte*
  {
    auto lck = _lock();

    auto data = _find_data(id);

    if (!data)
      return nullptr;

    if (((buff_header*)data)->size == 0)
      return nullptr;

    return ((buff_header*)data)->buff;
  }

  auto buffer_manager::size(uid id) const noexcept -> uint64_t
  {
    auto lck = _lock();

    auto data = _find_data(id);

    if (!data)
      return 0;

    return ((buff_header*)data)->size;
  }

  auto buffer_manager::get_pool_object() const noexcept
    -> object_ptr<BufferPool>
  {
    return m_pool;
  }
} // namespace yave