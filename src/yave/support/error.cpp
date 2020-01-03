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

  error::error(success&&) noexcept
    : m_error_info {nullptr}
  {
  }

  error::error(error&& other) noexcept
    : m_error_info {std::move(other.m_error_info)}
  {
  }

  error& error::operator=(const success&)
  {
    m_error_info = nullptr;
    return *this;
  }

  error& error::operator=(success&&) noexcept
  {
    m_error_info = nullptr;
    return *this;
  }

  error& error::operator=(error&& other) noexcept
  {
    m_error_info = std::move(other.m_error_info);
    return *this;
  }

  auto error::info() const -> const error_info_base*
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

  auto error::message() const -> std::string
  {
    assert(m_error_info);
    return m_error_info->message();
  }

  auto error::type() const -> const std::type_info&
  {
    assert(m_error_info);
    return m_error_info->type();
  }

  auto error::clone() const -> error
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

  success::success([[maybe_unused]] success&& other) noexcept
    : error(nullptr)
  {
    assert(other.is_success());
  }

  success& success::operator=([[maybe_unused]] const success& other)
  {
    assert(other.is_success());
    return *this;
  }

  success& success::operator=([[maybe_unused]] success&& other) noexcept
  {
    assert(other.is_success());
    return *this;
  }

  error_list::error_list()
  {
  }

  error_list::error_list(error_list&& other) noexcept
    : m_errors {std::move(other.m_errors)}
  {
  }

  error_list& error_list::operator=(error_list&& other) noexcept
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

  auto error_list::operator[](size_t index) const -> const error&
  {
    return m_errors[index];
  }

  auto error_list::at(size_t index) const -> const error&
  {
    return m_errors.at(index);
  }

  auto error_list::begin() const -> const_iterator
  {
    return m_errors.cbegin();
  }

  auto error_list::begin() -> iterator
  {
    return m_errors.begin();
  }

  auto error_list::end() const -> const_iterator
  {
    return m_errors.cend();
  }

  auto error_list::end() -> iterator
  {
    return m_errors.end();
  }

  void error_list::erase(const_iterator position)
  {
    m_errors.erase(position);
  }

  void error_list::erase(const_iterator first, const_iterator last)
  {
    m_errors.erase(first, last);
  }

  void error_list::clear()
  {
    m_errors.clear();
  }

  auto error_list::clone() const -> error_list
  {
    error_list ret;
    for (auto&& e : m_errors) {
      ret.m_errors.push_back(e.clone());
    }
    return ret;
  }

} // namespace yave