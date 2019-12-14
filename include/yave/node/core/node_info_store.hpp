//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_info.hpp>

#include <map>
#include <memory>

namespace yave {

  class node_info_store
  {
    using map_type = std::map<std::string, std::shared_ptr<node_info>>;

  public:
    node_info_store();
    node_info_store(const node_info_store&);
    node_info_store(node_info_store&&) noexcept;
    node_info_store& operator=(const node_info_store&);
    node_info_store& operator=(node_info_store&&) noexcept;

    /// add info
    [[nodiscard]] bool add(const node_info& info);

    /// add info
    [[nodiscard]] bool add(const std::vector<node_info>& info);

    /// exist?
    [[nodiscard]] bool exists(const std::string& name) const;

    /// find info from name
    [[nodiscard]] auto find(const std::string& name) const
      -> std::shared_ptr<const node_info>;

    /// remove info from name
    void remove(const std::string& name);

    /// remove all info from name list
    void remove(const std::vector<std::string>& names);

    /// clear
    void clear();

    /// empty?
    bool empty() const;

  private:
    /// map
    map_type m_map;
  };
}