//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/lib/unique_any/unique_any.hpp>

#include <string>
#include <memory>

namespace yave::compiler {

  /// compiler pipeline
  class pipeline
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

    void _add_data_any(const std::string& key, unique_any val);
    void _remove_data_any(const std::string& key);
    auto _get_data_any(const std::string& key) -> unique_any*;

  public:
    /// initialize pipeline with success state
    pipeline();
    /// dtor
    ~pipeline() noexcept;
    /// move ctor
    pipeline(pipeline&&) noexcept;

  public:
    /// success?
    bool success() const;
    /// set failed
    void set_failed();

  public:
    /// Add data
    template <class T>
    void add_data(const std::string& key, T&& value)
    {
      _add_data_any(key, unique_any(std::forward<T>(value)));
    }

    /// Remove data
    void remove_data(const std::string& key)
    {
      _remove_data_any(key);
    }

    /// Get data
    template <class T>
    auto get_data_if(const std::string& key) -> T*
    {
      if (auto p = _get_data_any(key)) {
        return unique_any_cast<T>(p);
      }
      return nullptr;
    }

    /// Get data
    template <class T>
    auto& get_data(const std::string& key)
    {
      if (auto p = _get_data_any(key)) {
        return unique_any_cast<T>(*p);
      }
      throw std::out_of_range("node_compile_pipeline: data not found");
    }

  public:
    /// monadic 'then'
    template <class F>
    auto& and_then(F&& f) &
    {
      if (success()) {
        f(*this);
      }
      return *this;
    }

    /// monadic 'else'
    template <class F>
    auto& or_else(F&& f) &
    {
      if (!success()) {
        f(*this);
      }
      return *this;
    }

    /// apply
    template <class F>
    auto& apply(F&& f) &
    {
      f(*this);
      return *this;
    }
  };

} // namespace yave