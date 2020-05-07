//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/editor/data_command.hpp>
#include <yave/editor/editor_data.hpp>

#include <memory>

namespace yave::editor {

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
    /// access data
    auto editor_data() const -> const editor::editor_data&;
    /// access data
    auto editor_data() -> editor::editor_data&;
  };
}