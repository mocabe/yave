//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/node_info.hpp>

#include <map>
#include <optional>
#include <functional>
#include <mutex>

namespace yave {

  /// NodeInfo manager.
  /// This class manages unique NodeInfo instances.
  class NodeInfoManager
  {
  public:
    /// info type
    using info_type = NodeInfo;
    /// map type
    using map_type = std::map<std::string, info_type>;
    /// iterator
    using iterator = typename map_type::iterator;
    /// const iterator
    using const_iterator = typename map_type::const_iterator;

    /// Copy constructor
    NodeInfoManager(const NodeInfoManager& other);
    /// Move constructor
    NodeInfoManager(NodeInfoManager&& other);
    /// operatpr=
    NodeInfoManager& operator=(const NodeInfoManager& other);
    /// operator=
    NodeInfoManager& operator=(NodeInfoManager&& other);

    /// Constructor
    NodeInfoManager();

    /// Add info.
    [[nodiscard]] bool add(const info_type& info);

    /// Remove info.
    void remove(const std::string& name);

    /// Remove info.
    void remove(map_type::const_iterator iter);

    /// Remove info.
    void remove(const NodeInfo& info);

    /// Check if the info exists.
    [[nodiscard]] bool exists(const std::string& name) const;

    /// Find info.
    [[nodiscard]] const info_type* find(const std::string& name) const;

    /// cbegin
    [[nodiscard]] const_iterator cbegin() const;

    /// cend
    [[nodiscard]] const_iterator cend() const;

    /// Lock
    [[nodiscard]] std::unique_lock<std::mutex> lock() const;

    /// Clear
    void clear();

  private:
    void remove_iterator(map_type::const_iterator iter);

  private:
    /// map
    std::map<std::string, info_type> m_info;
    /// mutex
    mutable std::mutex m_mtx;
  };
} // namespace yave