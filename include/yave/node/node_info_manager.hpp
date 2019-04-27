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
#include <memory>

namespace yave {

  /// NodeInfo manager.
  /// This class manages unique NodeInfo instances.
  class NodeInfoManager
  {
    /// map type
    using map_type = std::map<std::string, std::shared_ptr<const NodeInfo>>;

  public:
    /// info type
    using info_type = NodeInfo;

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

    /// Get list of info.
    [[nodiscard]] std::vector<std::shared_ptr<const info_type>> enumerate();

    /// Find info.
    [[nodiscard]] std::shared_ptr<const info_type>
      find(const std::string& name) const;

    /// Lock
    [[nodiscard]] std::unique_lock<std::mutex> lock() const;

    /// Clear
    void clear();

  private:
    /// map
    map_type m_info;
    /// mutex
    mutable std::mutex m_mtx;
  };
} // namespace yave