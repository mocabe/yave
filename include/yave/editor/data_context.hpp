//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/editor/data_command.hpp>

#include <memory>

namespace yave::editor {

  struct editor_data;
  class data_context;

  /// Access proxy for data_context with unique lock
  class data_context_access
  {
    friend class data_context;

    // ref
    data_context& m_ctx;
    // lock
    std::unique_lock<std::mutex> m_lck;

    data_context_access(data_context& ctx, std::unique_lock<std::mutex> lck)
      : m_ctx {ctx}
      , m_lck {std::move(lck)}
    {
    }

  public:
    // move
    data_context_access(data_context_access&&) noexcept = default;
    // get data ref
    auto data() -> editor_data&;
  };

  /// Access proxy for data_context with unique lock
  class const_data_context_access
  {
    friend class data_context;

    // ref
    const data_context& m_ctx;
    // lock
    std::unique_lock<std::mutex> m_lck;

    const_data_context_access(
      const data_context& ctx,
      std::unique_lock<std::mutex> lck)
      : m_ctx {ctx}
      , m_lck {std::move(lck)}
    {
    }

  public:
    // move
    const_data_context_access(const_data_context_access&&) noexcept = default;
    // get data ref
    auto data() const -> const editor_data&;
  };

  /// Application data context
  class data_context
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

    friend class data_context_access;
    friend class const_data_context_access;

  public:
    // ctor
    data_context();
    // dtor
    ~data_context() noexcept;

  public:
    /// exec
    void exec(std::unique_ptr<data_command>&& op) const;
    /// undo
    void undo();
    /// redo
    void redo();

  private:
    /// data access
    auto data() const -> const editor_data&;
    /// data access
    auto data() -> editor_data&;

  public:
    /// aquire data lock
    [[nodiscard]] auto lock() const -> const_data_context_access;
    /// aquire  data lock
    [[nodiscard]] auto lock() -> data_context_access;
  };

  inline auto const_data_context_access::data() const -> const editor_data&
  {
    return m_ctx.data();
  }

  inline auto data_context_access::data() -> editor_data&
  {
    return m_ctx.data();
  }
} // namespace yave::editor