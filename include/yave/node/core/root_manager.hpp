//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node.hpp>
#include <yave/node/objects/function.hpp>
#include <yave/node/objects/instance_getter.hpp>

#include <yave/support/log.hpp>

namespace yave {

  class root_manager
  {
  public:
    /// Ctor.
    root_manager(node_graph& graph, bind_info_manager& binds);

    /// Lock.
    [[nodiscard]] std::unique_lock<std::mutex> lock() const;

    /// Add new root node.
    template <class T>
    [[nodiscard]] node_handle add_root(const std::string& name);

    /// Remove root node.
    void remove_root(const node_handle& node);

    /// Remove root node.
    void remove_root(const std::string& node);

    /// Check if the handle is root.
    [[nodiscard]] bool is_root(const node_handle& node) const;

    /// Get root node from its name.
    [[nodiscard]] node_handle find_root(const std::string& name) const;

    /// Get type of root.
    [[nodiscard]] object_ptr<const Type>
      get_type(const node_handle& node) const;

    /// Get type of root.
    [[nodiscard]] object_ptr<const Type>
      get_type(const std::string& node) const;

    /// Get all root
    [[nodiscard]] std::vector<node_handle> roots() const;

  private:
    /// reference to node graph
    node_graph& m_graph;
    /// reference to bind manager
    bind_info_manager& m_binds;

  private:
    struct root_table;
    /// root tables
    std::vector<root_table> m_roots;

  private:
    /// mutex
    mutable std::mutex m_mtx;
  };

  /* impl */

  struct root_manager::root_table
  {
    std::string name;
    node_handle node;
    object_ptr<const Type> type;
  };

  namespace detail {

    template <class T>
    struct RootNodeFunction : NodeFunction<RootNodeFunction<T>, T, T>
    {
      typename RootNodeFunction::return_type code() const
      {
        return this->template arg<0>();
      }
    };

    inline const char* get_root_node_input_socket()
    {
      return "in";
    }

    inline const char* get_root_node_output_socket()
    {
      return "out";
    }

    template <class T>
    auto get_root_node_function_getter()
    {
      return make_object<InstanceGetterFunction<RootNodeFunction<T>>>();
    }

    template <class T>
    node_info get_root_node_info(const std::string& name)
    {
      return {
        name, {get_root_node_input_socket()}, {get_root_node_output_socket()}};
    }

    template <class T>
    bind_info get_root_node_bind(const std::string& name)
    {
      return {name,
              {get_root_node_input_socket()},
              get_root_node_output_socket(),
              get_root_node_function_getter<T>(),
              "Root node managed by root_manager"};
    }
  } // namespace detail


  template <class T>
  node_handle root_manager::add_root(const std::string& name)
  {
    auto it = std::find_if(m_roots.begin(), m_roots.end(), [&](auto& table) {
      return table.name == name && same_type(table.type, object_type<T>());
    });

    if (it != m_roots.end()) {
      Error("Root Already Exists");
      return nullptr;
    }

    auto node = m_graph.add(detail::get_root_node_info<T>(name));

    if (!node) {
      Error("Could not create root");
      return nullptr;
    }

    if (!m_binds.add(detail::get_root_node_bind<T>(name))) {
      m_graph.remove(node);
      Error("Could not add binding");
      return nullptr;
    }

    m_roots.push_back({name, node, object_type<T>()});

    return node;
  }

} // namespace yave