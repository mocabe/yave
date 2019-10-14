//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/bind_info.hpp>

#include <functional>
#include <mutex>
#include <map>
#include <memory>

namespace yave {

  /// Manager of bind_info instances.
  class bind_info_manager
  {
    /// map type
    using map_type =
      std::multimap<std::string, std::shared_ptr<const bind_info>>;

  public:
    /// info type
    using info_type = bind_info;

    /// Copy constructro
    bind_info_manager(const bind_info_manager& other);
    /// Move constructor
    bind_info_manager(bind_info_manager&& other);
    /// operator=
    bind_info_manager& operator=(const bind_info_manager& other);
    /// operator=
    bind_info_manager& operator=(bind_info_manager&& other);

    /// Constructor
    bind_info_manager();

    /// Constructor
    bind_info_manager(const std::vector<bind_info>& bind_list);

    /// Check if the info exists
    [[nodiscard]] bool exists(const std::string& name) const;

    /// Add info
    [[nodiscard]] bool add(const bind_info& info);

    /// Get list of info
    [[nodiscard]] auto enumerate()
      -> std::vector<std::shared_ptr<const info_type>>;

    /// Find info
    [[nodiscard]] auto find(const std::string& name) const
      -> std::vector<std::shared_ptr<const info_type>>;

    /// Find info
    [[nodiscard]] auto find(
      const std::string& name,
      const std::vector<std::string>& input,
      const std::string& output) const
      -> std::vector<std::shared_ptr<const info_type>>;

    /// Find info
    [[nodiscard]] auto find(const bind_info& info) const
      -> std::vector<std::shared_ptr<const info_type>>;

    /// Find matched info
    [[nodiscard]] auto get_binds(const node_info& info) const
      -> std::vector<std::shared_ptr<const info_type>>;

    /// Find matched info
    [[nodiscard]] auto get_binds(
      const std::string& name,
      const std::vector<std::string>& input_sockets,
      const std::vector<std::string>& output_sockets) const
      -> std::vector<std::shared_ptr<const info_type>>;

    /// Remove info
    void remove(const bind_info& info);

    /// Remove info
    void remove(const std::string& name);

    /// Remove info
    void remove(
      const std::string& name,
      const std::vector<std::string>& input_sockets,
      const std::string& output_socket);

    /// Clear
    void clear();

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