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

namespace yave {

  template <typename Traits>
  class Node;

  template <typename Traits>
  class NodeEdge;

  template <typename Traits>
  class NodeEdgeSocket;

  template <
    class NodeProperty,
    class SocketProperty,
    class EdgeProperty,
    class Tag>
  class Graph;

  /// empty class
  struct EmptyProperty
  {
  };

  /// default container_traits tag
  struct DefaultTraitTag;

  /// container_traits tells Graph how to manage Nodes,Edges,Sockets.
  /// Default implementation is specialized to std::vector as container, pointer
  /// type as descriptor.
  template <
    template <class>
    class ValueType,
    class GraphTraits,
    class Property,
    class Tag = DefaultTraitTag>
  struct container_traits;

  template <template <class> class ValueType, class GraphTraits, class Property>
  struct container_traits<ValueType, GraphTraits, Property, DefaultTraitTag>
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
      destroy(container_type &c, const descriptor_type &descriptor)
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
      access(container_type &c, const descriptor_type &descriptor)
    {
      (void)c;
      return *descriptor;
    }

    /// Access function.
    /// \param c Container.
    /// \param descriptor A valid descriptor.
    /// \returns reference of value type in the container.
    static inline const value_type &
      access(const container_type &c, const descriptor_type &descriptor)
    {
      (void)c;
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

    /// Access property.
    /// \param c Container
    /// \param p Inline property
    static inline property_type &
      property_access(container_type &c, const descriptor_type &d)
    {
      return access(c, d).inline_property();
    }

    /// Access property.
    /// \param c Container
    /// \param p Inline property
    static inline const property_type &
      property_access(const container_type &c, const descriptor_type &d)
    {
      return access(c, d).inline_property();
    }

    /// Access property.
    /// \param c Container
    /// \param p Inline property
    static inline property_type &
      property_at(container_type &c, const descriptor_type &d)
    {
      return at(c, d).inline_property();
    }

    /// Access property.
    /// \param c Container
    /// \param p Inline property
    static inline const property_type &
      property_at(const container_type &c, const descriptor_type &d)
    {
      return at(c, d).inline_property();
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
    DefaultTraitTag>
  {
    /// type
    using type =
      graph_traits<NodeProperty, SocketProperty, EdgeProperty, DefaultTraitTag>;

    /// graph type
    using graph_type =
      Graph<NodeProperty, SocketProperty, EdgeProperty, DefaultTraitTag>;

    /* primitive types */

    using node_type   = Node<type>;
    using edge_type   = NodeEdge<type>;
    using socket_type = NodeEdgeSocket<type>;

    /* property types */

    using node_property_type   = NodeProperty;
    using edge_property_type   = EdgeProperty;
    using socket_property_type = SocketProperty;

    /* container traits */

    using node_container_traits = //
      container_traits<           //
        Node,                     //
        type,                     //
        NodeProperty>;            //

    using edge_container_traits = //
      container_traits<           //
        NodeEdge,                 //
        type,                     //
        EdgeProperty>;            //

    using socket_container_traits = //
      container_traits<             //
        NodeEdgeSocket,             //
        type,                       //
        SocketProperty>;            //

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
  class Node
  {
  public:
    friend typename Traits::graph_type;

    /// Type of this class
    using type = Node<Traits>;
    /// Inline property type
    using inline_property_type =
      typename Traits::node_container_traits::inline_property_type;
    /// Descriptor type
    using descriptor_type = typename Traits::node_descriptor_type;

    /// A Constructor.
    /// \param args Arguments to initialize property class.
    template <class... Args>
    Node(Args &&... args)
      : m_sockets {}
      , m_inline_property {std::forward<Args>(args)...}
    {
    }

    /// A destructor.
    ~Node()
    {
    }

    /// Get sockets.
    auto sockets() const
    {
      return m_sockets;
    }

    /// Get socket count.
    auto n_sockets() const
    {
      return m_sockets.size();
    }

    /// Inline property accessor.
    const inline_property_type &inline_property() const
    {
      return m_inline_property;
    }

    /// Inline property accessor
    inline_property_type &inline_property()
    {
      return m_inline_property;
    }

  private:
    /// Set socket descriptor.
    /// \param descriptor descriptor of target socket
    /// \returns return true if success. false if invalid descriptor or it is
    /// already set.
    bool set_socket(const typename Traits::socket_descriptor_type &descriptor)
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
    void unset_socket(const typename Traits::socket_descriptor_type &descriptor)
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
    /// Socket descriptors.
    /// Shouldn't contain 2 or more same descriptors.
    std::vector<typename Traits::socket_descriptor_type> m_sockets;
    /// Inline property class instance
    inline_property_type m_inline_property;
  };

  /// \brief Edge class.
  /// this class represents single directed edge object in NodeGprah contains
  /// src,dst descriptors of socket. src and dst can contain same descriptor.
  /// constructed by 2 descriptors and should automatically be deleted when they
  /// deleted from Graph.
  template <typename Traits>
  class NodeEdge
  {
  public:
    friend typename Traits::graph_type;
    using type = NodeEdge<Traits>;
    using inline_property_type =
      typename Traits::edge_container_traits::inline_property_type;
    using descriptor_type = typename Traits::edge_descriptor_type;

    /// A constructor.
    /// \param src descriptor of source socket
    /// \param dst descriptor of destination socket
    /// \param args args to initialize property class instance
    template <class... Args>
    NodeEdge(
      const typename Traits::socket_descriptor_type &src,
      const typename Traits::socket_descriptor_type &dst,
      Args &&... args)
      : m_src {src}
      , m_dst {dst}
      , m_inline_property {std::forward<Args>(args)...}
    {
    }

    /// A destructor.
    ~NodeEdge()
    {
    }

    /// Get source socket.
    auto src() const
    {
      return m_src;
    }

    /// Get destination socket.
    auto dst() const
    {
      return m_dst;
    }

    /// Inline property accessor.
    const inline_property_type &inline_property() const
    {
      return m_inline_property;
    }

    /// Inline property accessor.
    inline_property_type &inline_property()
    {
      return m_inline_property;
    }

  private:
    /// socket descriptors
    typename Traits::socket_descriptor_type m_src, m_dst;
    /// inline property class instance
    inline_property_type m_inline_property;
  };

  /// \brief Socket class.
  /// This class represents socket object in Graph.
  /// contains list of Node descriptors and list of Edge descriptors.
  template <typename Traits>
  class NodeEdgeSocket
  {
  public:
    friend typename Traits::graph_type;
    using type = NodeEdgeSocket<Traits>;
    using inline_property_type =
      typename Traits::socket_container_traits::inline_property_type;
    using descriptor_type = typename Traits::edge_descriptor_type;

    /// A constructor.
    /// \param args Args for initialize property class
    template <class... Args>
    NodeEdgeSocket(Args &&... args)
      : m_inline_property {std::forward<Args>(args)...}
    {
    }

    /// A destructor
    ~NodeEdgeSocket()
    {
    }

    /// Get source edges.
    auto src_edges() const
    {
      return m_src_edges;
    }
    /// Get destination edges.
    auto dst_edges() const
    {
      return m_dst_edges;
    }

    /// Get srouce edge count.
    auto n_src_edges() const
    {
      return m_src_edges.size();
    }
    /// Get destination edge count.
    auto n_dst_edges() const
    {
      return m_dst_edges.size();
    }

    /// Get nodes.
    auto nodes() const
    {
      return m_nodes;
    }

    /// Get node count.
    auto n_nodes() const
    {
      return m_nodes.size();
    }

    /// Inline property accessor.
    const inline_property_type &inline_property() const
    {
      return m_inline_property;
    }

    /// Inline property accessor.
    inline_property_type &inline_property()
    {
      return m_inline_property;
    }

  private:
    /// Set src edge.
    /// \param descriptor descriptor of edge
    /// \returns true when success
    bool set_src_edge(const typename Traits::edge_descriptor_type &descriptor)
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
    bool set_dst_edge(const typename Traits::edge_descriptor_type &descriptor)
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
    bool set_node(const typename Traits::node_descriptor_type &descriptor)
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
    void unset_node(const typename Traits::node_descriptor_type &descriptor)
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
    void unset_src_edge(const typename Traits::edge_descriptor_type &descriptor)
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
    void unset_dst_edge(const typename Traits::edge_descriptor_type &descriptor)
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
    /// node descriptors
    std::vector<typename Traits::node_descriptor_type> m_nodes;
    /// edge descriptors
    std::vector<typename Traits::edge_descriptor_type> m_src_edges;
    /// edge descriptors
    std::vector<typename Traits::edge_descriptor_type> m_dst_edges;
    /// inline property class instance
    inline_property_type m_inline_property;
  };

  /// \brief Node Graph class.
  /// This class represents Node Graph Object.
  template <
    class NodeProperty   = EmptyProperty,
    class SocketProperty = EmptyProperty,
    class EdgeProperty   = EmptyProperty,
    class Tag            = DefaultTraitTag>
  class Graph
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

    /// A constructor
    Graph()
    {
    }

    /// Use copy functions to copy instances of Graph
    Graph(const Graph &) = delete;

    /// Move constructor
    Graph(Graph &&g)
    {
      m_nodes   = std::move(g.m_nodes);
      m_edges   = std::move(g.m_edges);
      m_sockets = std::move(g.m_sockets);
    }

    /// A destructor
    ~Graph()
    {
      clear();
      assert(empty());
    }

    /// Add node.
    /// \param args Args to initialize property
    /// \returns descriptor of new node
    template <class... Args>
    node_descriptor_type add_node(Args &&... args)
    {
      // add node
      return _create_n(std::forward<Args>(args)...);
    }

    /// Add socket.
    /// \param args Args to initialize property
    /// \returns descriptor of new socket
    template <class... Args>
    socket_descriptor_type add_socket(Args &&... args)
    {
      // add socket
      return _create_s(std::forward<Args>(args)...);
    }

    /// Add edge.
    /// \param src source socket
    /// \param dst destination socket
    /// \param args Args to initialize property class
    template <class... Args>
    edge_descriptor_type add_edge(
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

      auto e = _create_e(src, dst, std::forward<Args>(args)...);

      // set edge to sockets
      _access(src).set_src_edge(e);
      _access(dst).set_dst_edge(e);

      return e;
    }

    /// Remove node.
    /// \param descriptor descriptor of node
    /// \returns true if success.
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
    /// \returns true if sccess.
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
    /// \returns true if success
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
    bool exists(const node_descriptor_type &descriptor) const
    {
      return traits::node_container_traits::exists(m_nodes, descriptor);
    }

    /// Check if edge descriptor exists
    /// \param descriptor descriptor of edge
    /// \returns true if success
    bool exists(const edge_descriptor_type &descriptor) const
    {
      return traits::edge_container_traits::exists(m_edges, descriptor);
    }

    /// Check if socket descriptor exists
    /// \param descriptor descriptor of socket
    /// \returns true if success
    bool exists(const socket_descriptor_type &descriptor) const
    {
      return traits::socket_container_traits::exists(m_sockets, descriptor);
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of node
    /// \returns if success
    NodeProperty &operator[](const node_descriptor_type &descriptor)
    {
      return traits::node_container_traits::property_access(
        m_nodes, descriptor);
    }
    /// Descriptor access operator
    /// \param descriptor descriptor of node
    /// \returns if descriptor is invalid, return value is undefined.
    const NodeProperty &operator[](const node_descriptor_type &descriptor) const
    {
      return traits::node_container_traits::property_access(
        m_nodes, descriptor);
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of edge
    /// \returns if success
    EdgeProperty &operator[](const edge_descriptor_type &descriptor)
    {
      return traits::edge_container_traits::property_access(
        m_edges, descriptor);
    }
    /// Descriptor access operator.
    /// \param descriptor descriptor of edge
    /// \returns if success
    const EdgeProperty &operator[](const edge_descriptor_type &descriptor) const
    {
      return traits::edge_container_traits::property_access(
        m_edges, descriptor);
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    SocketProperty &operator[](const socket_descriptor_type &descriptor)
    {
      return traits::socket_container_traits::property_access(
        m_sockets, descriptor);
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    const SocketProperty &
      operator[](const socket_descriptor_type &descriptor) const
    {
      return traits::socket_container_traits::property_access(
        m_sockets, descriptor);
    }

    // Descriptor access operator.
    // \param descriptor descriptor of node
    // \returns if success
    NodeProperty &at(const node_descriptor_type &descriptor)
    {
      return traits::node_container_traits::property_at(m_nodes, descriptor);
    }
    /// Descriptor access operator.
    /// \param descriptor descriptor of node
    /// \returns if descriptor is invalid, return value is undefined.
    const NodeProperty &at(const node_descriptor_type &descriptor) const
    {
      return traits::node_container_traits::property_at(m_nodes, descriptor);
    }

    /// Descriptor access operator
    /// \param descriptor descriptor of edge
    /// \returns if success
    EdgeProperty &at(const edge_descriptor_type &descriptor)
    {
      return traits::edge_container_traits::property_at(m_edges, descriptor);
    }
    /// Descriptor access operator.
    /// \param descriptor descriptor of edge
    /// \returns if success
    const EdgeProperty &at(const edge_descriptor_type &descriptor) const
    {
      return traits::edge_container_traits::property_at(m_edges, descriptor);
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    SocketProperty &at(const socket_descriptor_type &descriptor)
    {
      return traits::socket_container_traits::property_access(
        m_sockets, descriptor);
    }

    /// Descriptor access operator.
    /// \param descriptor descriptor of socket
    /// \returns if success
    const SocketProperty &at(const socket_descriptor_type &descriptor) const
    {
      return traits::socket_container_traits::property_at(
        m_sockets, descriptor);
    }

    /// Attach socket.
    /// \param node node descriptor
    /// \param socket socket descriptor
    bool attach_socket(
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
    auto nodes() const
    {
      return m_nodes;
    }

    /// Get node count.
    auto n_nodes() const
    {
      return m_nodes.size();
    }

    /// Get nodes connected to the socket.
    std::vector<node_descriptor_type>
      nodes(const socket_descriptor_type &descriptor) const
    {
      auto &s = _at(descriptor);
      return s.nodes();
    }

    /// Get number of connected nodes to the socket.
    auto n_nodes(const socket_descriptor_type &descriptor) const
    {
      const auto &s = _at(descriptor);
      return s.n_nodes();
    }

    /// Get sockets.
    auto sockets() const
    {
      return m_sockets;
    }

    /// Get socket count.
    auto n_sockets() const
    {
      return m_sockets.size();
    }

    /// Get sockets connected to the node.
    std::vector<socket_descriptor_type>
      sockets(const node_descriptor_type &descriptor) const
    {
      const auto &n = _at(descriptor);
      return n.sockets();
    }

    /// Get number of connected sockets to the node.
    auto n_sockets(const node_descriptor_type &descriptor) const
    {
      const auto &n = _at(descriptor);
      return n.n_sockets();
    }

    /// Get edges.
    auto edges() const
    {
      return m_edges;
    }

    /// Get edge count.
    auto n_edges() const
    {
      return m_edges.size();
    }

    /// Get src edges.
    std::vector<edge_descriptor_type>
      src_edges(const socket_descriptor_type &descriptor) const
    {
      auto &s = _at(descriptor);
      return s.src_edges();
    }

    /// Get number of src edges.
    auto n_src_edges(const socket_descriptor_type &descriptor) const
    {
      const auto &s = _at(descriptor);
      return s.n_src_edges();
    }

    /// Get dst edges.
    std::vector<edge_descriptor_type>
      dst_edges(const socket_descriptor_type &descriptor) const
    {
      auto &s = _at(descriptor);
      return s.dst_edges();
    }

    /// Get number of dst edges.
    auto n_dst_edges(const socket_descriptor_type &descriptor) const
    {
      const auto &s = _at(descriptor);
      return s.n_dst_edges();
    }

    /// Get src socket.
    socket_descriptor_type src(const edge_descriptor_type &descriptor) const
    {
      auto &e = _at(descriptor);
      return e.src();
    }

    /// Get dst socket.
    socket_descriptor_type dst(const edge_descriptor_type &descriptor) const
    {
      auto &e = _at(descriptor);
      return e.dst();
    }

    /// Clone graph
    Graph<NodeProperty, SocketProperty, EdgeProperty> clone() const
    {
      Graph ng;
      auto &n_src = m_nodes;
      auto &s_src = m_sockets;
      auto &e_src = m_edges;

      std::vector<std::pair<node_descriptor_type, node_descriptor_type>> n_map;
      std::vector<std::pair<socket_descriptor_type, socket_descriptor_type>>
        s_map;

      for (auto &&n : n_src) {
        auto d = ng.add_node((*this)[n]);
        n_map.emplace_back(n, d);
      }

      for (auto &&s : s_src) {
        auto d = ng.add_socket((*this)[s]);
        s_map.emplace_back(s, d);
      }

      // attach sockets
      for (auto &&np : n_map) {
        auto &&[nsrc, ndst] = np;
        auto ss             = sockets(nsrc);
        for (auto &&s : ss) {
          for (auto &&sp : s_map) {
            auto &&[ssrc, sdst] = sp;
            if (ssrc == s) {
              ng.attach_socket(ndst, sdst);
              break;
            }
          }
        }
      }

      // add edge
      for (auto &&e : e_src) {
        auto ssrc = src(e);
        auto sdst = dst(e);

        for (auto &&sp1 : s_map) {
          auto &&[from_1, to_1] = sp1;
          if (ssrc == from_1) {
            for (auto &&sp2 : s_map) {
              auto [from_2, to_2] = sp2;
              if (sdst == from_2)
                ng.add_edge(to_1, to_2, (*this)[e]);
            }
          }
        }
      }
      return ng;
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
    bool empty()
    {
      return (m_nodes.empty() && m_edges.empty() && m_sockets.empty());
    }

  private:
    /// Create new node.
    /// \param args arguments to construct node property
    template <class... Args>
    inline node_descriptor_type _create_n(Args &&... args)
    {
      return traits::node_container_traits::create(
        m_nodes, std::forward<Args>(args)...);
    }

    /// Create new edge.
    /// \param args arguments to construct node property.
    template <class... Args>
    inline edge_descriptor_type _create_e(Args &&... args)
    {
      return traits::edge_container_traits::create(
        m_edges, std::forward<Args>(args)...);
    }

    /// Create new socket.
    /// \param args arguments to construct node property
    template <class... Args>
    inline socket_descriptor_type _create_s(Args &&... args)
    {
      return traits::socket_container_traits::create(
        m_sockets, std::forward<Args>(args)...);
    }

    /// Node access.
    /// \param n if nonexist or invalid, behaviour of this function is undefined
    inline node_type &_access(const node_descriptor_type &n)
    {
      return traits::node_container_traits::access(m_nodes, n);
    }

    inline const node_type &_access(const node_descriptor_type &n) const
    {
      return traits::node_container_traits::access(m_nodes, n);
    }

    /// Edge access.
    /// \param n if nonexist or invalid, behaviour of this function is undefined
    inline edge_type &_access(const edge_descriptor_type &e)
    {
      return traits::edge_container_traits::access(m_edges, e);
    }

    inline const edge_type &_access(const edge_descriptor_type &e) const
    {
      return traits::edge_container_traits::access(m_edges, e);
    }

    /// Socket access.
    /// \param n if nonexist or invalid, behaviour of this function is undefined
    inline socket_type &_access(const socket_descriptor_type &s)
    {
      return traits::socket_container_traits::access(m_sockets, s);
    }

    inline const socket_type &_access(const socket_descriptor_type &s) const
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
    void _destroy(const node_descriptor_type &descriptor)
    {
      traits::node_container_traits::destroy(m_nodes, descriptor);
    }

    /// Destroy edge.
    /// \param descriptor descriptor of edge
    /// \returns true if success
    void _destroy(const edge_descriptor_type &descriptor)
    {
      traits::edge_container_traits::destroy(m_edges, descriptor);
    }

    /// Destroy edge.
    /// \param descriptor descriptor of socket
    /// \returns true if success
    void _destroy(const socket_descriptor_type &descriptor)
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