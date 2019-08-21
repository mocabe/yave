//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/frame_buffer/frame_buffer_manager.hpp>
#include <yave/obj/frame_buffer/frame_buffer_pool.hpp>

#include <yave/support/id.hpp>
#include <yave/rts/atomic.hpp>

namespace yave {

  namespace {

    // header of frame buffer
    struct fb_head
    {
      uint8_t* mem;                       // pointer to allocated space.
      uint8_t* buff;                      // pointer to image buffer.
      atomic_refcount<uint64_t> refcount; // reference count.
      uid id;                             // id (for debug).
    };

    fb_head* allocate_fb(size_t sz, uid id)
    {
      uint8_t* mem  = new uint8_t[sizeof(fb_head) + sz] {};
      fb_head* head = new (mem) fb_head {mem, mem + sizeof(fb_head), 1U, id};
      return head;
    }

    void deallocate_fb(fb_head* head)
    {
      auto* mem = head->mem;
      head->~fb_head();
      delete[] mem;
    }

    size_t byte_size(uint32_t width, uint32_t height, image_format fmt)
    {
      return (size_t)width * height * byte_per_pixel(fmt);
    }
  } // namespace

  frame_buffer_manager::frame_buffer_manager(
    uint32_t width,
    uint32_t height,
    const image_format& format,
    const uuid& backend_id)
    : m_format {format}
    , m_width {width}
    , m_height {height}
  {
    // create pool object
    m_pool = make_object<FrameBufferPool>(
      (void*)this,
      backend_id,
      [](void* h) { return ((frame_buffer_manager*)h)->create(); },
      [](void* h, uid id) { return ((frame_buffer_manager*)h)->create(id); },
      [](void* h, uid id) { return ((frame_buffer_manager*)h)->ref(id); },
      [](void* h, uid id) { return ((frame_buffer_manager*)h)->unref(id); },
      [](void* h, uid id) { return ((frame_buffer_manager*)h)->get_data(id); },
      [](const void* h) { return ((const frame_buffer_manager*)h)->format(); },
      [](const void* h) { return ((const frame_buffer_manager*)h)->width(); },
      [](const void* h) { return ((const frame_buffer_manager*)h)->height(); });
  }

  frame_buffer_manager::~frame_buffer_manager() noexcept
  {
    assert(m_data.size() == m_id.size());
    for (auto&& p : m_data) {
      deallocate_fb((fb_head*)p);
    }
  }

  uid frame_buffer_manager::create()
  {
    uid id     = uid::random_generate();
    auto* buff = allocate_fb(byte_size(m_width, m_height, m_format), id);
    { // lock
      std::lock_guard lck {m_mtx};
      m_data.push_back(buff);
      m_id.push_back(id);
    }
    assert(m_data.size() == m_id.size());
    return id;
  }

  uid frame_buffer_manager::create(const uint8_t* parent)
  {
    uid id     = uid::random_generate();
    auto* buff = allocate_fb(byte_size(m_width, m_height, m_format), id);
    if (parent)
      std::memcpy(buff->buff, parent, byte_size(m_width, m_height, m_format));
    { // lock
      std::lock_guard lck {m_mtx};
      m_data.push_back(buff);
      m_id.push_back(id);
    }
    assert(m_data.size() == m_id.size());
    return id;
  }

  uid frame_buffer_manager::create(uid parent)
  {
    assert(m_id.size() == m_data.size());
    { // lock
      std::lock_guard lck {m_mtx};
      for (size_t i = 0; i < m_id.size(); ++i) {
        if (m_id[i] == parent) {
          assert(((fb_head*)m_data[i])->id == m_id[i]);
          uid id     = uid::random_generate();
          auto* buff = allocate_fb(byte_size(m_width, m_height, m_format), id);
          m_data.push_back(buff);
          m_id.push_back(id);
          return id;
        }
      }
    }
    // fallback
    return create();
  }

  void frame_buffer_manager::ref(uid id)
  {
    assert(m_id.size() == m_data.size());
    { // lock
      std::lock_guard lck {m_mtx};
      for (size_t i = 0; i < m_id.size(); ++i) {
        if (m_id[i] == id) {
          assert(((fb_head*)m_data[i])->id == m_id[i]);
          ((fb_head*)m_data[i])->refcount.fetch_add();
          return;
        }
      }
    }
  }

  void frame_buffer_manager::unref(uid id)
  {
    assert(m_id.size() == m_data.size());
    { // lock
      std::lock_guard lck {m_mtx};
      for (size_t i = 0; i < m_id.size(); ++i) {
        if (m_id[i] == id) {
          assert(((fb_head*)m_data[i])->id == m_id[i]);
          if (((fb_head*)m_data[i])->refcount.fetch_sub() == 1) {
            std::atomic_thread_fence(std::memory_order_acquire);
            deallocate_fb((fb_head*)m_data[i]);
          }
          m_data.erase(m_data.begin() + i);
          m_id.erase(m_id.begin() + i);
          return;
        }
      }
    }
  }

  uint8_t* frame_buffer_manager::get_data(uid id)
  {
    assert(m_id.size() == m_data.size());
    { // lock
      std::lock_guard lck {m_mtx};
      for (size_t i = 0; i < m_id.size(); ++i) {
        if (m_id[i] == id) {
          assert(((fb_head*)m_data[i])->id == m_id[i]);
          return ((fb_head*)m_data[i])->buff;
        }
      }
    }
    return nullptr;
  }

  const uint8_t* frame_buffer_manager::get_data(uid id) const
  {
    assert(m_id.size() == m_data.size());
    { // lock
      std::lock_guard lck {m_mtx};
      for (size_t i = 0; i < m_id.size(); ++i) {
        if (m_id[i] == id) {
          assert(((fb_head*)m_data[i])->id == m_id[i]);
          return ((fb_head*)m_data[i])->buff;
        }
      }
    }
    return nullptr;
  }

  image_format frame_buffer_manager::format() const
  {
    return m_format;
  }

  uint32_t frame_buffer_manager::width() const
  {
    return m_width;
  }

  uint32_t frame_buffer_manager::height() const
  {
    return m_height;
  }

  size_t frame_buffer_manager::size() const
  {
    assert(m_id.size() == m_data.size());
    { // lock
      std::lock_guard lck {m_mtx};
      return m_id.size();
    }
  }

  std::vector<uid> frame_buffer_manager::buffers() const
  {
    { // lock
      std::lock_guard lck {m_mtx};
      return m_id;
    }
  }

  object_ptr<FrameBufferPool> frame_buffer_manager::get_pool_object() const
  {
    return m_pool;
  }
}