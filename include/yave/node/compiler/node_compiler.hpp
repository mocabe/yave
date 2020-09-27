//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/compiler/compile_result.hpp>
#include <yave/node/compiler/executable.hpp>
#include <yave/node/compiler/type.hpp>
#include <yave/node/parser/node_parser.hpp>
#include <yave/node/core/socket_instance_manager.hpp>
#include <yave/node/core/node_definition_store.hpp>
#include <yave/node/core/node_declaration_store.hpp>

namespace yave {

  class node_compiler;

  class node_compiler_result
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    node_compiler_result();
    ~node_compiler_result() noexcept;
    node_compiler_result(node_compiler_result&&) noexcept;
    node_compiler_result& operator=(node_compiler_result&&) noexcept;

  public:
    /// success?
    bool success() const;

    /// clone executable if exists
    auto clone_executable() const -> std::optional<executable>;

    /// filtering results
    auto get_errors() const -> std::vector<compile_result>;
    auto get_warnings() const -> std::vector<compile_result>;
    auto get_infos() const -> std::vector<compile_result>;

    /// get reuslts associated to node.
    /// \param ng reference node graph
    /// \param n  target node
    auto get_results(const structured_node_graph& ng, const node_handle& n)
      const -> std::vector<compile_result>;

    /// get reuslt associated to socket.
    /// \param ng referenec node graph
    /// \param s target socket
    auto get_results(const structured_node_graph& ng, const socket_handle& s)
      const -> std::vector<compile_result>;

  private:
    friend class node_compiler;

    // add result
    void add_result(compile_result r);

    // set executable
    void set_executable(executable&& ng);
  };

  class node_compiler
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    struct params
    {
      /// node graph
      structured_node_graph node_graph;
      /// definitions
      node_definition_store node_defs;
    };

  private:
    static auto _init_params() -> params
    {
      return {};
    }

  public:
    /// Ctor
    node_compiler();
    /// Dtor
    ~node_compiler() noexcept;

    /// Compile parsed V2 node graph
    [[nodiscard]] auto compile(params p = _init_params())
      -> node_compiler_result;
  };

} // namespace yave