//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/shared_node_handle.hpp>
#include <yave/node/scene/managed_node_graph.hpp>
#include <yave/node/scene/layer_resource_info.hpp>

#include <memory>
#include <mutex>

namespace yave {

  /// Layer handle
  using layer_handle = descriptor_handle<struct node_layer*>;

  /// Layer resource handle
  using layer_resource_handle = node_handle;

  /// Layer info
  struct layer_info
  {
    layer_info(
      const std::string& name,
      const layer_handle& parent,
      const std::vector<layer_handle>& sublayers,
      const std::vector<layer_resource_info>& resources);

  public:
    auto name() const -> std::string;
    auto parent() const -> layer_handle;
    auto sublayers() const -> std::vector<layer_handle>;
    auto resources() const -> std::vector<layer_resource_info>;

  private:
    std::string m_name;
    layer_handle m_parent;
    std::vector<layer_handle> m_sublayers;
    std::vector<layer_resource_info> m_resources;
  };

  /// Node graph with layer control
  class layered_node_graph
  {
  public:
    /// Initialize root layer.
    layered_node_graph();

    /// Dtor
    ~layered_node_graph();

    /// exist?
    bool exists(const layer_handle& layer) const;

    /// get root layer
    auto root() const -> layer_handle;

    /// Get layers
    auto layers() const -> std::vector<layer_handle>;

    /// Get info
    auto get_info(const layer_handle& layer) const -> std::optional<layer_info>;

    /// Connect resources
    auto connect(
      const node_handle& src_n,
      const std::string& src_s,
      const node_handle& dst_n,
      const std::string& dst_s) -> connection_handle;

    /// Disconnect resourecs
    void disconnect(const connection_handle& handle);

    /// Clear
    void clear();

  public: /* layer control */
    /// Add new layer under target
    auto add_layer(const layer_handle& target) -> layer_handle;
    /// Remove layer and resources
    void remove_layer(const layer_handle& target);
    /// Set layer name
    void set_layer_name(const layer_handle& layer, const std::string& name);
    /// Get layer name
    auto get_layer_name(const layer_handle& layer) const
      -> std::optional<std::string>;
    /// Get sublayers
    auto get_sublayers(const layer_handle& layer) const
      -> std::vector<layer_handle>;
    /// Get parent layer
    auto get_parent(const layer_handle& layer) const -> layer_handle;
    /// Move layer below target layer.
    void move_below(const layer_handle& from, const layer_handle& to);
    /// Move layer above taregt layer.
    void move_above(const layer_handle& from, const layer_handle& to);
    /// Move layer into target layer (as highest layer in it).
    void move_into(const layer_handle& from, const layer_handle& to);
    /// Movable?
    bool movable_below(const layer_handle& from, const layer_handle& to) const;
    /// Movable?
    bool movable_above(const layer_handle& from, const layer_handle& to) const;
    /// Movable?
    bool movable_into(const layer_handle& from, const layer_handle& to) const;

  public: /* layer resource control */
    /// Add new resource to the layer
    auto add_resource(
      const std::string& name,
      const layer_handle& layer,
      layer_resource_scope scope) -> layer_resource_handle;
    /// Remove resource from layer
    void remove_resource(const layer_resource_handle& node);
    /// Get owning resources
    auto get_owning_resources(const layer_handle& layer) const
      -> std::vector<layer_resource_handle>;
    /// Get external resources
    auto get_inherited_resources(const layer_handle& layer) const
      -> std::vector<layer_resource_handle>;

  public: /* resource info */
    /// exists?
    bool exists(const layer_resource_handle& resource) const;
    /// exists?
    bool exists(
      const layer_resource_handle& resource,
      const layer_handle& layer) const;
    /// Get resource info
    auto get_info(const layer_resource_handle& handle) const
      -> std::optional<layer_resource_info>;
    /// Set new scope for the resource
    void set_resource_scope(
      const layer_resource_handle& res,
      layer_resource_scope scope);
    /// Get current resource scope
    auto get_resource_scope(const layer_resource_handle& res)
      -> std::optional<layer_resource_scope>;
    /// Set resource name
    void set_resource_name(
      const layer_resource_handle& res,
      const std::string& name);
    /// Get resource name
    auto get_resource_name(const layer_resource_handle& ret) const
      -> std::optional<std::string>;

  public: /* node info */
    /// register new node info
    [[nodiscard]] 
    bool register_node_info(const node_info& info);
    /// unregister node info
    void unregister_node_info(const node_info& info);

    /// register new node info
    [[nodiscard]] 
    bool register_node_info(const std::vector<node_info>& info);
    /// unregister node info
    void unregister_node_info(const std::vector<node_info>& info);

  public: /* node graph access */
    /// Access underlying node graph
    auto node_graph() const -> const managed_node_graph&;

  private:
    std::unique_ptr<node_layer> m_root;

  private: /* non-locking internal functions */
    /// access layer with handle
    auto _access(const layer_handle& layer) -> node_layer&;
    /// access layer with handle
    auto _access(const layer_handle& layer) const -> const node_layer&;
    /// access parent layer with handle
    auto _access_parent(const layer_handle& layer) -> node_layer&;
    /// exists?
    bool _exists(const layer_handle& layer) const;
    /// child?
    bool _is_child(const layer_handle& parent, const layer_handle& child) const;
    /// movable?
    bool _movable_below(const layer_handle& from, const layer_handle& to) const;
    /// movable?
    bool _movable_above(const layer_handle& from, const layer_handle& to) const;
    /// movable?
    bool _movable_into(const layer_handle& from, const layer_handle& to) const;
    /// layer
    auto _find_layer(const layer_resource_handle& node) const -> layer_handle;

  private:
    /// lock layer tree
    auto _lock() const -> std::unique_lock<std::mutex>;

  private:
    managed_node_graph m_node_graph;

  private:
    mutable std::mutex m_mtx;
  };

} // namespace yave
