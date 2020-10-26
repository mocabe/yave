//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/unique_any/unique_any.hpp>
#include <yave/lib/util/locked_reference.hpp>

#include <mutex>
#include <stdexcept>

namespace yave::editor {

  class data_context;
  class data_command;

  /// unexpected thread failure
  class data_thread_failure : std::runtime_error
  {
    std::exception_ptr m_exception;

  public:
    data_thread_failure(std::exception_ptr exception)
      : std::runtime_error("data thread terminated by uncaught exception")
      , m_exception {exception}
    {
    }

    /// get exception thrown in data thread
    [[nodiscard]] auto exception() const
    {
      return m_exception;
    }
  };

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
    void _add_data(unique_any data);

    /// remove data.
    /// \param id typeid of data to be removed
    void _remove_data(const std::type_info& id);

  public:
    /// add new data
    void add_data(unique_any data)
    {
      _add_data(std::move(data));
    }

    /// remove data
    template <class T>
    void remove_data()
    {
      _remove_data(typeid(T));
    }

  private:
    // internal data holder
    struct data_holder
    {
      // mutex
      std::mutex mtx = {};
      // data
      unique_any data;

      data_holder(unique_any d)
        : data {std::move(d)}
      {
      }
    };

    /// data access
    auto _get_data(const std::type_info& id) const
      -> std::shared_ptr<data_holder>;

    /// create locked data reference
    template <class T>
    auto _get_data_ref() const
    {
      if (auto p = _get_data(typeid(T))) {

        auto lck = std::unique_lock(p->mtx);

        if (auto d = unique_any_cast<T>(&p->data))
          return shared_locked_reference(
            std::shared_ptr<T>(p, d), // share lifetime with data_holder
            std::move(lck));
      }

      throw std::runtime_error("data_context: data not found");
    }

  public:
    template <class T>
    auto get_data()
    {
      return _get_data_ref<T>();
    }

    template <class T>
    auto get_data() const
    {
      return _get_data_ref<std::add_const_t<T>>();
    }
  };

} // namespace yave::editor