//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_graph.hpp>

#include <memory>

namespace yave {

  class managed_node_graph;

  /// Reference counted node handle.
  class shared_node_handle
  {
  public:
    shared_node_handle(
      managed_node_graph& node_graph,
      const node_handle& node_handle)
    {
      if (node_handle)
        m_ptr = std::make_shared<_data>(node_graph, node_handle);
      else
        m_ptr = nullptr;
    }

    shared_node_handle(nullptr_t)
    {
      m_ptr = nullptr;
    }

    shared_node_handle()                          = default;
    ~shared_node_handle()                         = default;
    shared_node_handle(const shared_node_handle&) = default;
    shared_node_handle(shared_node_handle&&)      = default;
    shared_node_handle& operator=(const shared_node_handle&) = default;
    shared_node_handle& operator=(shared_node_handle&&) = default;

    [[nodiscard]] explicit operator bool() const noexcept
    {
      return static_cast<bool>(m_ptr);
    }

    [[nodiscard]] auto get() const -> node_handle
    {
      if (m_ptr)
        return m_ptr->handle;
      return nullptr;
    }

    [[nodiscard]] auto operator*() const -> node_handle
    {
      return get();
    }

    [[nodiscard]] auto release() -> node_handle
    {
      if (m_ptr) {
        node_handle ret = nullptr;
        std::swap(m_ptr->handle, ret);
        m_ptr = nullptr;
        return ret;
      }
      return nullptr;
    }

    [[nodiscard]] auto use_count() const noexcept -> size_t
    {
      return m_ptr.use_count();
    }

    void swap(shared_node_handle& other) noexcept
    {
      m_ptr.swap(other.m_ptr);
    }

  private:
    struct _data
    {
      _data(managed_node_graph& g, const node_handle& h);
      ~_data();
      managed_node_graph& graph;
      node_handle handle;
    };

  private:
    std::shared_ptr<_data> m_ptr;
  };
} // namespace yave