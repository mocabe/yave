//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/core/data_types/primitive.hpp>
#include <yave/support/overloaded.hpp>

#include <mutex>

namespace yave {

  primitive_container::primitive_container()
    : m_prim {}
    , m_mtx {}
  {
  }

  primitive_container::primitive_container(const primitive_t& prim)
    : m_prim {prim}
    , m_mtx {}
  {
  }

  primitive_container::primitive_container(const primitive_container& other)
    : m_prim {other.m_prim}
    , m_mtx {}
  {
  }

  void primitive_container::set(const primitive_t& prim)
  {
    std::lock_guard lck {m_mtx};
    // FIXME: This does not actually assign new value on clang (gcc is fine).
    // m_prim = prim;
    std::visit(overloaded {[&](const auto& v) { m_prim = v; }}, prim);
  }

  primitive_t primitive_container::get() const
  {
    std::lock_guard lck {m_mtx};
    return m_prim;
  }
}