//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/parser/parse_result.hpp>
#include <yave/node/core/structured_node_graph.hpp>
#include <yave/support/error.hpp>

#include <optional>

namespace yave {

  // fwd
  class node_parser;

  /// parse result
  class node_parser_result
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// defualt init
    node_parser_result();
    /// dtor
    ~node_parser_result() noexcept;
    /// move
    node_parser_result(node_parser_result&&) noexcept;
    /// move assign
    node_parser_result& operator=(node_parser_result&&) noexcept;

  public:
    /// move out node graph
    auto take_node_graph() -> std::optional<structured_node_graph>;

    /// has error?
    bool has_error() const;

    /// filtering results
    auto get_errors() const -> std::vector<parse_result>;
    auto get_warnings() const -> std::vector<parse_result>;
    auto get_infos() const -> std::vector<parse_result>;

    /// get reuslts associated to node.
    /// \param ng reference node graph
    /// \param n  target node
    auto get_results(const structured_node_graph& ng, const node_handle& n)
      const -> std::vector<parse_result>;

    /// get reuslt associated to socket.
    /// \param ng referenec node graph
    /// \param s target socket
    auto get_results(const structured_node_graph& ng, const socket_handle& s)
      const -> std::vector<parse_result>;

  private:
    /// parser uses private functions to add data.
    friend class node_parser;

    /// add result
    void add_result(parse_result r);
    /// set node graph
    void set_node_graph(structured_node_graph&& ng);
  };

  /// Parse node graph and generate data for compiler stage.
  class node_parser
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// parser parameters
    struct params
    {
      /// node graph
      structured_node_graph node_graph;
      /// output socket of node graph
      socket_handle output_socket;
    };

  private:
    static auto _init_params() -> params
    {
      return {};
    }

  public:
    /// Default ctor.
    node_parser();
    /// Dtor
    ~node_parser() noexcept;

  public:
    /// Parse node graph
    /// \param param parser parameters
    [[nodiscard]] auto parse(params p = _init_params()) -> node_parser_result;
  };
} // namespace yave