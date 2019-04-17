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
  struct SocketInstance
  {
    /// socket object
    object_ptr<> object;
    /// socket type
    object_ptr<const Type> type;
    /// bind info
    const BindInfo* bind_info;
  };

  /// Socket instance manager
  class SocketInstanceManager
  {
  public:
    /// Constructor
    SocketInstanceManager();
    /// Copy constructor
    SocketInstanceManager(const SocketInstanceManager& other);
    /// Move constructor
    SocketInstanceManager(SocketInstanceManager&& other);
    /// operator=
    SocketInstanceManager& operator=(const SocketInstanceManager& other);
    /// operator=
    SocketInstanceManager& operator=(SocketInstanceManager&& other);

    /// find instance
    std::optional<SocketInstance>
      find(const NodeHandle& h, const std::string& socket) const;

    /// add instance
    void add(
      const NodeHandle& h,
      const std::string& socket,
      const SocketInstance& instance);

    /// remove instance
    void remove(const NodeHandle& h, const std::string& socket);

    /// remove instances
    void remove(const NodeHandle& h);

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
      SocketInstance instance;
    };

  private:
    /// map
    std::multimap<NodeHandle, instanceTable> m_map;
    /// mutex
    mutable std::mutex m_mtx;
  };
} // namespace yave