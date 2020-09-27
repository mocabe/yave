//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <cassert>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <map>

namespace yave::graph {

  // Low-level Graph implementation.
  // Graph is not like general graph representation. It has 2 different vertex
  // type; Node and Socket.
  // Each socket can be connected to nodes, and Edge represents connection
  // between sockets. Also, Edge has direction.

  // TODO: Possibly replace internal representation using Boost.Graph library.
  // TODO: Use memory pool for allocation to improve data locality.

  struct graph_id_generator
  {
    // id type
    using id_type = uint64_t;

    /// generate random ID for graph objects.
    auto generate() -> id_type
    {
      static auto mt = std::mt19937_64(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
      return id_type(mt());
    }
  };

  template <template <class> class ValueType, class Graph>
  struct graph_container
  {
    // value type
    using value_type = ValueType<Graph>;
    // descriptor type
    using descriptor_type = const value_type *;
    // id type
    using id_type = typename Graph::id_type;

    /// Internal container type for graph
    struct container_type
    {
      // descriptor map.
      std::map<descriptor_type, id_type> dsc_map;
      // id map.
      std::map<id_type, value_type> id_map;
    };

    /// Check if a descriptor exists in a container.
    /// \param descriptor descriptor
    /// \param return true when descriptor is valid and exists in the container
    bool exists(const descriptor_type &descriptor) const noexcept
    {
      auto &c   = m_container;
      auto &map = c.dsc_map;

      auto iter = map.find(descriptor);

      if (iter != map.end())
        return true;

      return false;
    }

    /// Check if an Id exists in a container.
    /// \param id id
    /// \param return true when the id exists in the container
    bool exists(const id_type &id) const noexcept
    {
      auto &c   = m_container;
      auto &map = c.id_map;

      auto iter = map.find(id);

      if (iter != map.end())
        return true;

      return false;
    }
    /// Find descriptor from ID value.
    /// \param id id
    /// \returns nullptr when not found
    auto find_descriptor(const id_type &id) const noexcept -> descriptor_type
    {
      auto &c   = m_container;
      auto &map = c.id_map;

      auto iter = map.find(id);

      if (iter != map.end())
        return &iter->second;

      return nullptr;
    }

    /// Destroy instance from a container.
    /// Descriptor will become invalid after calling this
    /// function.
    /// \param id Id
    /// \param descriptor Descriptor
    void destroy(const descriptor_type &descriptor, const id_type &id) noexcept
    {
      auto &c = m_container;
      // remove id
      {
        auto &map = c.id_map;

        auto iter = map.find(id);

        if (iter != map.end())
          map.erase(iter);
      }

      // remove descriptor
      {
        auto &map = c.dsc_map;

        auto iter = map.find(descriptor);

        if (iter != map.end())
          map.erase(iter);
      }
    }

    /// Instance in a container and return descriptor.
    /// \param id Id of new value
    /// \param args Args for initialize value type
    template <class... Args>
    auto create(const id_type &id, Args &&... args) -> descriptor_type
    {
      auto &c = m_container;

      descriptor_type dsc;
      {
        // insert id
        auto [iter, succ] =
          c.id_map.emplace(id, value_type(id, std::forward<Args>(args)...));
        dsc = succ ? &iter->second : nullptr;
      }

      // duplicated id
      if (!dsc)
        return nullptr;

      // insert dsc
      c.dsc_map.emplace(dsc, id);

      return dsc;
    }

    /// Get list of descriptors.
    auto descriptors() const
    {
      auto &c = m_container;

      std::vector<descriptor_type> ret;
      ret.reserve(c.dsc_map.size());

      for (auto &&pair : c.dsc_map) {
        ret.push_back(pair.first);
      }

      return ret;
    }

    /// Get list of IDs.
    /// \param c Container
    auto ids() const
    {
      auto &c = m_container;

      std::vector<id_type> ret;
      ret.reserve(c.id_map.size());

      for (auto &&pair : c.id_map) {
        ret.push_back(pair.first);
      }

      return ret;
    }

    /// Get number of elements.
    auto size() const noexcept
    {
      auto &c = m_container;
      assert(c.dsc_map.size() == c.id_map.size());
      return c.dsc_map.size();
    }

    /// Access function.
    /// \param descriptor A valid descriptor.
    /// \returns reference of value type in the container.
    auto &access(const descriptor_type &descriptor) noexcept
    {
      assert(descriptor);
      return *const_cast<value_type *>(descriptor);
    }

    /// Access function.
    /// \param descriptor A valid descriptor.
    /// \returns reference of value type in the container.
    auto &access(const descriptor_type &descriptor) const noexcept
    {
      assert(descriptor);
      return *descriptor;
    }

    /// Access function.
    /// \param descriptor descriptor
    /// \throws std::out_of_range when given descriptor is invalid or nonexist
    /// in the container.
    /// \returns reference of value type.
    auto &at(const descriptor_type &descriptor)
    {
      if (!exists(descriptor))
        throw std::out_of_range("descriptor does not exist.");
      return access(descriptor);
    }

    /// Access function.
    ///  reference of value type.
    /// \param descriptor Descriptor
    /// \throws std::out_of_range when given descriptor is invalid or nonexist
    /// in the container.
    auto &at(const descriptor_type &descriptor) const
    {
      if (!exists(descriptor))
        throw std::out_of_range("descriptor does not exist.");
      return access(descriptor);
    }

  private:
    // container
    container_type m_container;
  };

  /// \brief Node class.
  /// This class repsesents Node object in Graph contains descriptors of sockets
  /// which have link to this object.
  template <class Graph>
  class node
  {
  public:
    /// Type of this class
    using type = node<Graph>;
    /// Property type
    using property_type = typename Graph::node_property_type;
    /// Descriptor type
    using descriptor_type = typename Graph::node_descriptor_type;
    /// Socket descriptor type
    using socket_descriptor_type = typename Graph::socket_descriptor_type;
    /// ID type
    using id_type = typename Graph::id_type;

    /// A Constructor.
    /// \param id Unique id for this node.
    /// \param args Arguments to initialize property class.
    template <class... Args>
    node(const id_type &id, Args &&... args)
      : m_id {id}
      , m_sockets {}
      , m_property {std::forward<Args>(args)...}
    {
    }

    /// A destructor.
    ~node() noexcept
    {
    }

    /// Get id.
    [[nodiscard]] auto id() const noexcept
    {
      return m_id;
    }

    /// Get sockets.
    [[nodiscard]] auto &sockets() const
    {
      return m_sockets;
    }

    /// Get socket count.
    [[nodiscard]] auto n_sockets() const
    {
      return m_sockets.size();
    }

    /// Property accessor.
    [[nodiscard]] auto &property() const
    {
      return m_property;
    }

    /// Property accessor
    [[nodiscard]] auto &property()
    {
      return m_property;
    }

    /// Set socket descriptor.
    /// \param dsc descriptor of target socket
    void set_socket(const socket_descriptor_type &dsc)
    {
      assert(dsc);

      if (std::find(m_sockets.begin(), m_sockets.end(), dsc) != m_sockets.end())
        return;

      m_sockets.push_back(dsc); // set socket
    }

    /// Remove socket.
    /// \param dsc descriptor of socket
    void unset_socket(const socket_descriptor_type &dsc)
    {
      assert(dsc);
      auto e = std::remove(m_sockets.begin(), m_sockets.end(), dsc);
      m_sockets.erase(e, m_sockets.end());
    }

  private:
    /// Id of this node.
    id_type m_id;
    /// Sockets attached to this node.
    /// Shouldn't contain 2 or more same sockets.
    std::basic_string<socket_descriptor_type> m_sockets;
    /// Property class instance
    property_type m_property;
  };

  /// \brief Edge class.
  /// this class represents single directed edge object in NodeGprah contains
  /// src,dst descriptors of socket. src and dst can contain same descriptor.
  /// constructed by 2 descriptors and should automatically be deleted when they
  /// deleted from Graph.
  template <typename Graph>
  class edge
  {
  public:
    /// Type
    using type = edge<Graph>;
    /// Property type
    using property_type = typename Graph::edge_property_type;
    /// Descriptor type
    using descriptor_type = typename Graph::edge_descriptor_type;
    /// Socket descriptor type
    using socket_descriptor_type = typename Graph::socket_descriptor_type;
    /// ID type
    using id_type = typename Graph::id_type;

    /// A constructor.
    /// \param id Unique id for this edge.
    /// \param src descriptor of source socket.
    /// \param dst descriptor of destination socket.
    /// \param args args to initialize property class instance.
    template <class... Args>
    edge(
      const id_type &id,
      const socket_descriptor_type &src,
      const socket_descriptor_type &dst,
      Args &&... args)
      : m_id {id}
      , m_src {src}
      , m_dst {dst}
      , m_property {std::forward<Args>(args)...}
    {
    }

    /// A destructor.
    ~edge() noexcept
    {
    }

    /// Get id.
    [[nodiscard]] auto id() const noexcept
    {
      return m_id;
    }

    /// Get source socket.
    [[nodiscard]] auto src() const
    {
      return m_src;
    }

    /// Get destination socket.
    [[nodiscard]] auto dst() const
    {
      return m_dst;
    }

    /// Property accessor.
    [[nodiscard]] auto &property() const
    {
      return m_property;
    }

    /// Pproperty accessor.
    [[nodiscard]] auto &property()
    {
      return m_property;
    }

  private:
    /// Id of this socket.
    id_type m_id;
    /// socket descriptors
    socket_descriptor_type m_src, m_dst;
    /// Property class instance
    property_type m_property;
  };

  /// \brief Socket class.
  /// This class represents socket object in Graph.
  /// contains list of Node descriptors and list of Edge descriptors.
  template <typename Graph>
  class socket
  {
  public:
    /// Type
    using type = socket<Graph>;
    /// Property type
    using property_type = typename Graph::socket_property_type;
    /// Descriptor type
    using descriptor_type = typename Graph::socket_descriptor_type;
    /// Node descriptor type
    using node_descriptor_type = typename Graph::node_descriptor_type;
    /// Edge descriptor type
    using edge_descriptor_type = typename Graph::edge_descriptor_type;
    /// ID type
    using id_type = typename Graph::id_type;

    /// A constructor.
    /// \param id Unique id for this socket.
    /// \param args Args for initialize property class.
    template <class... Args>
    socket(const id_type &id, Args &&... args)
      : m_id {id}
      , m_nodes {}
      , m_src_edges {}
      , m_dst_edges {}
      , m_property {std::forward<Args>(args)...}
    {
    }

    /// A destructor
    ~socket() noexcept
    {
    }

    /// Get id.
    [[nodiscard]] auto id() const noexcept
    {
      return m_id;
    }

    /// Get nodes.
    [[nodiscard]] auto &nodes() const
    {
      return m_nodes;
    }

    /// Get source edges.
    [[nodiscard]] auto &src_edges() const
    {
      return m_src_edges;
    }
    /// Get destination edges.
    [[nodiscard]] auto &dst_edges() const
    {
      return m_dst_edges;
    }

    /// Get node count.
    [[nodiscard]] auto n_nodes() const
    {
      return m_nodes.size();
    }

    /// Get srouce edge count.
    [[nodiscard]] auto n_src_edges() const
    {
      return m_src_edges.size();
    }

    /// Get destination edge count.
    [[nodiscard]] auto n_dst_edges() const
    {
      return m_dst_edges.size();
    }

    /// Property accessor.
    [[nodiscard]] auto &property() const
    {
      return m_property;
    }

    /// Property accessor.
    [[nodiscard]] auto &property()
    {
      return m_property;
    }

    /// Set node.
    /// \param dsc descriptor of node
    void set_node(const node_descriptor_type &dsc)
    {
      assert(dsc);

      if (std::find(m_nodes.begin(), m_nodes.end(), dsc) != m_nodes.end())
        return;

      m_nodes.push_back(dsc);
    }

    /// Set src edge.
    /// \param dsc descriptor of edge
    void set_src_edge(const edge_descriptor_type &dsc)
    {
      assert(dsc);

      if (
        std::find(m_src_edges.begin(), m_src_edges.end(), dsc)
        != m_src_edges.end())
        return;

      m_src_edges.push_back(dsc);
    }

    /// Set dst edge.
    /// \param dsc descriptor of edge
    void set_dst_edge(const edge_descriptor_type &dsc)
    {
      assert(dsc);

      if (
        std::find(m_dst_edges.begin(), m_dst_edges.end(), dsc)
        != m_dst_edges.end())
        return;

      m_dst_edges.push_back(dsc);
    }

    /// Unset node.
    /// \param dsc descriptor of node
    void unset_node(const node_descriptor_type &dsc)
    {
      auto e = std::remove(m_nodes.begin(), m_nodes.end(), dsc);
      m_nodes.erase(e, m_nodes.end());
    }

    /// Unset edge.
    /// \param dsc descriptor of edge
    void unset_src_edge(const edge_descriptor_type &dsc)
    {
      auto e = std::remove(m_src_edges.begin(), m_src_edges.end(), dsc);
      m_src_edges.erase(e, m_src_edges.end());
    }

    /// Unset edge
    /// \param dsc descriptor of edge
    void unset_dst_edge(const edge_descriptor_type &dsc)
    {
      auto e = std::remove(m_dst_edges.begin(), m_dst_edges.end(), dsc);
      m_dst_edges.erase(e, m_dst_edges.end());
    }

  private:
    /// Id of this edge.
    id_type m_id;
    /// node descriptors
    std::basic_string<node_descriptor_type> m_nodes;
    /// edge descriptors
    std::basic_string<edge_descriptor_type> m_src_edges;
    /// edge descriptors
    std::basic_string<edge_descriptor_type> m_dst_edges;
    /// property class instance
    property_type m_property;
  };

  struct empty_graph_property
  {
  };

  /// \brief Node Graph class.
  /// This class represents Node Graph Object.
  template <
    class NodeProperty   = empty_graph_property,
    class SocketProperty = empty_graph_property,
    class EdgeProperty   = empty_graph_property>
  class graph
  {
  public:
    // clang-format off

    using type    = graph<NodeProperty, SocketProperty, EdgeProperty>;

    using id_generator           = graph_id_generator;
    using id_type                = typename id_generator::id_type;

    using node_property_type     = NodeProperty;
    using socket_property_type   = SocketProperty;
    using edge_property_type     = EdgeProperty;

    using node_type              = node<type>;
    using socket_type            = socket<type>;
    using edge_type              = edge<type>;

    using node_container_type    = graph_container<node, type>;
    using socket_container_type  = graph_container<socket, type>;
    using edge_container_type    = graph_container<edge, type>;

    using node_descriptor_type   = typename node_container_type::descriptor_type;   
    using socket_descriptor_type = typename socket_container_type::descriptor_type; 
    using edge_descriptor_type   = typename edge_container_type::descriptor_type;

    // clang-format on

    /// Constructor
    graph()
    {
    }

    /// Destructor
    ~graph() noexcept
    {
      clear();
    }

    /// Use copy functions to copy instances of Graph
    graph(const graph &) = delete;

    /// Move constructor
    graph(graph &&other) = default;

    /// Copy assignment is deleted. use clone().
    graph &operator=(const graph &) = delete;

    /// Move assignment.
    graph &operator=(graph &&other) = default;

    /// Add node with id.
    /// \param id Unique ID of new node.
    /// \param args Args to initialize property.
    /// \returns descriptor of new node.
    template <class... Args>
    [[nodiscard]] auto add_node_with_id(const id_type &id, Args &&... args)
      -> node_descriptor_type
    {
      return _create_n(id, std::forward<Args>(args)...);
    }

    /// Add node.
    /// \param args Args to initialize property.
    /// \returns descriptor of new node.
    template <class... Args>
    [[nodiscard]] auto add_node(Args &&... args) -> node_descriptor_type
    {
      return add_node_with_id(_unique_id_gen(), std::forward<Args>(args)...);
    }

    /// Add socket with id.
    /// \param id Unique ID for new socket.
    /// \param args Args to initialize property.
    /// \returns descriptor of new socket.
    template <class... Args>
    [[nodiscard]] auto add_socket_with_id(const id_type &id, Args &&... args)
      -> socket_descriptor_type
    {
      return _create_s(id, std::forward<Args>(args)...);
    }

    /// Add socket.
    /// \param args Args to initialize property
    /// \returns descriptor of new socket
    template <class... Args>
    [[nodiscard]] auto add_socket(Args &&... args) -> socket_descriptor_type
    {
      return add_socket_with_id(_unique_id_gen(), std::forward<Args>(args)...);
    }

    /// Add edge.
    /// \param src source socket.
    /// \param dst destination socket.
    /// \param Unique ID for new edge.
    /// \param args Args to initialize property class.
    template <class... Args>
    [[nodiscard]] auto add_edge_with_id(
      const socket_descriptor_type &src,
      const socket_descriptor_type &dst,
      const id_type &id,
      Args &&... args) -> edge_descriptor_type
    {
      assert(exists(src) && exists(dst));

      for (auto &&e : _access(src).src_edges())
        if (_access(e).dst() == dst)
          return e;

      for (auto &&e : _access(dst).dst_edges())
        if (_access(e).src() == src)
          return e;

      auto e = _create_e(id, src, dst, std::forward<Args>(args)...);

      if (!e)
        return nullptr;

      // set edge to sockets
      _access(src).set_src_edge(e);
      _access(dst).set_dst_edge(e);

      return e;
    }

    /// Add edge.
    /// \param src source socket.
    /// \param dst destination socket.
    /// \param args Args to initialize property class.
    template <class... Args>
    [[nodiscard]] auto add_edge(
      const socket_descriptor_type &src,
      const socket_descriptor_type &dst,
      Args &&... args) -> edge_descriptor_type
    {
      return add_edge_with_id(
        src, dst, _unique_id_gen(), std::forward<Args>(args)...);
    }

    /// Remove node.
    /// \param descriptor valid descriptor of node
    void remove_node(const node_descriptor_type &descriptor)
    {
      assert(exists(descriptor));

      // delete links from sockets
      for (auto &&s : _access(descriptor).sockets())
        _access(s).unset_node(descriptor);

      _destroy(descriptor);
      return;
    }

    /// \brief Remove edge.
    /// \param descriptor valid descriptor of edge
    void remove_edge(const edge_descriptor_type &descriptor)
    {
      assert(exists(descriptor));

      auto src = _access(descriptor).src();
      auto dst = _access(descriptor).dst();

      // should remove more than one edges.
      _access(src).unset_src_edge(descriptor);
      _access(dst).unset_dst_edge(descriptor);

      _destroy(descriptor);
      return;
    }

    /// Remove socket
    /// Removes all of edges connected from the socket.
    /// \param descriptor valid descriptor of socket
    void remove_socket(const socket_descriptor_type &descriptor)
    {
      assert(exists(descriptor));

      for (auto &&e : _access(descriptor).src_edges()) {
        auto dst = _access(e).dst();
        _access(dst).unset_dst_edge(e);
        _destroy(e);
      }
      for (auto &&e : _access(descriptor).dst_edges()) {
        auto src = _access(e).src();
        _access(src).unset_src_edge(e);
        _destroy(e);
      }

      for (auto &&n : _access(descriptor).nodes())
        _access(n).unset_socket(descriptor); // remove all links from node

      _destroy(descriptor);
      return;
    }

    /// Check if node descriptor exists.
    /// \param descriptor descriptor of node
    /// \returns true if success
    [[nodiscard]] bool exists(const node_descriptor_type &descriptor) const
    {
      return m_nodes.exists(descriptor);
    }

    /// Check if edge descriptor exists
    /// \param descriptor descriptor of edge
    /// \returns true if success
    [[nodiscard]] bool exists(const edge_descriptor_type &descriptor) const
    {
      return m_edges.exists(descriptor);
    }

    /// Check if socket descriptor exists
    /// \param descriptor descriptor of socket
    /// \returns true if success
    [[nodiscard]] bool exists(const socket_descriptor_type &descriptor) const
    {
      return m_sockets.exists(descriptor);
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of node
    /// \returns if success
    [[nodiscard]] auto operator[](const node_descriptor_type &descriptor)
      -> node_property_type &
    {
      return _access(descriptor).property();
    }
    /// Descriptor access operator
    /// \param descriptor descriptor of node
    /// \returns if descriptor is invalid, return value is undefined.
    [[nodiscard]] auto operator[](const node_descriptor_type &descriptor) const
      -> const node_property_type &
    {
      return _access(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of edge
    /// \returns if success
    [[nodiscard]] auto operator[](const edge_descriptor_type &descriptor)
      -> edge_property_type &
    {
      return _access(descriptor).property();
    }
    /// Descriptor access operator.
    /// \param descriptor descriptor of edge
    /// \returns if success
    [[nodiscard]] auto operator[](const edge_descriptor_type &descriptor) const
      -> const edge_property_type &
    {
      return _access(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    [[nodiscard]] auto operator[](const socket_descriptor_type &descriptor)
      -> socket_property_type &
    {
      return _access(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    [[nodiscard]] auto operator[](const socket_descriptor_type &descriptor)
      const -> const socket_property_type &
    {
      return _access(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of node
    /// \returns if success
    [[nodiscard]] auto at(const node_descriptor_type &descriptor)
      -> node_property_type &
    {
      return _at(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of node
    /// \returns if descriptor is invalid, return value is undefined.
    [[nodiscard]] auto at(const node_descriptor_type &descriptor) const
      -> const node_property_type &
    {
      return _at(descriptor).property();
    }

    /// Descriptor access operator
    /// \param descriptor descriptor of edge
    /// \returns if success
    [[nodiscard]] auto at(const edge_descriptor_type &descriptor)
      -> edge_property_type &
    {
      return _at(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of edge
    /// \returns if success
    [[nodiscard]] auto at(const edge_descriptor_type &descriptor) const
      -> const edge_property_type &
    {
      return _at(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    [[nodiscard]] auto at(const socket_descriptor_type &descriptor)
      -> socket_property_type &
    {
      return _at(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    [[nodiscard]] auto at(const socket_descriptor_type &descriptor) const
      -> const socket_property_type &
    {
      return _at(descriptor).property();
    }

    /// Attach socket.
    /// \param node valid node descriptor
    /// \param socket valid socket descriptor
    [[nodiscard]] bool attach_socket(
      const node_descriptor_type &node,
      const socket_descriptor_type &socket)
    {
      assert(exists(node) && exists(socket));

      _access(socket).set_node(node);
      _access(node).set_socket(socket);

      return true;
    }

    /// Detach socket.
    /// \param node valid node descriptor
    /// \param socket valid socket descriptor
    void detach_socket(
      const node_descriptor_type &node,
      const socket_descriptor_type &socket)
    {
      assert(exists(node) && exists(socket));

      for (auto &&n : _access(socket).nodes()) {
        if (n == node) {
          // delete link
          _access(socket).unset_node(node);
          _access(node).unset_socket(socket);
          return;
        }
      }
    }

    /// Get nodes.
    [[nodiscard]] auto nodes() const -> std::vector<node_descriptor_type>
    {
      return _nodes();
    }

    /// Get node count.
    [[nodiscard]] auto n_nodes() const -> size_t
    {
      return _n_nodes();
    }

    /// Get nodes connected to the socket.
    [[nodiscard]] auto nodes(const socket_descriptor_type &descriptor) const
      -> std::vector<node_descriptor_type>
    {
      auto &ns = _at(descriptor).nodes();
      return {ns.begin(), ns.end()};
    }

    /// Get number of connected nodes to the socket.
    [[nodiscard]] auto n_nodes(const socket_descriptor_type &descriptor) const
      -> size_t
    {
      return _at(descriptor).n_nodes();
    }

    /// Get sockets.
    [[nodiscard]] auto sockets() const -> std::vector<socket_descriptor_type>
    {
      return _sockets();
    }

    /// Get socket count.
    [[nodiscard]] auto n_sockets() const -> size_t
    {
      return _n_sockets();
    }

    /// Get sockets connected to the node.
    [[nodiscard]] auto sockets(const node_descriptor_type &descriptor) const
      -> std::vector<socket_descriptor_type>
    {
      auto &ss = _at(descriptor).sockets();
      return {ss.begin(), ss.end()};
    }

    /// Get number of connected sockets to the node.
    [[nodiscard]] auto n_sockets(const node_descriptor_type &descriptor) const
      -> size_t
    {
      return _at(descriptor).n_sockets();
    }

    /// Get edges.
    [[nodiscard]] auto edges() const -> std::vector<edge_descriptor_type>
    {
      return _edges();
    }

    /// Get edge count.
    [[nodiscard]] auto n_edges() const -> size_t
    {
      return _n_edges();
    }

    /// Get src edges.
    [[nodiscard]] auto src_edges(const socket_descriptor_type &descriptor) const
      -> std::vector<edge_descriptor_type>
    {
      auto &es = _at(descriptor).src_edges();
      return {es.begin(), es.end()};
    }

    /// Get number of src edges.
    [[nodiscard]] auto n_src_edges(
      const socket_descriptor_type &descriptor) const -> size_t
    {
      return _at(descriptor).n_src_edges();
    }

    /// Get dst edges.
    [[nodiscard]] auto dst_edges(const socket_descriptor_type &descriptor) const
      -> std::vector<edge_descriptor_type>
    {
      auto &es = _at(descriptor).dst_edges();
      return {es.begin(), es.end()};
    }

    /// Get number of dst edges.
    [[nodiscard]] auto n_dst_edges(
      const socket_descriptor_type &descriptor) const -> size_t
    {
      return _at(descriptor).n_dst_edges();
    }

    /// Get src socket.
    [[nodiscard]] auto src(const edge_descriptor_type &descriptor) const
      -> socket_descriptor_type
    {
      return _at(descriptor).src();
    }

    /// Get dst socket.
    [[nodiscard]] auto dst(const edge_descriptor_type &descriptor) const
      -> socket_descriptor_type
    {
      return _at(descriptor).dst();
    }

    /// Get id of node.
    [[nodiscard]] auto id(const node_descriptor_type &descriptor) const
      -> id_type
    {
      return _at(descriptor).id();
    }

    /// Get id of socket.
    [[nodiscard]] auto id(const socket_descriptor_type &descriptor) const
      -> id_type
    {
      return _at(descriptor).id();
    }

    /// Get id of edge.
    [[nodiscard]] auto id(const edge_descriptor_type &descriptor) const
      -> id_type
    {
      return _at(descriptor).id();
    }

    /// Find node from id.
    [[nodiscard]] auto node(const id_type &id) const -> node_descriptor_type
    {
      return m_nodes.find_descriptor(id);
    }

    /// Get socket from id.
    [[nodiscard]] auto socket(const id_type &id) const -> socket_descriptor_type
    {
      return m_sockets.find_descriptor(id);
    }

    /// Get edge from id.
    [[nodiscard]] auto edge(const id_type &id) const -> edge_descriptor_type
    {
      return m_edges.find_descriptor(id);
    }

    /// Clone graph
    [[nodiscard]] auto clone() const -> graph
    {
      // ret
      graph g;

      // map
      std::vector<std::pair<node_descriptor_type, node_descriptor_type>>
        ndsc_map;
      std::vector<std::pair<socket_descriptor_type, socket_descriptor_type>>
        sdsc_map;
      std::vector<std::pair<edge_descriptor_type, edge_descriptor_type>>
        edsc_map;

      ndsc_map.reserve(n_nodes());
      sdsc_map.reserve(n_sockets());
      edsc_map.reserve(n_edges());

      auto _pcomp = [](const auto &l, const auto &r) {
        return l.first < r.first;
      };

      auto _find_dsc = [_pcomp](const auto &dsc, const auto &map) {
        auto lb = std::lower_bound(
          map.begin(), map.end(), std::pair {dsc, dsc}, _pcomp);
        assert(lb != map.end());
        assert(lb->first == dsc);
        return lb->second;
      };

      // sockets
      for (auto &&s : sockets()) {
        // copy socket
        auto dsc = g._create_s(_access(s).id(), _access(s).property());
        sdsc_map.emplace_back(s, dsc);
      }
      std::sort(sdsc_map.begin(), sdsc_map.end(), _pcomp);

      // nodes
      for (auto &&n : nodes()) {
        // copy nodes
        auto dsc = g._create_n(_access(n).id(), _access(n).property());
        ndsc_map.emplace_back(n, dsc);
        // set sockets
        for (auto &&s : _access(n).sockets())
          g._access(dsc).set_socket(_find_dsc(s, sdsc_map));
      }
      std::sort(ndsc_map.begin(), ndsc_map.end(), _pcomp);

      // edges
      for (auto &&e : edges()) {
        // copy edges
        auto s   = _find_dsc(_access(e).src(), sdsc_map);
        auto d   = _find_dsc(_access(e).dst(), sdsc_map);
        auto dsc = g._create_e(_access(e).id(), s, d, _access(e).property());
        edsc_map.emplace_back(e, dsc);
      }
      std::sort(edsc_map.begin(), edsc_map.end(), _pcomp);

      for (auto &&s : sockets()) {
        auto dsc = _find_dsc(s, sdsc_map);
        // set nodes
        for (auto &&n : _access(s).nodes())
          g._access(dsc).set_node(_find_dsc(n, ndsc_map));
        // set src edges
        for (auto &&e : _access(s).src_edges()) {
          g._access(dsc).set_src_edge(_find_dsc(e, edsc_map));
        }
        // set dst edges
        for (auto &&e : _access(s).dst_edges()) {
          g._access(dsc).set_dst_edge(_find_dsc(e, edsc_map));
        }
      }

      assert(g.n_nodes() == n_nodes());
      assert(g.n_sockets() == n_sockets());
      assert(g.n_edges() == n_edges());

      return g;
    }

    /// Clear graph
    void clear()
    {
      for (auto &&e : edges())
        _destroy(e);
      for (auto &&n : nodes())
        _destroy(n);
      for (auto &&s : sockets())
        _destroy(s);
      assert(empty());
    }

    /// Empty
    [[nodiscard]] bool empty() const
    {
      return (_n_nodes() == 0 && _n_sockets() == 0 && _n_edges() == 0);
    }

  private:
    /// Create new node.
    /// \param id id for new node
    /// \param args arguments to construct node property
    template <class... Args>
    inline auto _create_n(const id_type &id, Args &&... args)
    {
      return m_nodes.create(id, std::forward<Args>(args)...);
    }

    /// Create new edge.
    /// \param id id for new edge.
    /// \param args arguments to construct node property.
    template <class... Args>
    inline auto _create_e(const id_type &id, Args &&... args)
    {
      return m_edges.create(id, std::forward<Args>(args)...);
    }

    /// Create new socket.
    /// \param id id for new socket.
    /// \param args arguments to construct node property
    template <class... Args>
    inline auto _create_s(const id_type &id, Args &&... args)
    {
      return m_sockets.create(id, std::forward<Args>(args)...);
    }

    /// Create random id for node/socket/edge.
    auto _unique_id_gen()
    {
      return m_idgen.generate();
    }

    /// Get list of node descriptors.
    inline auto _nodes() const
    {
      return m_nodes.descriptors();
    }

    /// Get size of nodes.
    inline auto _n_nodes() const
    {
      return m_nodes.size();
    }

    /// Get list of socket descriptors.
    inline auto _sockets() const
    {
      return m_sockets.descriptors();
    }

    /// Get size of sockets.
    inline auto _n_sockets() const
    {
      return m_sockets.size();
    }

    /// Get list of edges descriptors.
    inline auto _edges() const
    {
      return m_edges.descriptors();
    }

    /// Get size of edges.
    inline auto _n_edges() const
    {
      return m_edges.size();
    }

    /// Node access.
    /// \param n if nonexist or invalid, behaviour of this function is undefined
    inline auto &_access(const node_descriptor_type &n) noexcept
    {
      return m_nodes.access(n);
    }

    inline auto &_access(const node_descriptor_type &n) const noexcept
    {
      return m_nodes.access(n);
    }

    /// Edge access.
    /// \param n if nonexist or invalid, behaviour of this function is undefined
    inline auto &_access(const edge_descriptor_type &e) noexcept
    {
      return m_edges.access(e);
    }

    inline auto &_access(const edge_descriptor_type &e) const noexcept
    {
      return m_edges.access(e);
    }

    /// Socket access.
    /// \param n if nonexist or invalid, behaviour of this function is undefined
    inline auto &_access(const socket_descriptor_type &s) noexcept
    {
      return m_sockets.access(s);
    }

    inline auto &_access(const socket_descriptor_type &s) const noexcept
    {
      return m_sockets.access(s);
    }

    /// Range checked node access.
    /// \param descriptor descriptor
    /// \returns reference
    inline auto &_at(const node_descriptor_type &descriptor)
    {
      return m_nodes.at(descriptor);
    }

    inline auto &_at(const node_descriptor_type &descriptor) const
    {
      return m_nodes.at(descriptor);
    }

    /// Range checked Edge access
    /// \param descriptor descriptor
    /// \returns reference
    inline auto &_at(const edge_descriptor_type &descriptor)
    {
      return m_edges.at(descriptor);
    }

    inline auto &_at(const edge_descriptor_type &descriptor) const
    {
      return m_edges.at(descriptor);
    }

    /// Range checked Socket access.
    /// \param descriptor descriptor
    /// \returns reference
    inline auto &_at(const socket_descriptor_type &descriptor)
    {
      return m_sockets.at(descriptor);
    }

    inline auto &_at(const socket_descriptor_type &descriptor) const
    {
      return m_sockets.at(descriptor);
    }

    /// Destroy node.
    /// \param descriptor descriptor of node
    /// \returns true if success
    void _destroy(const node_descriptor_type &descriptor) noexcept
    {
      m_nodes.destroy(descriptor, _access(descriptor).id());
    }

    /// Destroy edge.
    /// \param descriptor descriptor of edge
    /// \returns true if success
    void _destroy(const edge_descriptor_type &descriptor) noexcept
    {
      m_edges.destroy(descriptor, _access(descriptor).id());
    }

    /// Destroy edge.
    /// \param descriptor descriptor of socket
    /// \returns true if success
    void _destroy(const socket_descriptor_type &descriptor) noexcept
    {
      m_sockets.destroy(descriptor, _access(descriptor).id());
    }

    /// id generator
    [[no_unique_address]] //
    id_generator m_idgen;
    /// list of node descriptors
    node_container_type m_nodes;
    /// list of edge descriptors
    edge_container_type m_edges;
    /// list of socket descriptors
    socket_container_type m_sockets;
  };

} // namespace yave::graph