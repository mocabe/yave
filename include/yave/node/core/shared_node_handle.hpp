//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_graph.hpp>

#include <memory>

namespace yave {

  /// Reference counted node handle.
  class shared_node_handle
  {
  public:
    shared_node_handle(node_graph& node_graph, const node_handle& node_handle)
    {
      m_ptr = std::make_shared<data>(node_graph, node_handle);
    }

    shared_node_handle()                          = default;
    ~shared_node_handle()                         = default;
    shared_node_handle(const shared_node_handle&) = default;
    shared_node_handle(shared_node_handle&&)      = default;
    shared_node_handle& operator=(const shared_node_handle&) = default;
    shared_node_handle& operator=(shared_node_handle&&) = default;

    [[nodiscard]] node_handle get() const
    {
      if (m_ptr)
        return m_ptr->handle;
      return nullptr;
    }

    [[nodiscard]] node_handle operator*() const
    {
      return get();
    }

    [[nodiscard]] node_handle release()
    {
      if (m_ptr) {
        node_handle ret = nullptr;
        std::swap(m_ptr->handle, ret);
        m_ptr = nullptr;
        return ret;
      }
      return nullptr;
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
      return static_cast<bool>(m_ptr);
    }

    [[nodiscard]] size_t use_count() const noexcept
    {
      return m_ptr.use_count();
    }

    void swap(shared_node_handle& other) noexcept
    {
      m_ptr.swap(other.m_ptr);
    }

  private:
    struct data
    {
      data(node_graph& g, const node_handle& h)
        : graph {g}
        , handle {h}
      {
        assert(g.exists(h));
      }
      ~data()
      {
        if (handle)
          graph.remove(handle);
      }
      node_graph& graph;
      node_handle handle;
    };

  private:
    std::shared_ptr<data> m_ptr;
  };
} // namespace yave