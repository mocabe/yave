//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/lib/graph/graph.hpp>

using namespace yave::graph;

TEST_CASE("Graph init", "[lib][graph]")
{
  SECTION("Graph<>")
  {
    using ng = graph<>;
    ng g;
    REQUIRE(g.nodes().empty());
    REQUIRE(g.sockets().empty());
    REQUIRE(g.edges().empty());
  }
  SECTION("Graph<int, int, int>")
  {
    using ng = graph<int, int, int>;
    ng g;
    REQUIRE(g.nodes().empty());
    REQUIRE(g.sockets().empty());
    REQUIRE(g.edges().empty());
  }
  SECTION("Graph<int, double, bool>")
  {
    using ng = graph<int, double, bool>;
    ng g;
    REQUIRE(g.nodes().empty());
    REQUIRE(g.sockets().empty());
    REQUIRE(g.edges().empty());
  }
}

TEST_CASE("Graph control", "[lib][graph]")
{
  graph<> g;

  SECTION("node")
  {
    auto n = g.add_node();

    SECTION("add_node")
    {
      REQUIRE(g.exists(n));
      REQUIRE(g.sockets(n).empty());
    }

    SECTION("remove_node")
    {
      g.remove_node(n);
      REQUIRE(g.nodes().empty());
    }
  }

  SECTION("socket")
  {
    auto s = g.add_socket();

    SECTION("add_socket")
    {
      REQUIRE(g.exists(s));
      REQUIRE(g.nodes(s).empty());
      REQUIRE(g.src_edges(s).empty());
      REQUIRE(g.dst_edges(s).empty());
    }

    SECTION("remove_socket")
    {
      g.remove_socket(s);
      REQUIRE(g.sockets().empty());
    }
  }

  SECTION("attach")
  {
    auto n = g.add_node();
    auto s = g.add_socket();

    SECTION("attach_socket")
    {
      REQUIRE(g.attach_socket(n, s));
      REQUIRE(g.sockets(n).size() == 1);
      REQUIRE(g.nodes(s).size() == 1);
      REQUIRE(g.sockets(n)[0] == s);
      REQUIRE(g.nodes(s)[0] == n);

      REQUIRE(g.attach_socket(n, s));
      REQUIRE(g.sockets(n).size() == 1);
      REQUIRE(g.nodes(s).size() == 1);
    }

    SECTION("remove_node")
    {
      g.remove_node(n);
      REQUIRE_FALSE(g.exists(n));
      REQUIRE(g.nodes().empty());
      REQUIRE(g.nodes(s).empty());
    }
  }

  SECTION("edge")
  {
    auto s0 = g.add_socket();
    auto s1 = g.add_socket();
    auto e  = g.add_edge(s0, s1);

    SECTION("add_edge")
    {
      REQUIRE(g.exists(e));
      REQUIRE(g.src(e) == s0);
      REQUIRE(g.dst(e) == s1);

      REQUIRE(g.src_edges(s0).size() == 1);
      REQUIRE(g.dst_edges(s0).size() == 0);
      REQUIRE(g.src_edges(s1).size() == 0);
      REQUIRE(g.dst_edges(s1).size() == 1);
    }

    SECTION("remove_edge")
    {
      auto loop = g.add_edge(s1, s1);
      g.remove_edge(e);
      g.remove_edge(loop);
      REQUIRE(g.src_edges(s0).empty());
      REQUIRE(g.dst_edges(s0).empty());
      REQUIRE(g.src_edges(s1).empty());
      REQUIRE(g.dst_edges(s1).empty());
    }

    SECTION("remove_socket")
    {
      auto n0 = g.add_node();
      auto n1 = g.add_node();
      REQUIRE(g.attach_socket(n0, s0));
      REQUIRE(g.attach_socket(n1, s1));

      g.remove_socket(s0);
      REQUIRE_FALSE(g.exists(s0));
      REQUIRE_FALSE(g.exists(e));
      REQUIRE(g.sockets(n0).size() == 0);
      REQUIRE(g.sockets(n1).size() == 1);
      REQUIRE(g.src_edges(g.sockets(n1)[0]).empty());
      REQUIRE(g.dst_edges(g.sockets(n1)[0]).empty());
    }
  }
  SECTION("clear")
  {
    // setup
    auto n0 = g.add_node();
    auto n1 = g.add_node();
    auto s0 = g.add_socket();
    auto s1 = g.add_socket();
    auto s2 = g.add_socket();

    REQUIRE(g.attach_socket(n0, s0));
    REQUIRE(g.attach_socket(n0, s1));
    REQUIRE(g.attach_socket(n0, s2));
    REQUIRE(g.attach_socket(n1, s2));

    auto e0 [[maybe_unused]] = g.add_edge(s0, s1);
    auto e1 [[maybe_unused]] = g.add_edge(s1, s0);
    auto e2 [[maybe_unused]] = g.add_edge(s0, s2);
    auto e3 [[maybe_unused]] = g.add_edge(s2, s0);
    auto e4 [[maybe_unused]] = g.add_edge(s2, s2);

    // test
    g.clear();

    REQUIRE(g.nodes().empty());
    REQUIRE(g.sockets().empty());
    REQUIRE(g.edges().empty());
  }
}

TEST_CASE("Graph property", "[lib][graph]")
{
  graph<std::string, int, int> g;
  auto n = g.add_node("test");
  auto s = g.add_socket(42);
  auto e = g.add_edge(s, s, 123);
  REQUIRE(g[n] == "test");
  REQUIRE(g[s] == 42);
  REQUIRE(g[e] == 123);
}

TEST_CASE("Graph clone", "[lib][graph]")
{
  graph<std::string, int, int> g;
  auto n  = g.add_node("test");
  auto s1 = g.add_socket(1);
  auto s2 = g.add_socket(2);

  auto e1 [[maybe_unused]] = g.add_edge(s1, s2, 1);
  auto e2 [[maybe_unused]] = g.add_edge(s2, s1, 2);
  auto e3 [[maybe_unused]] = g.add_edge(s1, s1, 3);

  REQUIRE(g.attach_socket(n, s1));
  REQUIRE(g.attach_socket(n, s2));

  REQUIRE(g.sockets(g.nodes()[0]).size() == 2);

  auto cpy     = g.clone();
  auto nodes   = cpy.nodes();
  auto sockets = cpy.sockets();
  auto edges   = cpy.edges();

  REQUIRE(cpy.n_nodes() == g.n_nodes());
  REQUIRE(cpy.n_sockets() == g.n_sockets());
  REQUIRE(cpy.n_edges() == g.n_edges());

  REQUIRE(cpy[cpy.nodes()[0]] == "test");

  auto s = cpy.sockets(cpy.nodes()[0]);
  REQUIRE(cpy.sockets(cpy.nodes()[0]).size() == 2);
  REQUIRE(cpy[s[0]] == g[g.sockets()[0]]);
  REQUIRE(cpy[s[1]] == g[g.sockets()[1]]);

  REQUIRE(cpy.src_edges(cpy.sockets()[0]).size() == g.src_edges(g.sockets()[0]).size());
  REQUIRE(cpy.dst_edges(cpy.sockets()[0]).size() == g.dst_edges(g.sockets()[0]).size());
  REQUIRE(cpy.src_edges(cpy.sockets()[1]).size() == g.src_edges(g.sockets()[1]).size());
  REQUIRE(cpy.dst_edges(cpy.sockets()[1]).size() == g.dst_edges(g.sockets()[1]).size());
}

TEST_CASE("Graph id", "[lib][graph]")
{
  graph<> g;
  auto n = g.add_node();
  auto s = g.add_socket();
  auto d = g.add_socket();
  auto e = g.add_edge(s, d);
  REQUIRE(g.node(g.id(n)) == n);
  REQUIRE(g.socket(g.id(s)) == s);
  REQUIRE(g.socket(g.id(d)) == d);
  REQUIRE(g.edge(g.id(e)) == e);
}