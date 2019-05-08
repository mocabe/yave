//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/tools/error.hpp>

namespace yave {

  ErrorInfoBase::~ErrorInfoBase() noexcept
  {
  }

  Error::Error()
    : m_error_info {nullptr}
  {
  }

  Error::Error(nullptr_t)
    : m_error_info {nullptr}
  {
  }

  Error::Error(std::unique_ptr<ErrorInfoBase>&& err)
    : m_error_info {std::move(err)}
  {
  }

  Error::Error(const Success&)
    : m_error_info {nullptr}
  {
  }

  Error::Error(Error&& other)
    : m_error_info {std::move(other.m_error_info)}
  {
  }

  Error& Error::operator=(const Success&)
  {
    m_error_info = nullptr;
    return *this;
  }

  Error& Error::operator=(Error&& other)
  {
    m_error_info = std::move(other.m_error_info);
    return *this;
  }

  const ErrorInfoBase* Error::info() const
  {
    return m_error_info.get();
  }

  bool Error::is_success() const
  {
    return m_error_info.get() == nullptr;
  }

  Error::operator bool() const
  {
    return is_success();
  }

  std::string Error::message() const
  {
    assert(m_error_info);
    return m_error_info->message();
  }

  const std::type_info& Error::type() const
  {
    assert(m_error_info);
    return m_error_info->type();
  }

  Success::Success()
    : Error(nullptr)
  {
  }

  Success::Success([[maybe_unused]] const Success& other)
  {
    assert(other.is_success());
  }

  Success::Success([[maybe_unused]] const Success&& other)
  {
    assert(other.is_success());
  }

  Success& Success::operator=([[maybe_unused]] const Success& other)
  {
    assert(other.is_success());
    return *this;
  }

  Success& Success::operator=([[maybe_unused]] Success&& other)
  {
    assert(other.is_success());
    return *this;
  }

  ErrorList::ErrorList()
  {
  }

  ErrorList::ErrorList(ErrorList&& other)
    : m_errors {std::move(other.m_errors)}
  {
  }

  ErrorList& ErrorList::operator=(ErrorList&& other)
  {
    m_errors = std::move(other.m_errors);
    return *this;
  }

  void ErrorList::push_back(Error&& error)
  {
    if (!error)
      m_errors.push_back(std::move(error));
  }

  size_t ErrorList::size() const
  {
    return m_errors.size();
  }

  const Error& ErrorList::operator[](size_t index) const
  {
    return m_errors[index];
  }

  const Error& ErrorList::at(size_t index) const
  {
    return m_errors.at(index);
  }

  typename ErrorList::const_iterator ErrorList::begin() const
  {
    return m_errors.cbegin();
  }

  typename ErrorList::const_iterator ErrorList::end() const
  {
    return m_errors.cend();
  }

  void ErrorList::erase(const_iterator position)
  {
    m_errors.erase(position);
  }

  void ErrorList::erase(const_iterator first, const_iterator last)
  {
    m_errors.erase(first, last);
  }

} // namespace yave