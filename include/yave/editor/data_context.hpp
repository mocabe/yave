//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/editor/unique_context_data.hpp>

#include <mutex>

namespace yave::editor {

  struct editor_data;
  class data_context;
  class data_command;

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

    // new data
    void add_data(unique_context_data new_data);

    // remove data
    template <class T>
    void remove_data();

    // get data by casting
    template <class T>
    auto get_data() -> T&;
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

    // get data by casting
    template <class T>
    auto get_data() -> const T&;
  };

  /// Application data context
  class data_context
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

    friend class data_context_access;
    friend class const_data_context_access;

  public:
    /// access types
    using accessor       = data_context_access;
    using const_accessor = const_data_context_access;

  public:
    // ctor
    data_context();
    // dtor
    ~data_context() noexcept;

  public:
    /// post command
    void cmd(std::unique_ptr<data_command>&& op) const;
    /// undo
    void undo();
    /// redo
    void redo();

  private:
    /// add new data.
    /// \param new data to be added
    /// \requires data.type() should be unique in context
    void add_data(unique_context_data data);

    /// remove data.
    /// \param id typeid of data to be removed
    void remove_data(const std::type_info& id);

    /// data access
    auto get_data(const std::type_info& id) const -> const void*;
    /// data access
    auto get_data(const std::type_info& id) -> void*;

  public:
    /// aquire data lock
    [[nodiscard]] auto lock() const -> const_data_context_access;
    /// aquire  data lock
    [[nodiscard]] auto lock() -> data_context_access;
  };

  inline void data_context_access::add_data(unique_context_data new_data)
  {
    m_ctx.add_data(std::move(new_data));
  }

  template <class T>
  void data_context_access::remove_data()
  {
    m_ctx.remove_data(typeid(std::decay_t<T>));
  }

  template <class T>
  auto data_context_access::get_data() -> T&
  {
    if (auto p = m_ctx.get_data(typeid(std::decay_t<T>)))
      return *static_cast<T*>(p);

    throw std::range_error("requested data type does not exist");
  }

  template <class T>
  auto const_data_context_access::get_data() -> const T&
  {
    if (auto p = m_ctx.get_data(typeid(std::decay_t<T>)))
      return *static_cast<const T*>(p);

    throw std::range_error("requested data type does not exist");
  }

} // namespace yave::editor