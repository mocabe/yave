//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <string>
#include <memory>
#include <vector>
#include <typeinfo>

namespace yave {

  /// Polymorphic error info interface.
  class error_info_base
  {
  protected:
    error_info_base()                       = default;
    error_info_base(const error_info_base&) = default;

  public:
    /// Generate error message.
    virtual auto message() const -> std::string = 0;
    /// type info
    virtual auto type() const -> const std::type_info& = 0;
    /// clone this error
    virtual auto clone() const -> std::unique_ptr<error_info_base> = 0;
    /// dtor
    virtual ~error_info_base() noexcept;
  };

  /// Base class of errorInfo objects.
  template <class Derived>
  struct error_info : error_info_base
  {
    [[nodiscard]] auto type() const -> const std::type_info& override
    {
      return typeid(Derived);
    }

    [[nodiscard]] auto clone() const
      -> std::unique_ptr<error_info_base> override
    {
      return std::make_unique<Derived>(*static_cast<const Derived*>(this));
    }
  };

  /// error
  class error
  {
  public:
    /// Initialize error with success.
    error();

    /// Initialize error with success.
    error(nullptr_t);

    /// Initialize error with error info.
    error(std::unique_ptr<error_info_base>&& err);

    /// Move ctor.
    error(error&& other);

    /// operator=
    error& operator=(error&&);

    /// Copy constructor (deleted)
    error(const error&) = delete;
    /// Copy assignment operator (deleted)
    error& operator=(const error&) = delete;

    /// Initialize from success
    explicit error(const class success&);

    /// Initialize from success
    error& operator=(const class success&);

    /// operator bool
    [[nodiscard]] explicit operator bool() const;

    /// Get pointer to errorInfo.
    [[nodiscard]] auto info() const -> const error_info_base*;

    /// success?
    [[nodiscard]] bool is_success() const;

    /// Get error message
    [[nodiscard]] auto message() const -> std::string;

    /// Get type_info
    [[nodiscard]] auto type() const -> const std::type_info&;

    /// Clone
    [[nodiscard]] auto clone() const -> error;

  private:
    std::unique_ptr<const error_info_base> m_error_info;
  };

  /// success
  class success : public error
  {
  public:
    /// Ctor.
    success();
    /// Copy ctor.
    success(const success& other);
    /// Move ctor.
    success(const success&& other);
    /// Copy assignment operator.
    success& operator=(const success& other);
    /// Move assignment operator.
    success& operator=(success&& other);

  private:
    using error::error;
  };

  /// Simple list of error objects.
  class error_list
  {
  public:
    /// iterator
    using iterator = typename std::vector<error>::iterator;
    /// const_iterator
    using const_iterator = typename std::vector<error>::const_iterator;

    /// Initialize error_list with empty list.
    error_list();

    /// Move constructor.
    error_list(error_list&& other);

    /// Move assignment operator.
    error_list& operator=(error_list&&);

    /// Copy ctor (deleted)
    error_list(const error_list&) = delete;

    /// Copy assignment operator (deleted)
    error_list& operator=(const error_list&) = delete;

    /// Add error to the list.
    void push_back(error&& error);

    /// Get size of list.
    [[nodiscard]] size_t size() const;

    /// Empty?
    [[nodiscard]] bool empty() const;

    /// operator[]
    [[nodiscard]] auto operator[](size_t index) const -> const error&;

    /// at
    [[nodiscard]] auto at(size_t index) const -> const error&;

    /// begin
    [[nodiscard]] auto begin() const -> const_iterator;

    /// end
    [[nodiscard]] auto end() const -> const_iterator;

    /// begin
    [[nodiscard]] auto begin() -> iterator;

    /// end
    [[nodiscard]] auto end() -> iterator;

    /// Erase element.
    void erase(const_iterator position);

    /// Erase range of elements.
    void erase(const_iterator fist, const_iterator last);

    /// Clear error list
    void clear();

    /// Clone errors
    [[nodiscard]] auto clone() const -> error_list;

  private:
    std::vector<error> m_errors;
  };

  /// Make error from new ErrorInfo instance.
  template <class ErrorInfoT, class... Args>
  [[nodiscard]] auto make_error(Args&&... args) -> error
  {
    return error(std::make_unique<ErrorInfoT>(std::forward<Args>(args)...));
  }

} // namespace yave