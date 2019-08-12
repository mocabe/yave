//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <cassert>
#include <exception>
#include <stdexcept>
#include <map>
#include <memory>
#include <vector>
#include <utility>
#include <algorithm>
#include <string>
#include <random>
#include <chrono>

namespace yave::graph {

  // Low-level Graph implementation.
  // Graph is not like general graph representation. It has 2 different vertex
  // type; Node and Socket.
  // Each socket can be connected to nodes, and Edge represents connection
  // between sockets. Also, Edge has direction.

  // TODO: Possibly replace internal representation using Boost.Graph library.
  // TODO: Use memory pool for allocation to improve data locality.
  // TODO: Implement outline property types (property types its instances are
  // allocated separately, instead of embedded into Node/Socket/Edge instances).

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
    class Property,
    class Tag = default_graph_trait_tag>
  struct graph_container_traits;

  template <template <class> class ValueType, class GraphTraits, class Property>
  struct graph_container_traits<
    ValueType,
    GraphTraits,
    Property,
    default_graph_trait_tag>
  {

    // value type
    using value_type = ValueType<GraphTraits>;
    // descriptor type
    using descriptor_type = value_type *;
    // container type
    using container_type = std::vector<descriptor_type>;
    // property type
    using property_type = Property;
    // inline property type
    using inline_property_type = Property;

    /// Check if a descriptor exists in a container.
    /// \param c Container
    /// \param descriptor descriptor
    /// \param return true when descriptor is valid and exists in the container
    static inline bool
      exists(const container_type &c, const descriptor_type &descriptor)
    {
      if (descriptor == nullptr)
        return false;

      auto found = std::find(c.begin(), c.end(), descriptor);

      if (found != c.end())
        return true;

      return false;
    }

    /// Destroy instance from a container.
    /// Descriptor will become invalid after calling this
    /// function.
    /// \param c Container
    /// \param descriptor Descriptor
    static inline void
      destroy(container_type &c, const descriptor_type &descriptor) noexcept
    {
      auto tail = std::remove_if(c.begin(), c.end(), [&](descriptor_type dsc) {
        return dsc == descriptor;
      });

      if (tail != c.end()) {
        c.erase(tail, c.end());
        delete (descriptor);
      }
    }

    /// Instance in a container and return descriptor.
    /// \param c Container
    /// \param args Args for initialize value type
    template <class... Args>
    static inline descriptor_type create(container_type &c, Args &&... args)
    {
      value_type *v = new value_type(std::forward<Args>(args)...);
      c.push_back(v);
      return v;
    }

    /// Access function.
    /// \param c Container.
    /// \param descriptor A valid descriptor.
    /// \returns reference of value type in the container.
    static inline value_type &
      access(container_type &c, const descriptor_type &descriptor) noexcept
    {
      (void)c;
      assert(descriptor);
      return *descriptor;
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
    static inline value_type &
      at(container_type &c, const descriptor_type &descriptor)
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
    static inline const value_type &
      at(const container_type &c, const descriptor_type &descriptor)
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
      auto seed =
        std::chrono::high_resolution_clock::now().time_since_epoch().count();

      auto gen = std::mt19937_64(seed);

      return gen();
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

    /* container traits */

    using node_container_traits =
      graph_container_traits<node, type, NodeProperty>;

    using edge_container_traits =
      graph_container_traits<edge, type, EdgeProperty>;

    using socket_container_traits =
      graph_container_traits<socket, type, SocketProperty>;

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

    /* ID traits */

    using id_traits = graph_id_traits<node_type, socket_type, edge_type>;
    using id_type   = typename id_traits::id_type;
  };

  /// \brief Node class.
  /// This class repsesents Node object in Graph contains descriptors of sockets
  /// which have link to this object.
  template <typename Traits>
  class node
  {
  public:
    friend typename Traits::graph_type;

    /// Type of this class
    using type = node<Traits>;
    /// Inline property type
    using inline_property_type =
      typename Traits::node_container_traits::inline_property_type;
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
      , m_inline_property {std::forward<Args>(args)...}
    {
    }

    /// A destructor.
    ~node() noexcept
    {
    }

    /// Get id.
    [[nodiscard]] id_type id() const noexcept
    {
      return m_id;
    }

    /// Get sockets.
    [[nodiscard]] std::vector<socket_descriptor_type> sockets() const
    {
      return {m_sockets.begin(), m_sockets.end()};
    }

    /// Get socket count.
    [[nodiscard]] size_t n_sockets() const
    {
      return m_sockets.size();
    }

    /// Inline property accessor.
    [[nodiscard]] const inline_property_type &inline_property() const
    {
      return m_inline_property;
    }

    /// Inline property accessor
    [[nodiscard]] inline_property_type &inline_property()
    {
      return m_inline_property;
    }

  private:
    /// Set socket descriptor.
    /// \param descriptor descriptor of target socket
    /// \returns return true if success. false if invalid descriptor or it is
    /// already set.
    [[nodiscard]] bool set_socket(const socket_descriptor_type &descriptor)
    {
      // error check
      for (auto &&d : m_sockets) {
        if (d == descriptor)
          return false;
      }

      m_sockets.push_back(descriptor); // set socket
      return true;
    }

    /// Remove socket.
    /// \param descriptor descriptor of socket
    /// \returns true when succsess false if invalid descriptor
    void unset_socket(const socket_descriptor_type &descriptor)
    {
      auto tmp {m_sockets};
      for (auto iter = tmp.begin(); iter != tmp.end(); ++iter) {
        if (*iter == descriptor) {
          iter = tmp.erase(iter); // not noexcept
          if (iter == tmp.end())
            break;
        }
      }
      m_sockets.swap(tmp);
      return;
    }

  private:
    /// Id of this node.
    id_type m_id;
    /// Socket descriptors.
    /// Shouldn't contain 2 or more same descriptors.
    std::basic_string<socket_descriptor_type> m_sockets;
    /// Inline property class instance
    inline_property_type m_inline_property;
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
    friend typename Traits::graph_type;
    /// Type
    using type = edge<Traits>;
    /// Inline property type
    using inline_property_type =
      typename Traits::edge_container_traits::inline_property_type;
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
      , m_inline_property {std::forward<Args>(args)...}
    {
    }

    /// A destructor.
    ~edge() noexcept
    {
    }

    /// Get id.
    [[nodiscard]] id_type id() const noexcept
    {
      return m_id;
    }

    /// Get source socket.
    [[nodiscard]] socket_descriptor_type src() const
    {
      return m_src;
    }

    /// Get destination socket.
    [[nodiscard]] socket_descriptor_type dst() const
    {
      return m_dst;
    }

    /// Inline property accessor.
    [[nodiscard]] const inline_property_type &inline_property() const
    {
      return m_inline_property;
    }

    /// Inline property accessor.
    [[nodiscard]] inline_property_type &inline_property()
    {
      return m_inline_property;
    }

  private:
    /// Id of this socket.
    id_type m_id;
    /// socket descriptors
    socket_descriptor_type m_src, m_dst;
    /// inline property class instance
    inline_property_type m_inline_property;
  };

  /// \brief Socket class.
  /// This class represents socket object in Graph.
  /// contains list of Node descriptors and list of Edge descriptors.
  template <typename Traits>
  class socket
  {
  public:
    friend typename Traits::graph_type;
    /// Type
    using type = socket<Traits>;
    /// Inline property type
    using inline_property_type =
      typename Traits::socket_container_traits::inline_property_type;
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
      , m_inline_property {std::forward<Args>(args)...}
    {
    }

    /// A destructor
    ~socket() noexcept
    {
    }

    /// Get id.
    [[nodiscard]] id_type id() const noexcept
    {
      return m_id;
    }

    /// Get nodes.
    [[nodiscard]] std::vector<node_descriptor_type> nodes() const
    {
      return {m_nodes.begin(), m_nodes.end()};
    }

    /// Get source edges.
    [[nodiscard]] std::vector<edge_descriptor_type> src_edges() const
    {
      return {m_src_edges.begin(), m_src_edges.end()};
    }
    /// Get destination edges.
    [[nodiscard]] std::vector<edge_descriptor_type> dst_edges() const
    {
      return {m_dst_edges.begin(), m_dst_edges.end()};
    }

    /// Get node count.
    [[nodiscard]] size_t n_nodes() const
    {
      return m_nodes.size();
    }

    /// Get srouce edge count.
    [[nodiscard]] size_t n_src_edges() const
    {
      return m_src_edges.size();
    }

    /// Get destination edge count.
    [[nodiscard]] size_t n_dst_edges() const
    {
      return m_dst_edges.size();
    }

    /// Inline property accessor.
    [[nodiscard]] const inline_property_type &inline_property() const
    {
      return m_inline_property;
    }

    /// Inline property accessor.
    [[nodiscard]] inline_property_type &inline_property()
    {
      return m_inline_property;
    }

  private:
    /// Set src edge.
    /// \param descriptor descriptor of edge
    /// \returns true when success
    [[nodiscard]] bool set_src_edge(const edge_descriptor_type &descriptor)
    {
      // error check
      for (auto &&d : m_src_edges) {
        if (d == descriptor)
          return false;
      }

      m_src_edges.push_back(descriptor); // add edge
      return true;
    }
    /// Set dst edge.
    /// \param descriptor descriptor of edge
    /// \returns true when success
    [[nodiscard]] bool set_dst_edge(const edge_descriptor_type &descriptor)
    {

      // error check
      for (auto &&d : m_dst_edges) {
        if (d == descriptor)
          return false;
      }

      m_dst_edges.push_back(descriptor); // add edge
      return true;
    }

    /// Set node.
    /// \param descriptor descriptor of node
    /// \returns true when success
    [[nodiscard]] bool set_node(const node_descriptor_type &descriptor)
    {

      // error check
      for (auto &&d : m_nodes) {
        if (d == descriptor)
          return false;
      }

      m_nodes.push_back(descriptor); // add node
      return true;
    }

    /// Unset node.
    /// \param descriptor Descriptor of node
    /// \returns true if success
    void unset_node(const node_descriptor_type &descriptor)
    {
      auto tmp {m_nodes};
      for (auto iter = tmp.begin(); iter != tmp.end(); ++iter) {
        if (*iter == descriptor)
          iter = tmp.erase(iter);
        if (iter == tmp.end())
          break;
      }
      m_nodes.swap(tmp);
      return;
    }

    /// Unset edge.
    /// \param descriptor Descriptor of edge
    /// \returns true if success
    void unset_src_edge(const edge_descriptor_type &descriptor)
    {
      // erase descriptor
      auto tmp {m_src_edges};
      for (auto iter = tmp.begin(); iter != tmp.end(); ++iter) {
        if (*iter == descriptor)
          iter = tmp.erase(iter);
        if (iter == tmp.end())
          break;
      }
      m_src_edges.swap(tmp);
      return;
    }

    /// Unset edge
    /// \param descriptor descriptor of edge
    /// \returns true if success.
    void unset_dst_edge(const edge_descriptor_type &descriptor)
    {
      // erase descriptor
      auto tmp {m_dst_edges};
      for (auto iter = tmp.begin(); iter != tmp.end(); ++iter) {
        if (*iter == descriptor)
          iter = tmp.erase(iter);
        if (iter == tmp.end())
          break;
      }
      m_dst_edges.swap(tmp);
      return;
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
    /// inline property class instance
    inline_property_type m_inline_property;
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

    using node_type   = typename traits::node_type;
    using edge_type   = typename traits::edge_type;
    using socket_type = typename traits::socket_type;

    using node_descriptor_type   = typename traits::node_descriptor_type;
    using edge_descriptor_type   = typename traits::edge_descriptor_type;
    using socket_descriptor_type = typename traits::socket_descriptor_type;

    using id_type = typename traits::id_type;

    /// A constructor
    graph()
    {
    }

    /// Use copy functions to copy instances of Graph
    graph(const graph &) = delete;

    /// Move constructor
    graph(graph &&other)
    {
      m_nodes   = std::move(other.m_nodes);
      m_edges   = std::move(other.m_edges);
      m_sockets = std::move(other.m_sockets);
    }

    /// Copy assignment is deleted. use clone().
    graph &operator=(const graph &) = delete;

    /// Move assignment.
    graph &operator=(graph &&other)
    {
      clear();
      m_nodes   = std::move(other.m_nodes);
      m_edges   = std::move(other.m_edges);
      m_sockets = std::move(other.m_sockets);

      return *this;
    }

    /// A destructor
    ~graph()
    {
      clear();
      assert(empty());
    }

    /// Add node.
    /// \param args Args to initialize property
    /// \returns descriptor of new node
    template <class... Args>
    [[nodiscard]] node_descriptor_type add_node(Args &&... args)
    {
      // add node
      return _create_n(_random_id_gen(), std::forward<Args>(args)...);
    }

    /// Add socket.
    /// \param args Args to initialize property
    /// \returns descriptor of new socket
    template <class... Args>
    [[nodiscard]] socket_descriptor_type add_socket(Args &&... args)
    {
      // add socket
      return _create_s(_random_id_gen(), std::forward<Args>(args)...);
    }

    /// Add edge.
    /// \param src source socket
    /// \param dst destination socket
    /// \param args Args to initialize property class
    template <class... Args>
    [[nodiscard]] edge_descriptor_type add_edge(
      const socket_descriptor_type &src,
      const socket_descriptor_type &dst,
      Args &&... args)
    {
      if (!exists(src))
        throw std::invalid_argument("src descriptor doesn't exist");
      if (!exists(dst))
        throw std::invalid_argument("dst descriptor doesn't exist");

      for (auto &&e : _access(src).m_src_edges) {
        if ((*this).dst(e) == dst)
          throw std::runtime_error("edge already exists");
      }

      for (auto &&e : _access(dst).m_dst_edges) {
        if ((*this).src(e) == src)
          throw std::runtime_error("edge already exists");
      }

      auto e =
        _create_e(_random_id_gen(), src, dst, std::forward<Args>(args)...);

      // set edge to sockets
      bool r1 [[maybe_unused]] = _access(src).set_src_edge(e);
      bool r2 [[maybe_unused]] = _access(dst).set_dst_edge(e);
      assert(r1 && r2);

      return e;
    }

    /// Remove node.
    /// \param descriptor descriptor of node
    void remove_node(const node_descriptor_type &descriptor)
    {
      if (!exists(descriptor))
        return; // invalid descriptor

      // delete links from sockets
      for (auto &&s : _access(descriptor).m_sockets)
        _at(s).unset_node(descriptor);

      _destroy(descriptor);
      return;
    }

    /// \brief Remove edge.
    /// \param descriptor descriptor of edge
    void remove_edge(const edge_descriptor_type &descriptor)
    {
      if (!exists(descriptor))
        return; // invalid descriptor

      auto &srcSocket = _access(descriptor).m_src;
      auto &dstSocket = _access(descriptor).m_dst;

      // should remove more than one edges.
      _access(srcSocket).unset_src_edge(descriptor);
      _access(dstSocket).unset_dst_edge(descriptor);

      _destroy(descriptor);
      return;
    }

    /// Remove socket
    /// Removes all of edges connected from the socket.
    /// \param descriptor descriptor of socket
    void remove_socket(const socket_descriptor_type &descriptor)
    {
      if (!exists(descriptor))
        return; // nonexist descriptor{

      for (auto &&e : src_edges(descriptor)) remove_edge(e);
      for (auto &&e : dst_edges(descriptor)) remove_edge(e);

      auto nds = nodes(descriptor);

      for (auto &&n : nds)
        _access(n).unset_socket(descriptor); // remove all links from node
      _destroy(descriptor); // remove all socket and delete instance

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
    [[nodiscard]] NodeProperty &
      operator[](const node_descriptor_type &descriptor)
    {
      return _access(descriptor).inline_property();
    }
    /// Descriptor access operator
    /// \param descriptor descriptor of node
    /// \returns if descriptor is invalid, return value is undefined.
    [[nodiscard]] const NodeProperty &
      operator[](const node_descriptor_type &descriptor) const
    {
      return _access(descriptor).inline_property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of edge
    /// \returns if success
    [[nodiscard]] EdgeProperty &
      operator[](const edge_descriptor_type &descriptor)
    {
      return _access(descriptor).inline_property();
    }
    /// Descriptor access operator.
    /// \param descriptor descriptor of edge
    /// \returns if success
    [[nodiscard]] const EdgeProperty &
      operator[](const edge_descriptor_type &descriptor) const
    {
      return _access(descriptor).inline_property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    [[nodiscard]] SocketProperty &
      operator[](const socket_descriptor_type &descriptor)
    {
      return _access(descriptor).inline_property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    [[nodiscard]] const SocketProperty &
      operator[](const socket_descriptor_type &descriptor) const
    {
      return _access(descriptor).inline_property();
    }

    // Descriptor access operator.
    // \param descriptor descriptor of node
    // \returns if success
    [[nodiscard]] NodeProperty &at(const node_descriptor_type &descriptor)
    {
      return _at(descriptor).inline_property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of node
    /// \returns if descriptor is invalid, return value is undefined.
    [[nodiscard]] const NodeProperty &
      at(const node_descriptor_type &descriptor) const
    {
      return _at(descriptor).inline_property();
    }

    /// Descriptor access operator
    /// \param descriptor descriptor of edge
    /// \returns if success
    [[nodiscard]] EdgeProperty &at(const edge_descriptor_type &descriptor)
    {
      return _at(descriptor).inline_property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of edge
    /// \returns if success
    [[nodiscard]] const EdgeProperty &
      at(const edge_descriptor_type &descriptor) const
    {
      return _at(descriptor).inline_property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    [[nodiscard]] SocketProperty &at(const socket_descriptor_type &descriptor)
    {
      return _at(descriptor).inline_property();
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    [[nodiscard]] const SocketProperty &
      at(const socket_descriptor_type &descriptor) const
    {
      return _at(descriptor).inline_property();
    }

    /// Attach socket.
    /// \param node node descriptor
    /// \param socket socket descriptor
    [[nodiscard]] bool attach_socket(
      const node_descriptor_type &node,
      const socket_descriptor_type &socket)
    {
      if (!(exists(node) && exists(socket)))
        return false;

      if (_access(socket).set_node(node)) {
        if (_access(node).set_socket(socket)) {
          return true;
        } else {
          // unset socket to avoid one-way connection
          _access(socket).unset_node(node);
        }
      }
      return false;
    }

    /// Get nodes.
    [[nodiscard]] auto nodes() const
    {
      return m_nodes;
    }

    /// Get node count.
    [[nodiscard]] auto n_nodes() const
    {
      return m_nodes.size();
    }

    /// Get nodes connected to the socket.
    [[nodiscard]] std::vector<node_descriptor_type>
      nodes(const socket_descriptor_type &descriptor) const
    {
      auto &s = _at(descriptor);
      return s.nodes();
    }

    /// Get number of connected nodes to the socket.
    [[nodiscard]] auto n_nodes(const socket_descriptor_type &descriptor) const
    {
      const auto &s = _at(descriptor);
      return s.n_nodes();
    }

    /// Get sockets.
    [[nodiscard]] auto sockets() const
    {
      return m_sockets;
    }

    /// Get socket count.
    [[nodiscard]] auto n_sockets() const
    {
      return m_sockets.size();
    }

    /// Get sockets connected to the node.
    [[nodiscard]] std::vector<socket_descriptor_type>
      sockets(const node_descriptor_type &descriptor) const
    {
      const auto &n = _at(descriptor);
      return n.sockets();
    }

    /// Get number of connected sockets to the node.
    [[nodiscard]] auto n_sockets(const node_descriptor_type &descriptor) const
    {
      const auto &n = _at(descriptor);
      return n.n_sockets();
    }

    /// Get edges.
    [[nodiscard]] auto edges() const
    {
      return m_edges;
    }

    /// Get edge count.
    [[nodiscard]] auto n_edges() const
    {
      return m_edges.size();
    }

    /// Get src edges.
    [[nodiscard]] std::vector<edge_descriptor_type>
      src_edges(const socket_descriptor_type &descriptor) const
    {
      auto &s = _at(descriptor);
      return s.src_edges();
    }

    /// Get number of src edges.
    [[nodiscard]] auto
      n_src_edges(const socket_descriptor_type &descriptor) const
    {
      const auto &s = _at(descriptor);
      return s.n_src_edges();
    }

    /// Get dst edges.
    [[nodiscard]] std::vector<edge_descriptor_type>
      dst_edges(const socket_descriptor_type &descriptor) const
    {
      auto &s = _at(descriptor);
      return s.dst_edges();
    }

    /// Get number of dst edges.
    [[nodiscard]] auto
      n_dst_edges(const socket_descriptor_type &descriptor) const
    {
      const auto &s = _at(descriptor);
      return s.n_dst_edges();
    }

    /// Get src socket.
    [[nodiscard]] socket_descriptor_type
      src(const edge_descriptor_type &descriptor) const
    {
      auto &e = _at(descriptor);
      return e.src();
    }

    /// Get dst socket.
    [[nodiscard]] socket_descriptor_type
      dst(const edge_descriptor_type &descriptor) const
    {
      auto &e = _at(descriptor);
      return e.dst();
    }

    /// Get id of node.
    [[nodiscard]] id_type id(const node_descriptor_type &descriptor) const
    {
      auto &n = _at(descriptor);
      return n.id();
    }

    /// Get id of socket.
    [[nodiscard]] id_type id(const socket_descriptor_type &descriptor) const
    {
      auto &s = _at(descriptor);
      return s.id();
    }

    /// Get id of edge.
    [[nodiscard]] id_type id(const edge_descriptor_type &descriptor) const
    {
      auto &e = _at(descriptor);
      return e.id();
    }

    /// Find node from id.
    [[nodiscard]] node_descriptor_type node(const id_type &id) const
    {
      for (auto &&n : nodes()) {
        if (_access(n).id() == id)
          return n;
      }
      return nullptr;
    }

    /// Get socket from id.
    [[nodiscard]] socket_descriptor_type socket(const id_type &id) const
    {
      for (auto &&s : sockets()) {
        if (_access(s).id() == id)
          return s;
      }
      return nullptr;
    }

    /// Get edge from id.
    [[nodiscard]] edge_descriptor_type edge(const id_type &id) const
    {
      for (auto &&e : edges()) {
        if (_access(e).id() == id)
          return e;
      }
      return nullptr;
    }

    /// Clone graph
    [[nodiscard]] graph clone() const
    {
      graph g;

      std::map<id_type, node_descriptor_type> n_map;
      std::map<id_type, socket_descriptor_type> s_map;

      for (auto &&n : nodes()) {
        // copy nodes
        auto dsc = g._create_n(id(n), _access(n).inline_property());
        n_map.emplace(id(n), dsc);
        assert(dsc);
      }

      for (auto &&s : sockets()) {
        // copy socket
        auto dsc = g._create_s(id(s), _access(s).inline_property());
        s_map.emplace(id(s), dsc);
        assert(dsc);

        // attach socket
        for (auto &&n : _access(s).nodes()) {
          [[maybe_unused]] auto r = g.attach_socket(n_map.at(id(n)), dsc);
          assert(r);
        }
      }

      for (auto &&e : edges()) {
        // copy edges
        auto s   = s_map.at(id(_access(e).src()));
        auto d   = s_map.at(id(_access(e).dst()));
        auto dsc = g._create_e(id(e), s, d, _access(e).inline_property());
        assert(dsc);

        // set back pointers
        {
          [[maybe_unused]] auto r = g._access(s).set_src_edge(dsc);
          assert(r);
        }
        {
          [[maybe_unused]] auto r = g._access(d).set_dst_edge(dsc);
          assert(r);
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
      for (auto &&e : edges()) _destroy(e);
      for (auto &&n : nodes()) _destroy(n);
      for (auto &&s : sockets()) _destroy(s);
      assert(empty());
    }

    /// Empty
    [[nodiscard]] bool empty()
    {
      return (m_nodes.empty() && m_edges.empty() && m_sockets.empty());
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

    inline const socket_type &
      _at(const socket_descriptor_type &descriptor) const
    {
      return traits::socket_container_traits::at(m_sockets, descriptor);
    }

    /// Destroy node.
    /// \param descriptor descriptor of node
    /// \returns true if success
    void _destroy(const node_descriptor_type &descriptor) noexcept
    {
      traits::node_container_traits::destroy(m_nodes, descriptor);
    }

    /// Destroy edge.
    /// \param descriptor descriptor of edge
    /// \returns true if success
    void _destroy(const edge_descriptor_type &descriptor) noexcept
    {
      traits::edge_container_traits::destroy(m_edges, descriptor);
    }

    /// Destroy edge.
    /// \param descriptor descriptor of socket
    /// \returns true if success
    void _destroy(const socket_descriptor_type &descriptor) noexcept
    {
      traits::socket_container_traits::destroy(m_sockets, descriptor);
    }

    /// list of node descriptors
    typename traits::node_container_type m_nodes;

    /// list of edge descriptors
    typename traits::edge_container_type m_edges;

    /// list of socket descriptors
    typename traits::socket_container_type m_sockets;
  };

} // namespace yave