//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_info.hpp>

#include <map>
#include <optional>
#include <functional>
#include <mutex>
#include <memory>

namespace yave {

  /// node_info manager.
  /// This class manages unique node_info instances.
  class node_info_manager
  {
    /// map type
    using map_type = std::map<std::string, std::shared_ptr<const node_info>>;

  public:
    /// info type
    using info_type = node_info;

    /// Copy constructor
    node_info_manager(const node_info_manager& other);
    /// Move constructor
    node_info_manager(node_info_manager&& other);
    /// operatpr=
    node_info_manager& operator=(const node_info_manager& other);
    /// operator=
    node_info_manager& operator=(node_info_manager&& other);

    /// Constructor
    node_info_manager();

    /// Constructor
    node_info_manager(const std::vector<node_info>& info_list);

    /// Add info.
    [[nodiscard]] bool add(const info_type& info);

    /// Check if the info exists.
    [[nodiscard]] bool exists(const std::string& name) const;

    /// Get list of info.
    [[nodiscard]] auto enumerate()
      -> std::vector<std::shared_ptr<const info_type>>;

    /// Find info.
    [[nodiscard]] auto find(const std::string& name) const
      -> std::shared_ptr<const info_type>;

    /// Remove info.
    void remove(const std::string& name);

    /// Remove info.
    void remove(map_type::const_iterator iter);

    /// Remove info.
    void remove(const node_info& info);

    /// Clear
    void clear();

    // Empty?
    bool empty() const;

  private:
    /// Lock
    [[nodiscard]] auto _lock() const -> std::unique_lock<std::mutex>;

  private:
    /// map
    map_type m_info;
    /// mutex
    mutable std::mutex m_mtx;
  };
} // namespace yave