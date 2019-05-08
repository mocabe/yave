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
  class ErrorInfoBase
  {
  public:
    /// Generate error message.
    virtual std::string message() const = 0;
    /// type info
    virtual const std::type_info& type() const = 0;
    /// dtor
    virtual ~ErrorInfoBase() noexcept;
  };

  /// Base class of ErrorInfo objects.
  template <class Derived>
  struct ErrorInfo : ErrorInfoBase
  {
    virtual std::string message() const override
    {
      return "ErrorInfo";
    }

    virtual const std::type_info& type() const override
    {
      return typeid(Derived);
    }

    virtual ~ErrorInfo() noexcept override
    {
    }
  };

  /// Error
  class Error
  {
  public:
    /// Initialize Error with success.
    Error();

    /// Initialize Error with success.
    Error(nullptr_t);

    /// Initialize Error with error info.
    Error(std::unique_ptr<ErrorInfoBase>&& err);

    /// Move ctor.
    Error(Error&& other);

    /// operator=
    Error& operator=(Error&&);

    /// Copy constructor (deleted)
    Error(const Error&) = delete;
    /// Copy assignment operator (deleted)
    Error& operator=(const Error&) = delete;

    /// Initialize from Success
    explicit Error(const class Success&);

    /// Initialize from Success
    Error& operator=(const class Success&);

    /// Get pointer to ErrorInfo.
    const ErrorInfoBase* info() const;

    /// success?
    bool is_success() const;

    /// operator bool
    explicit operator bool() const;

    /// Get error message
    std::string message() const;

    /// Get type_info
    const std::type_info& type() const;

  private:
    std::unique_ptr<const ErrorInfoBase> m_error_info;
  };

  /// Success
  class Success : public Error
  {
  public:
    /// Ctor.
    Success();
    /// Copy ctor.
    Success(const Success& other);
    /// Move ctor.
    Success(const Success&& other);
    /// Copy assignment operator.
    Success& operator=(const Success& other);
    /// Move assignment operator.
    Success& operator=(Success&& other);

  private:
    using Error::Error;
  };

  /// Simple list of Error objects.
  class ErrorList
  {
  public:
    /// const_iterator
    using const_iterator = typename std::vector<Error>::const_iterator;

    /// Initialize ErrorList with empty list.
    ErrorList();

    /// Move constructor.
    ErrorList(ErrorList&& other);

    /// Move assignment operator.
    ErrorList& operator=(ErrorList&&);

    /// Copy ctor (deleted)
    ErrorList(const ErrorList&) = delete;

    /// Copy assignment operator (deleted)
    ErrorList& operator=(const ErrorList&) = delete;

    /// Add error to the list.
    void push_back(Error&& error);

    /// Get size of list.
    size_t size() const;

    /// Empty?
    bool empty() const;

    /// operator[]
    const Error& operator[](size_t index) const;

    /// at
    const Error& at(size_t index) const;

    /// begin
    const_iterator begin() const;

    /// end
    const_iterator end() const;

    /// Erase element.
    void erase(const_iterator position);

    /// Erase range of elements.
    void erase(const_iterator fist, const_iterator last);


  private:
    std::vector<Error> m_errors;
  };

  /// Make Error from new ErrorInfo instance.
  template <class ErrorInfoT, class... Args>
  Error make_error(Args&&... args)
  {
    return Error(std::make_unique<ErrorInfoT>(std::forward<Args>(args)...));
  }

} // namespace yave