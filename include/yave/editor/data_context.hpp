//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/editor/data_command.hpp>

#include <memory>

namespace yave::editor {

  struct editor_data;

  /// Application data context
  class data_context
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    // ctor
    data_context();
    // dtor
    ~data_context() noexcept;

  public:
    /// exec
    void exec(std::unique_ptr<data_command>&& op);
    /// undo
    void undo();
    /// redo
    void redo();

  public:
    /// aquire data lock
    [[nodiscard]] auto lock() const -> std::unique_lock<std::mutex>;

  public:
    /// get data
    auto data() const -> const editor_data&;
    /// get data
    auto data() -> editor_data&;
  };
}