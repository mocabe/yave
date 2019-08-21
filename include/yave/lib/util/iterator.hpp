//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <iterator>

namespace yave {

  /// simple STL iterator adaptor for pointers
  template <class Pointer, class Container>
  class iterator
  {
    template <class Ptr, class Ctnr>
    friend class iterator;

    Pointer m_pointer;
    using traits = std::iterator_traits<Pointer>;

  public:
    using iterator_type     = Pointer;
    using iterator_category = typename traits::iterator_category;
    using pointer           = typename traits::pointer;
    using value_type        = typename traits::value_type;
    using reference         = typename traits::reference;
    using difference_type   = typename traits::difference_type;

    constexpr iterator() noexcept
      : m_pointer {nullptr}
    {
    }

    constexpr explicit iterator(Pointer p) noexcept
      : m_pointer {p}
    {
    }

    template <class Ptr>
    constexpr iterator(
      const iterator<Ptr, Container>& other,
      std::enable_if_t<std::is_convertible_v<Ptr, Pointer>, nullptr_t> =
        nullptr) noexcept
      : m_pointer {other.m_pointer}
    {
    }

    reference operator*() const noexcept
    {
      return *m_pointer;
    }

    pointer operator->() const noexcept
    {
      return m_pointer;
    }

    iterator& operator++() noexcept
    {
      ++m_pointer;
      return *this;
    }

    iterator operator++(int) noexcept
    {
      return iterator(m_pointer++);
    }

    iterator& operator--() noexcept
    {
      --m_pointer;
      return *this;
    }

    iterator operator--(int) noexcept
    {
      return iterator(m_pointer--);
    }

    reference operator[](difference_type n) const noexcept
    {
      return m_pointer[n];
    }

    iterator& operator+=(difference_type n) noexcept
    {
      m_pointer += n;
      return *this;
    }

    iterator operator+(difference_type n) noexcept
    {
      return iterator(m_pointer + n);
    }

    iterator& operator-=(difference_type n) noexcept
    {
      m_pointer -= n;
      return *this;
    }

    iterator operator-(difference_type n) noexcept
    {
      return iterator(m_pointer - n);
    }

    friend inline bool operator==(const iterator& lhs, const iterator& rhs)
    {
      return lhs.m_pointer == rhs.m_pointer;
    }

    friend inline bool operator!=(const iterator& lhs, const iterator& rhs)
    {
      return lhs.m_pointer != rhs.m_pointer;
    }
  };

} // namespace yave