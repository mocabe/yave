//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/support/error.hpp>

namespace yave {

  error_info_base::~error_info_base() noexcept
  {
  }

  error::error()
    : m_error_info {nullptr}
  {
  }

  error::error(nullptr_t)
    : m_error_info {nullptr}
  {
  }

  error::error(std::unique_ptr<error_info_base>&& err)
    : m_error_info {std::move(err)}
  {
  }

  error::error(const success&)
    : m_error_info {nullptr}
  {
  }

  error::error(error&& other)
    : m_error_info {std::move(other.m_error_info)}
  {
  }

  error& error::operator=(const success&)
  {
    m_error_info = nullptr;
    return *this;
  }

  error& error::operator=(error&& other)
  {
    m_error_info = std::move(other.m_error_info);
    return *this;
  }

  const error_info_base* error::info() const
  {
    return m_error_info.get();
  }

  bool error::is_success() const
  {
    return m_error_info.get() == nullptr;
  }

  error::operator bool() const
  {
    return is_success();
  }

  std::string error::message() const
  {
    assert(m_error_info);
    return m_error_info->message();
  }

  const std::type_info& error::type() const
  {
    assert(m_error_info);
    return m_error_info->type();
  }

  error error::clone() const
  {
    return m_error_info->clone();
  }

  success::success()
    : error(nullptr)
  {
  }

  success::success([[maybe_unused]] const success& other)
    : error(nullptr)
  {
    assert(other.is_success());
  }

  success::success([[maybe_unused]] const success&& other)
    : error(nullptr)
  {
    assert(other.is_success());
  }

  success& success::operator=([[maybe_unused]] const success& other)
  {
    assert(other.is_success());
    return *this;
  }

  success& success::operator=([[maybe_unused]] success&& other)
  {
    assert(other.is_success());
    return *this;
  }

  error_list::error_list()
  {
  }

  error_list::error_list(error_list&& other)
    : m_errors {std::move(other.m_errors)}
  {
  }

  error_list& error_list::operator=(error_list&& other)
  {
    m_errors = std::move(other.m_errors);
    return *this;
  }

  void error_list::push_back(error&& error)
  {
    if (!error)
      m_errors.push_back(std::move(error));
  }

  size_t error_list::size() const
  {
    return m_errors.size();
  }

  bool error_list::empty() const
  {
    return m_errors.empty();
  }

  const error& error_list::operator[](size_t index) const
  {
    return m_errors[index];
  }

  const error& error_list::at(size_t index) const
  {
    return m_errors.at(index);
  }

  typename error_list::const_iterator error_list::begin() const
  {
    return m_errors.cbegin();
  }

  typename error_list::const_iterator error_list::end() const
  {
    return m_errors.cend();
  }

  void error_list::erase(const_iterator position)
  {
    m_errors.erase(position);
  }

  void error_list::erase(const_iterator first, const_iterator last)
  {
    m_errors.erase(first, last);
  }

} // namespace yave