//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/bind_info.hpp>

#include <functional>
#include <mutex>
#include <map>

namespace yave {

  /// Manager of BindInfo instances.
  class BindInfoManager
  {
  public:
    /// info type
    using info_type = BindInfo;
    /// map type
    using map_type = std::multimap<std::string, info_type>;
    /// iterator type
    using iterator = typename map_type::iterator;
    /// const iterator type
    using const_iterator = typename map_type::const_iterator;

    /// Copy constructro
    BindInfoManager(const BindInfoManager& other);
    /// Move constructor
    BindInfoManager(BindInfoManager&& other);
    /// operator=
    BindInfoManager& operator=(const BindInfoManager& other);
    /// operator=
    BindInfoManager& operator=(BindInfoManager&& other);

    /// Constructor
    BindInfoManager();

    /// Add info
    [[nodiscard]] bool add(const BindInfo& info);

    /// Remove info
    void remove(const std::string& name);

    /// Remove info
    void remove(map_type::const_iterator iter);

    /// Check if the info exists
    [[nodiscard]] bool exists(const std::string& name) const;

    /// Remove info
    void remove(
      const std::string& name,
      const std::vector<std::string> input_sockets,
      const std::string& output_socket);

    /// Find info
    [[nodiscard]] std::vector<const info_type*>
      find(const std::string& name) const;

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
    map_type m_info;
    /// mutex
    mutable std::mutex m_mtx;
  };

} // namespace yave