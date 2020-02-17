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

  template <typename Traits>
  class node;

  template <typename Traits>
  class edge;

  template <typename Traits>
  class socket;

  template <
    class NodeProperty,
    class SocketProperty,
    class EdgeProperty,
    class Tag>
  class graph;

  /// empty class
  struct empty_graph_property
  {
  };

  /// default trait tag
  struct default_graph_trait_tag;

  /// container_traits tells Graph how to manage Nodes,Edges,Sockets.
  /// Default implementation is specialized to std::vector as container, pointer
  /// type as descriptor.
  template <
    template <class>
    class ValueType,
    class GraphTraits,
    class Tag = default_graph_trait_tag>
  struct graph_container_traits;

  template <template <class> class ValueType, class GraphTraits>
  struct graph_container_traits<ValueType, GraphTraits, default_graph_trait_tag>
  {
    // value type
    using value_type = ValueType<GraphTraits>;
    // descriptor type
    using descriptor_type = const value_type *;
    // id type
    using id_type = typename GraphTraits::id_type;

    /// Internal container type for graph
    struct container_type
    {
      // descriptor map.
      std::map<descriptor_type, id_type> dsc_map;
      // id map.
      std::map<id_type, value_type> id_map;
    };

    /// Check if a descriptor exists in a container.
    /// \param c Container
    /// \param descriptor descriptor
    /// \param return true when descriptor is valid and exists in the container
    static inline bool exists(
      const container_type &c,
      const descriptor_type &descriptor)
    {
      auto &map = c.dsc_map;

      auto iter = map.find(descriptor);

      if (iter != map.end())
        return true;

      return false;
    }

    /// Check if an Id exists in a container.
    /// \param c Container
    /// \param id id
    /// \param return true when the id exists in the container
    static inline bool exists(const container_type &c, const id_type &id)
    {
      auto &map = c.id_map;

      auto iter = map.find(id);

      if (iter != map.end())
        return true;

      return false;
    }

    /// Find descriptor from ID value.
    /// \param c Container
    /// \param id id
    /// \returns nullptr when not found
    static inline descriptor_type find_descriptor(
      const container_type &c,
      const id_type &id)
    {
      auto &map = c.id_map;

      auto iter = map.find(id);

      if (iter != map.end())
        return &iter->second;

      return nullptr;
    }

    /// Destroy instance from a container.
    /// Descriptor will become invalid after calling this
    /// function.
    /// \param c Container
    /// \param id Id
    /// \param descriptor Descriptor
    static inline void destroy(
      container_type &c,
      const descriptor_type &descriptor,
      const id_type &id) noexcept
    {
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
    /// \param c Container
    /// \param id Id of new value
    /// \param args Args for initialize value type
    template <class... Args>
    static inline descriptor_type create(
      container_type &c,
      const id_type &id,
      Args &&... args)
    {
      descriptor_type dsc;

      // insert id
      {
        auto &map = c.id_map;
        auto [iter, succ] =
          map.emplace(id, value_type(id, std::forward<Args>(args)...));
        assert(succ);
        dsc = &iter->second;
      }
      // insert dsc
      {
        auto &map = c.dsc_map;
        map.emplace(dsc, id);
      }

      return dsc;
    }

    /// Get list of descriptors.
    /// \param c Container
    static inline std::vector<descriptor_type> descriptors(
      const container_type &c)
    {
      std::vector<descriptor_type> ret;

      for (auto &&pair : c.dsc_map) {
        ret.push_back(pair.first);
      }

      return ret;
    }

    /// Get list of IDs.
    /// \param c Container
    static inline std::vector<id_type> ids(const container_type &c)
    {
      std::vector<id_type> ret;

      for (auto &&pair : c.id_map) {
        ret.push_back(pair.first);
      }

      return ret;
    }

    /// Get number of elements.
    /// \param c Container
    static inline size_t size(const container_type &c)
    {
      assert(c.dsc_map.size() == c.id_map.size());
      return c.dsc_map.size();
    }

    /// Access function.
    /// \param c Container.
    /// \param descriptor A valid descriptor.
    /// \returns reference of value type in the container.
    static inline value_type &access(
      container_type &c,
      const descriptor_type &descriptor) noexcept
    {
      (void)c;
      assert(descriptor);
      return *const_cast<value_type *>(descriptor);
    }

    /// Access function.
    /// \param c Container.
    /// \param descriptor A valid descriptor.
    /// \returns reference of value type in the container.
    static inline const value_type &access(
      const container_type &c,
      const descriptor_type &descriptor) noexcept
    {
      (void)c;
      assert(descriptor);
      return *descriptor;
    }

    /// Access function.
    /// \param c Container
    /// \param descriptor descriptor
    /// \throws std::out_of_range when given descriptor is invalid or nonexist
    /// in the container.
    /// \returns reference of value type.
    static inline value_type &at(
      container_type &c,
      const descriptor_type &descriptor)
    {
      if (!exists(c, descriptor))
        throw std::out_of_range("descriptor does not exist.");
      return access(c, descriptor);
    }

    /// Access function.
    ///  reference of value type.
    /// \param c Container
    /// \param descriptor Descriptor
    /// \throws std::out_of_range when given descriptor is invalid or nonexist
    /// in the container.
    static inline const value_type &at(
      const container_type &c,
      const descriptor_type &descriptor)
    {
      if (!exists(c, descriptor))
        throw std::out_of_range("descriptor does not exist.");
      return access(c, descriptor);
    }
  };

  /// \brief trait class for defining ID type for node/socket/edge.
  template <class Node, class Edge, class Socket>
  struct graph_id_traits
  {
    /// id type
    using id_type = uint64_t;

    /// generate random ID for graph objects.
    static id_type random_generate()
    {
      static auto mt = std::mt19937_64(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
      return id_type(mt());
    }
  };

  /// \brief graph_traits provides all of typedefs used in Graph class.
  /// Property types can only be customized through this class.
  template <
    class NodeProperty,
    class SocketProperty,
    class EedgeProperty,
    class Tag>
  struct graph_traits;

  template <class NodeProperty, class SocketProperty, class EdgeProperty>
  struct graph_traits<
    NodeProperty,
    SocketProperty,
    EdgeProperty,
    default_graph_trait_tag>
  {
    /// type
    using type = graph_traits<
      NodeProperty,
      SocketProperty,
      EdgeProperty,
      default_graph_trait_tag>;

    /// graph type
    using graph_type = graph<
      NodeProperty,
      SocketProperty,
      EdgeProperty,
      default_graph_trait_tag>;

    /* primitive types */

    using node_type   = node<type>;
    using edge_type   = edge<type>;
    using socket_type = socket<type>;

    /* property types */

    using node_property_type   = NodeProperty;
    using edge_property_type   = EdgeProperty;
    using socket_property_type = SocketProperty;

    /* ID traits */

    using id_traits = graph_id_traits<node_type, socket_type, edge_type>;
    using id_type   = typename id_traits::id_type;

    /* container traits */

    using node_container_traits   = graph_container_traits<node, type>;
    using edge_container_traits   = graph_container_traits<edge, type>;
    using socket_container_traits = graph_container_traits<socket, type>;

    /* container types */

    using node_container_type =                         //
      typename node_container_traits::container_type;   //
    using edge_container_type =                         //
      typename edge_container_traits::container_type;   //
    using socket_container_type =                       //
      typename socket_container_traits::container_type; //

    /* descriptor types */

    using node_descriptor_type =
      typename node_container_traits::descriptor_type;
    using edge_descriptor_type =
      typename edge_container_traits::descriptor_type;
    using socket_descriptor_type =
      typename socket_container_traits::descriptor_type;
  };

  /// \brief Node class.
  /// This class repsesents Node object in Graph contains descriptors of sockets
  /// which have link to this object.
  template <typename Traits>
  class node
  {
  public:
    /// Type of this class
    using type = node<Traits>;
    /// Property type
    using property_type = typename Traits::node_property_type;
    /// Descriptor type
    using descriptor_type = typename Traits::node_descriptor_type;
    /// Socket descriptor type
    using socket_descriptor_type = typename Traits::socket_descriptor_type;
    /// ID type
    using id_type = typename Traits::id_type;

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
  template <typename Traits>
  class edge
  {
  public:
    /// Type
    using type = edge<Traits>;
    /// Property type
    using property_type = typename Traits::edge_property_type;
    /// Descriptor type
    using descriptor_type = typename Traits::edge_descriptor_type;
    /// Socket descriptor type
    using socket_descriptor_type = typename Traits::socket_descriptor_type;
    /// ID type
    using id_type = typename Traits::id_type;

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
  template <typename Traits>
  class socket
  {
  public:
    /// Type
    using type = socket<Traits>;
    /// Property type
    using property_type = typename Traits::socket_property_type;
    /// Descriptor type
    using descriptor_type = typename Traits::socket_descriptor_type;
    /// Node descriptor type
    using node_descriptor_type = typename Traits::node_descriptor_type;
    /// Edge descriptor type
    using edge_descriptor_type = typename Traits::edge_descriptor_type;
    /// ID type
    using id_type = typename Traits::id_type;

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

  /// \brief Node Graph class.
  /// This class represents Node Graph Object.
  template <
    class NodeProperty   = empty_graph_property,
    class SocketProperty = empty_graph_property,
    class EdgeProperty   = empty_graph_property,
    class Tag            = default_graph_trait_tag>
  class graph
  {
  public:
    using traits =
      graph_traits<NodeProperty, SocketProperty, EdgeProperty, Tag>;

    using node_type              = typename traits::node_type;
    using edge_type              = typename traits::edge_type;
    using socket_type            = typename traits::socket_type;
    using node_descriptor_type   = typename traits::node_descriptor_type;
    using edge_descriptor_type   = typename traits::edge_descriptor_type;
    using socket_descriptor_type = typename traits::socket_descriptor_type;
    using id_type                = typename traits::id_type;

    /// A constructor
    graph()
    {
    }

    /// Use copy functions to copy instances of Graph
    graph(const graph &) = delete;

    /// Move constructor
    graph(graph &&other) = default;

    /// Copy assignment is deleted. use clone().
    graph &operator=(const graph &) = delete;

    /// Move assignment.
    graph &operator=(graph &&other) = default;

    /// A destructor
    ~graph()
    {
      clear();
      assert(empty());
    }

    /// Add node with id.
    /// \param id Unique ID of new node.
    /// \param args Args to initialize property.
    /// \returns descriptor of new node.
    template <class... Args>
    [[nodiscard]] node_descriptor_type add_node_with_id(
      const id_type &id,
      Args &&... args)
    {
      return _create_n(id, std::forward<Args>(args)...);
    }

    /// Add node.
    /// \param args Args to initialize property.
    /// \returns descriptor of new node.
    template <class... Args>
    [[nodiscard]] node_descriptor_type add_node(Args &&... args)
    {
      return add_node_with_id(_random_id_gen(), std::forward<Args>(args)...);
    }

    /// Add socket with id.
    /// \param id Unique ID for new socket.
    /// \param args Args to initialize property.
    /// \returns descriptor of new socket.
    template <class... Args>
    [[nodiscard]] socket_descriptor_type add_socket_with_id(
      const id_type &id,
      Args &&... args)
    {
      return _create_s(id, std::forward<Args>(args)...);
    }

    /// Add socket.
    /// \param args Args to initialize property
    /// \returns descriptor of new socket
    template <class... Args>
    [[nodiscard]] socket_descriptor_type add_socket(Args &&... args)
    {
      return add_socket_with_id(_random_id_gen(), std::forward<Args>(args)...);
    }

    /// Add edge.
    /// \param src source socket.
    /// \param dst destination socket.
    /// \param Unique ID for new edge.
    /// \param args Args to initialize property class.
    template <class... Args>
    [[nodiscard]] edge_descriptor_type add_edge_with_id(
      const socket_descriptor_type &src,
      const socket_descriptor_type &dst,
      const id_type &id,
      Args &&... args)
    {
      if (!exists(src))
        throw std::invalid_argument("src descriptor doesn't exist");
      if (!exists(dst))
        throw std::invalid_argument("dst descriptor doesn't exist");

      for (auto &&e : _access(src).src_edges()) {
        if (_access(e).dst() == dst)
          throw std::runtime_error("edge already exists");
      }

      for (auto &&e : _access(dst).dst_edges()) {
        if (_access(e).src() == src)
          throw std::runtime_error("edge already exists");
      }

      auto e = _create_e(id, src, dst, std::forward<Args>(args)...);

      assert(e);

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
    [[nodiscard]] edge_descriptor_type add_edge(
      const socket_descriptor_type &src,
      const socket_descriptor_type &dst,
      Args &&... args)
    {
      return add_edge_with_id(
        src, dst, _random_id_gen(), std::forward<Args>(args)...);
    }

    /// Remove node.
    /// \param descriptor descriptor of node
    void remove_node(const node_descriptor_type &descriptor)
    {
      if (!exists(descriptor))
        return; // invalid descriptor

      // delete links from sockets
      for (auto &&s : _access(descriptor).sockets())
        _access(s).unset_node(descriptor);

      _destroy(descriptor);
      return;
    }

    /// \brief Remove edge.
    /// \param descriptor descriptor of edge
    void remove_edge(const edge_descriptor_type &descriptor)
    {
      if (!exists(descriptor))
        return; // invalid descriptor

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
    /// \param descriptor descriptor of socket
    void remove_socket(const socket_descriptor_type &descriptor)
    {
      if (!exists(descriptor))
        return; // nonexist descriptor

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
      return traits::node_container_traits::exists(m_nodes, descriptor);
    }

    /// Check if edge descriptor exists
    /// \param descriptor descriptor of edge
    /// \returns true if success
    [[nodiscard]] bool exists(const edge_descriptor_type &descriptor) const
    {
      return traits::edge_container_traits::exists(m_edges, descriptor);
    }

    /// Check if socket descriptor exists
    /// \param descriptor descriptor of socket
    /// \returns true if success
    [[nodiscard]] bool exists(const socket_descriptor_type &descriptor) const
    {
      return traits::socket_container_traits::exists(m_sockets, descriptor);
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of node
    /// \returns if success
    [[nodiscard]] NodeProperty &operator[](
      const node_descriptor_type &descriptor)
    {
      return _access(descriptor).property();
    }
    /// Descriptor access operator
    /// \param descriptor descriptor of node
    /// \returns if descriptor is invalid, return value is undefined.
    [[nodiscard]] const NodeProperty &operator[](
      const node_descriptor_type &descriptor) const
    {
      return _access(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of edge
    /// \returns if success
    [[nodiscard]] EdgeProperty &operator[](
      const edge_descriptor_type &descriptor)
    {
      return _access(descriptor).property();
    }
    /// Descriptor access operator.
    /// \param descriptor descriptor of edge
    /// \returns if success
    [[nodiscard]] const EdgeProperty &operator[](
      const edge_descriptor_type &descriptor) const
    {
      return _access(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    [[nodiscard]] SocketProperty &operator[](
      const socket_descriptor_type &descriptor)
    {
      return _access(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    [[nodiscard]] const SocketProperty &operator[](
      const socket_descriptor_type &descriptor) const
    {
      return _access(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of node
    /// \returns if success
    [[nodiscard]] NodeProperty &at(const node_descriptor_type &descriptor)
    {
      return _at(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of node
    /// \returns if descriptor is invalid, return value is undefined.
    [[nodiscard]] const NodeProperty &at(
      const node_descriptor_type &descriptor) const
    {
      return _at(descriptor).property();
    }

    /// Descriptor access operator
    /// \param descriptor descriptor of edge
    /// \returns if success
    [[nodiscard]] EdgeProperty &at(const edge_descriptor_type &descriptor)
    {
      return _at(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of edge
    /// \returns if success
    [[nodiscard]] const EdgeProperty &at(
      const edge_descriptor_type &descriptor) const
    {
      return _at(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    [[nodiscard]] SocketProperty &at(const socket_descriptor_type &descriptor)
    {
      return _at(descriptor).property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    [[nodiscard]] const SocketProperty &at(
      const socket_descriptor_type &descriptor) const
    {
      return _at(descriptor).property();
    }

    /// Attach socket.
    /// \param node node descriptor
    /// \param socket socket descriptor
    [[nodiscard]] bool attach_socket(
      const node_descriptor_type &node,
      const socket_descriptor_type &socket)
    {
      if (!exists(node) || !exists(socket))
        return false;

      _access(socket).set_node(node);
      _access(node).set_socket(socket);

      return true;
    }

    /// Detach socket.
    /// \param node node descriptor
    /// \param socket socket descriptor
    void detach_socket(
      const node_descriptor_type &node,
      const socket_descriptor_type &socket)
    {
      if (!exists(node) || !exists(socket))
        return;

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
    [[nodiscard]] std::vector<node_descriptor_type> nodes() const
    {
      return _nodes();
    }

    /// Get node count.
    [[nodiscard]] size_t n_nodes() const
    {
      return _n_nodes();
    }

    /// Get nodes connected to the socket.
    [[nodiscard]] std::vector<node_descriptor_type> nodes(
      const socket_descriptor_type &descriptor) const
    {
      auto &ns = _at(descriptor).nodes();
      return {ns.begin(), ns.end()};
    }

    /// Get number of connected nodes to the socket.
    [[nodiscard]] size_t n_nodes(const socket_descriptor_type &descriptor) const
    {
      return _at(descriptor).n_nodes();
    }

    /// Get sockets.
    [[nodiscard]] std::vector<socket_descriptor_type> sockets() const
    {
      return _sockets();
    }

    /// Get socket count.
    [[nodiscard]] size_t n_sockets() const
    {
      return _n_sockets();
    }

    /// Get sockets connected to the node.
    [[nodiscard]] std::vector<socket_descriptor_type> sockets(
      const node_descriptor_type &descriptor) const
    {
      auto &ss = _at(descriptor).sockets();
      return {ss.begin(), ss.end()};
    }

    /// Get number of connected sockets to the node.
    [[nodiscard]] size_t n_sockets(const node_descriptor_type &descriptor) const
    {
      return _at(descriptor).n_sockets();
    }

    /// Get edges.
    [[nodiscard]] std::vector<edge_descriptor_type> edges() const
    {
      return _edges();
    }

    /// Get edge count.
    [[nodiscard]] size_t n_edges() const
    {
      return _n_edges();
    }

    /// Get src edges.
    [[nodiscard]] std::vector<edge_descriptor_type> src_edges(
      const socket_descriptor_type &descriptor) const
    {
      auto &es = _at(descriptor).src_edges();
      return {es.begin(), es.end()};
    }

    /// Get number of src edges.
    [[nodiscard]] size_t n_src_edges(
      const socket_descriptor_type &descriptor) const
    {
      return _at(descriptor).n_src_edges();
    }

    /// Get dst edges.
    [[nodiscard]] std::vector<edge_descriptor_type> dst_edges(
      const socket_descriptor_type &descriptor) const
    {
      auto &es = _at(descriptor).dst_edges();
      return {es.begin(), es.end()};
    }

    /// Get number of dst edges.
    [[nodiscard]] size_t n_dst_edges(
      const socket_descriptor_type &descriptor) const
    {
      return _at(descriptor).n_dst_edges();
    }

    /// Get src socket.
    [[nodiscard]] socket_descriptor_type src(
      const edge_descriptor_type &descriptor) const
    {
      return _at(descriptor).src();
    }

    /// Get dst socket.
    [[nodiscard]] socket_descriptor_type dst(
      const edge_descriptor_type &descriptor) const
    {
      return _at(descriptor).dst();
    }

    /// Get id of node.
    [[nodiscard]] id_type id(const node_descriptor_type &descriptor) const
    {
      return _at(descriptor).id();
    }

    /// Get id of socket.
    [[nodiscard]] id_type id(const socket_descriptor_type &descriptor) const
    {
      return _at(descriptor).id();
    }

    /// Get id of edge.
    [[nodiscard]] id_type id(const edge_descriptor_type &descriptor) const
    {
      return _at(descriptor).id();
    }

    /// Find node from id.
    [[nodiscard]] node_descriptor_type node(const id_type &id) const
    {
      return traits::node_container_traits::find_descriptor(m_nodes, id);
    }

    /// Get socket from id.
    [[nodiscard]] socket_descriptor_type socket(const id_type &id) const
    {
      return traits::socket_container_traits::find_descriptor(m_sockets, id);
    }

    /// Get edge from id.
    [[nodiscard]] edge_descriptor_type edge(const id_type &id) const
    {
      return traits::edge_container_traits::find_descriptor(m_edges, id);
    }

    /// Clone graph
    [[nodiscard]] graph clone() const
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
    inline node_descriptor_type _create_n(const id_type &id, Args &&... args)
    {
      return traits::node_container_traits::create(
        m_nodes, id, std::forward<Args>(args)...);
    }

    /// Create new edge.
    /// \param id id for new edge.
    /// \param args arguments to construct node property.
    template <class... Args>
    inline edge_descriptor_type _create_e(const id_type &id, Args &&... args)
    {
      return traits::edge_container_traits::create(
        m_edges, id, std::forward<Args>(args)...);
    }

    /// Create new socket.
    /// \param id id for new socket.
    /// \param args arguments to construct node property
    template <class... Args>
    inline socket_descriptor_type _create_s(const id_type &id, Args &&... args)
    {
      return traits::socket_container_traits::create(
        m_sockets, id, std::forward<Args>(args)...);
    }

    /// Create random id for node/socket/edge.
    id_type _random_id_gen()
    {
      return traits::id_traits::random_generate();
    }

    /// Get list of node descriptors.
    inline std::vector<node_descriptor_type> _nodes() const
    {
      return traits::node_container_traits::descriptors(m_nodes);
    }

    /// Get size of nodes.
    inline size_t _n_nodes() const
    {
      return traits::node_container_traits::size(m_nodes);
    }

    /// Get list of socket descriptors.
    inline std::vector<socket_descriptor_type> _sockets() const
    {
      return traits::socket_container_traits::descriptors(m_sockets);
    }

    /// Get size of sockets.
    inline size_t _n_sockets() const
    {
      return traits::socket_container_traits::size(m_sockets);
    }

    /// Get list of edges descriptors.
    inline std::vector<edge_descriptor_type> _edges() const
    {
      return traits::edge_container_traits::descriptors(m_edges);
    }

    /// Get size of edges.
    inline size_t _n_edges() const
    {
      return traits::edge_container_traits::size(m_edges);
    }

    /// Node access.
    /// \param n if nonexist or invalid, behaviour of this function is undefined
    inline node_type &_access(const node_descriptor_type &n) noexcept
    {
      return traits::node_container_traits::access(m_nodes, n);
    }

    inline const node_type &_access(const node_descriptor_type &n) const
      noexcept
    {
      return traits::node_container_traits::access(m_nodes, n);
    }

    /// Edge access.
    /// \param n if nonexist or invalid, behaviour of this function is undefined
    inline edge_type &_access(const edge_descriptor_type &e) noexcept
    {
      return traits::edge_container_traits::access(m_edges, e);
    }

    inline const edge_type &_access(const edge_descriptor_type &e) const
      noexcept
    {
      return traits::edge_container_traits::access(m_edges, e);
    }

    /// Socket access.
    /// \param n if nonexist or invalid, behaviour of this function is undefined
    inline socket_type &_access(const socket_descriptor_type &s) noexcept
    {
      return traits::socket_container_traits::access(m_sockets, s);
    }

    inline const socket_type &_access(const socket_descriptor_type &s) const
      noexcept
    {
      return traits::socket_container_traits::access(m_sockets, s);
    }

    /// Range checked node access.
    /// \param descriptor descriptor
    /// \returns reference
    inline node_type &_at(const node_descriptor_type &descriptor)
    {
      return traits::node_container_traits::at(m_nodes, descriptor);
    }

    inline const node_type &_at(const node_descriptor_type &descriptor) const
    {
      return traits::node_container_traits::at(m_nodes, descriptor);
    }

    /// Range checked Edge access
    /// \param descriptor descriptor
    /// \returns reference
    inline edge_type &_at(const edge_descriptor_type &descriptor)
    {
      return traits::edge_container_traits::at(m_edges, descriptor);
    }

    inline const edge_type &_at(const edge_descriptor_type &descriptor) const
    {
      return traits::edge_container_traits::at(m_edges, descriptor);
    }

    /// Range checked Socket access.
    /// \param descriptor descriptor
    /// \returns reference
    inline socket_type &_at(const socket_descriptor_type &descriptor)
    {
      return traits::socket_container_traits::at(m_sockets, descriptor);
    }

    inline const socket_type &_at(
      const socket_descriptor_type &descriptor) const
    {
      return traits::socket_container_traits::at(m_sockets, descriptor);
    }

    /// Destroy node.
    /// \param descriptor descriptor of node
    /// \returns true if success
    void _destroy(const node_descriptor_type &descriptor) noexcept
    {
      traits::node_container_traits::destroy(
        m_nodes, descriptor, _access(descriptor).id());
    }

    /// Destroy edge.
    /// \param descriptor descriptor of edge
    /// \returns true if success
    void _destroy(const edge_descriptor_type &descriptor) noexcept
    {
      traits::edge_container_traits::destroy(
        m_edges, descriptor, _access(descriptor).id());
    }

    /// Destroy edge.
    /// \param descriptor descriptor of socket
    /// \returns true if success
    void _destroy(const socket_descriptor_type &descriptor) noexcept
    {
      traits::socket_container_traits::destroy(
        m_sockets, descriptor, _access(descriptor).id());
    }

    /// list of node descriptors
    typename traits::node_container_type m_nodes;

    /// list of edge descriptors
    typename traits::edge_container_type m_edges;

    /// list of socket descriptors
    typename traits::socket_container_type m_sockets;
  };

} // namespace yave::graph