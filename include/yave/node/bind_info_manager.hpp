//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/bind_info.hpp>

#include <functional>
#include <mutex>
#include <map>
#include <memory>

namespace yave {

  /// Manager of BindInfo instances.
  class BindInfoManager
  {
    /// map type
    using map_type =
      std::multimap<std::string, std::shared_ptr<const BindInfo>>;

  public:
    /// info type
    using info_type = BindInfo;

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

    /// Check if the info exists
    [[nodiscard]] bool exists(const std::string& name) const;

    /// Add info
    [[nodiscard]] bool add(const BindInfo& info);

    /// Remove info
    void remove(const std::string& name);

    /// Remove info
    void remove(
      const std::string& name,
      const std::vector<std::string>& input_sockets,
      const std::string& output_socket);

    /// Get list of info
    [[nodiscard]] std::vector<std::shared_ptr<const info_type>> enumerate();

    /// Find info
    [[nodiscard]] std::vector<std::shared_ptr<const info_type>>
      find(const std::string& name) const;

    /// Find info
    [[nodiscard]] std::vector<std::shared_ptr<const info_type>> find(
      const std::string& name,
      const std::vector<std::string>& input,
      const std::string& output) const;

    /// Find info
    [[nodiscard]] std::vector<std::shared_ptr<const info_type>>
      find(const BindInfo& info) const;

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