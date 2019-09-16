//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/bind_info.hpp>

#include <mutex>
#include <map>
#include <optional>

namespace yave {

  /// Socket instance cache
  struct socket_instance
  {
    /// socket object
    object_ptr<const Object> instance;
    /// type
    object_ptr<const Type> type;
    /// bind
    std::shared_ptr<const bind_info> bind;
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
    auto find(const uid& id, const std::string& socket) const
      -> std::optional<socket_instance>;

    /// size
    auto size() const -> size_t;

    /// add instance
    void add(
      const uid& id,
      const std::string& socket,
      const socket_instance& socket_instance);

    /// remove instance
    void remove(const uid& id, const std::string& socket);

    /// remove instances
    void remove(const uid& id);

    /// clear
    void clear();

  private:
    /// lock
    [[nodiscard]] auto _lock() const -> std::unique_lock<std::mutex>;

  private:
    struct instanceTable
    {
      std::string socket;
      socket_instance si;
    };

  private:
    /// map
    std::multimap<uid, instanceTable> m_map;
    /// mutex
    mutable std::mutex m_mtx;
  };
} // namespace yave