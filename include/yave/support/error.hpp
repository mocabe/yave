//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>

#include <string>
#include <memory>
#include <vector>
#include <typeinfo>

namespace yave {

  /// Polymorphic error info interface.
  class error_info_base
  {
  public:
    /// Generate error message.
    virtual std::string message() const = 0;
    /// type info
    virtual const std::type_info& type() const = 0;
    /// clone this error
    virtual std::unique_ptr<error_info_base> clone() const = 0;
    /// dtor
    virtual ~error_info_base() noexcept;
  };

  /// Base class of errorInfo objects.
  template <class Derived>
  struct error_info : error_info_base
  {
    virtual std::string message() const override
    {
      return "error_info";
    }

    virtual const std::type_info& type() const override
    {
      return typeid(Derived);
    }

    virtual std::unique_ptr<error_info_base> clone() const override
    {
      return std::make_unique<Derived>(*static_cast<const Derived*>(this));
    }

    virtual ~error_info() noexcept override
    {
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

    /// Get pointer to errorInfo.
    [[nodiscard]] const error_info_base* info() const;

    /// success?
    [[nodiscard]] bool is_success() const;

    /// operator bool
    [[nodiscard]] explicit operator bool() const;

    /// Get error message
    [[nodiscard]] std::string message() const;

    /// Get type_info
    [[nodiscard]] const std::type_info& type() const;

    /// Clone
    [[nodiscard]] error clone() const;

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
    [[nodiscard]] const error& operator[](size_t index) const;

    /// at
    [[nodiscard]] const error& at(size_t index) const;

    /// begin
    [[nodiscard]] const_iterator begin() const;

    /// end
    [[nodiscard]] const_iterator end() const;

    /// begin
    [[nodiscard]] iterator begin();

    /// end
    [[nodiscard]] iterator end();

    /// Erase element.
    void erase(const_iterator position);

    /// Erase range of elements.
    void erase(const_iterator fist, const_iterator last);

    /// Clear error list
    void clear();

  private:
    std::vector<error> m_errors;
  };

  /// Make error from new ErrorInfo instance.
  template <class ErrorInfoT, class... Args>
  error make_error(Args&&... args)
  {
    return error(std::make_unique<ErrorInfoT>(std::forward<Args>(args)...));
  }

} // namespace yave