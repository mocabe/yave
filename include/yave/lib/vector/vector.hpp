//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/util/iterator.hpp>

#include <memory>
#include <vector>
#include <iterator>

namespace yave {

  /// Simple vector class.
  /// Just like yave::string, this class is mainly for data transfer as heap
  /// object (possibly across binary boundary, so we cannot depend on STL).
  /// We don't use capacity, so this is basically a wrapper of classic (pointer,
  /// size) pair.
  /// push_back/resize are also not supported. Use std::vector for these
  /// operations.
  /// Supports conversion from/to std::vector.
  template <class T, class Alloc = std::allocator<T>>
  class vector
  {
    static_assert(std::is_nothrow_move_constructible_v<T>);
    static_assert(std::is_nothrow_destructible_v<T>);

  public:
    // clang-format off
    using allocator       = Alloc;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = typename std::allocator_traits<Alloc>::pointer;
    using const_pointer   = typename std::allocator_traits<Alloc>::const_pointer;
    using iterator        = yave::iterator<pointer, vector>;
    using const_iterator  = yave::iterator<const_pointer, vector>;
    // clang-format on

  public:
    /// Default constructor
    vector() noexcept
    {
      m_ptr  = nullptr;
      m_size = 0;
    }

    /// Initialized n default elements
    explicit vector(size_t n)
    {
      m_ptr  = _alloc(n);
      m_size = n;
    }

    /// Copy initialize n elements
    explicit vector(size_t n, const T& value)
    {
      m_ptr  = _alloc(n, value);
      m_size = n;
    }

    /// Copy initialize from iterator range
    template <
      class InputIter,
      class = std::enable_if_t<std::is_convertible_v<
        typename std::iterator_traits<InputIter>::iterator_category,
        std::input_iterator_tag>>>
    vector(InputIter first, InputIter last)
    {
      auto n = std::distance(first, last);
      m_ptr  = _alloc_copy(first, n);
      m_size = n;
    }

    /// Copy constructor
    vector(const vector& other)
    {
      if (this == std::addressof(other)) {
        m_ptr  = nullptr;
        m_size = 0;
        return;
      }
      m_ptr  = _alloc_copy(other.m_ptr, other.m_size);
      m_size = other.m_size;
    }

    /// Move constructor
    vector(vector&& other) noexcept
      : vector()
    {
      swap(other);
    }

    /// Copy initialize from std::vector
    vector(const std::vector<T, allocator>& other)
      : vector(other.begin(), other.end())
    {
    }

    /// Move initialize from std::vector
    vector(std::vector<T, allocator>&& other) noexcept
    {
      auto tmp = std::move(other);
      auto n   = std::distance(tmp.begin(), tmp.end());
      m_ptr    = _alloc_move(tmp.begin(), n);
      m_size   = n;
    }

    /// Destructor
    ~vector() noexcept
    {
      _dealloc(m_ptr, m_size);
    }

    /// operator=
    vector& operator=(const vector& other)
    {
      if (this == std::addressof(other)) {
        return *this;
      }
      auto tmp = other;
      swap(tmp);
      return *this;
    }

    /// operator=
    vector& operator=(vector&& other) noexcept
    {
      auto tmp = std::move(other);
      swap(tmp);
      return *this;
    }

    /// Conversion operator to std::vector
    [[nodiscard]] operator std::vector<T, allocator>() const
    {
      return std::vector<T, allocator>(begin(), end());
    }

    /// Swap
    void swap(vector& other) noexcept
    {
      std::swap(m_ptr, other.m_ptr);
      std::swap(m_size, other.m_size);
    }

    /// operator[]
    [[nodiscard]] auto operator[](size_t n) -> reference
    {
      return m_ptr[n];
    }

    /// operator[]
    [[nodiscard]] auto operator[](size_t n) const -> const_reference
    {
      return m_ptr[n];
    }

    /// at
    [[nodiscard]] auto at(size_t n) -> reference
    {
      if (n >= m_size)
        throw std::out_of_range("yave::vector::at()");

      return m_ptr[n];
    }

    /// at
    [[nodiscard]] auto at(size_t n) const -> const_reference
    {
      if (n >= m_size)
        throw std::out_of_range("yave::vector::at()");

      return m_ptr[n];
    }

    /// data
    [[nodiscard]] auto data() noexcept -> pointer
    {
      return m_ptr;
    }

    /// data
    [[nodiscard]] auto data() const noexcept -> const_pointer
    {
      return m_ptr;
    }

    /// Get size
    [[nodiscard]] size_t size() const noexcept
    {
      return m_size;
    }

    /// empty?
    [[nodiscard]] bool empty() const noexcept
    {
      return m_size == 0;
    }

    /// front
    [[nodiscard]] auto front() -> reference
    {
      return *begin();
    }

    /// front
    [[nodiscard]] auto front() const -> const_reference
    {
      return *begin();
    }

    /// back
    [[nodiscard]] auto back() -> reference
    {
      return *(--end());
    }

    /// back
    [[nodiscard]] auto back() const -> const_reference
    {
      return *(--end());
    }

    /// begin
    [[nodiscard]] auto begin() noexcept -> iterator
    {
      return iterator(m_ptr);
    }

    /// end
    [[nodiscard]] auto end() noexcept -> iterator
    {
      return iterator(m_ptr + m_size);
    }

    /// begin
    [[nodiscard]] auto begin() const noexcept -> const_iterator
    {
      return const_iterator(m_ptr);
    }

    /// end
    [[nodiscard]] auto end() const noexcept -> const_iterator
    {
      return const_iterator(m_ptr + m_size);
    }

    /// cbegin
    [[nodiscard]] auto cbegin() const noexcept -> const_iterator
    {
      return begin();
    }

    /// cend
    [[nodiscard]] auto cend() const noexcept -> const_iterator
    {
      return end();
    }

  private:
    [[nodiscard]] T* _alloc_uninitialized(size_t n)
    {
      return allocator().allocate(n);
    }

    [[nodiscard]] T* _alloc(size_t n)
    {
      auto mem = _alloc_uninitialized(n);
      std::uninitialized_value_construct_n(mem, n);
      return mem;
    }

    [[nodiscard]] T* _alloc(size_t n, const T& v)
    {
      auto mem = _alloc_uninitialized(n);
      std::uninitialized_fill_n(mem, n, v);
      return mem;
    }

    template <class InIt>
    [[nodiscard]] T* _alloc_copy(InIt itr, size_t n)
    {
      auto mem = _alloc_uninitialized(n);
      std::uninitialized_copy_n(itr, n, mem);
      return mem;
    }

    template <class InIt>
    [[nodiscard]] T* _alloc_move(InIt itr, size_t n)
    {
      auto mem = _alloc_uninitialized(n);
      std::uninitialized_move_n(itr, n, mem);
      return mem;
    }

    void _dealloc(T* ptr, size_t n) noexcept
    {
      std::destroy_n(ptr, n);
      allocator().deallocate(ptr, n);
    }

  private:
    /// data
    T* m_ptr;
    /// size
    uint64_t m_size;
  };
} // namespace yave