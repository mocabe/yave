//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/node_handle.hpp>
#include <yave/node/bind_info.hpp>

#include <mutex>
#include <map>
#include <optional>

namespace yave {

  /// Socket instance cache
  struct socket_instance
  {
    /// socket object
    object_ptr<> object;
    /// socket type
    object_ptr<const Type> type;
    /// bind info
    const bind_info* bind_info;
  };

  /// Socket instance manager
  class socket_instance_manager
  {
  public:
    /// Constructor
    socket_instance_manager();
    /// Copy constructor
    socket_instance_manager(const socket_instance_manager& other);
    /// Move constructor
    socket_instance_manager(socket_instance_manager&& other);
    /// operator=
    socket_instance_manager& operator=(const socket_instance_manager& other);
    /// operator=
    socket_instance_manager& operator=(socket_instance_manager&& other);

    /// find instance
    std::optional<socket_instance>
      find(const node_handle& h, const std::string& socket) const;

    /// add instance
    void add(
      const node_handle& h,
      const std::string& socket,
      const socket_instance& instance);

    /// remove instance
    void remove(const node_handle& h, const std::string& socket);

    /// remove instances
    void remove(const node_handle& h);

    /// lock
    [[nodiscard]] std::unique_lock<std::mutex> lock() const;

    /// clear
    void clear();

    /// size
    size_t size() const;

  private:
    struct instanceTable
    {
      std::string socket;
      socket_instance instance;
    };

  private:
    /// map
    std::multimap<node_handle, instanceTable> m_map;
    /// mutex
    mutable std::mutex m_mtx;
  };
} // namespace yave