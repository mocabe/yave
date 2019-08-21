//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/data/lib/time.hpp>
#include <yave/lib/util/iterator.hpp>

#include <vector>
#include <iterator>

namespace yave {

  /// Template container for key-pairs for keyframe.
  template <class T>
  class keyframe
  {
    /// key type (time)
    using key_type = time;
    /// value type
    using value_type = T;

    /// key iterator
    using const_key_iterator = iterator<key_type*, keyframe>;
    /// value iterator
    using value_iterator = iterator<value_type*, keyframe>;
    /// value iterator
    using const_value_iterator = iterator<value_type*, keyframe>;

  private:
    using k_t = key_type;
    using v_t = value_type;

  public:
    /// Initialize empty keyframe
    keyframe()
    {
      m_keys   = new k_t[1] {time::max()};
      m_values = new v_t[1];
      m_size   = 0;
    }

    /// copy keyframe
    keyframe(const keyframe& other)
    {
      m_keys   = new k_t[other.m_size + 1];
      m_values = new v_t[other.m_size + 1];

      for (size_t i = 0; i < other.m_size + 1; ++i) {
        m_keys[i]   = other.m_keys[i];
        m_values[i] = other.m_values[i];
      }
      m_size = other.m_size;
    }

    /// Move keyframe
    keyframe(keyframe&& other) noexcept
    {
      m_keys   = nullptr;
      m_values = nullptr;
      m_size   = 0;

      std::swap(m_keys, other.m_keys);
      std::swap(m_values, other.m_values);
      std::swap(m_size, other.m_size);
    }

    ~keyframe()
    {
      delete[] m_keys;
      delete[] m_values;
    }

    /// empty?
    bool empty() const noexcept
    {
      return m_size == 0;
    }

    /// find range
    auto find_range(const time& key) const noexcept -> std::pair<time, time>
    {
      if (empty())
        return {time::min(), time::max()};

      if (key < m_keys[0])
        return {time::min(), m_keys[0]};

      for (size_t i = 0; i < m_size; ++i) {
        if (m_keys[i] <= key && key < m_keys[i + 1]) {
          return {m_keys[i], m_keys[i + 1]};
        }
      }
      return {m_keys[m_size - 1], time::max()};
    }

    /// find value if exists, returns default value when empty.
    auto find_value(const time& key) const noexcept -> value_type
    {
      if (empty())
        return v_t {};

      if (key < m_keys[0])
        return m_values[0];

      for (size_t i = 0; i < m_size; ++i) {
        if (m_keys[i] <= key && key < m_keys[i + 1]) {
          return m_values[i];
        }
      }
      return m_values[m_size - 1];
    }

    /// Set new value to exisint key, otherwise not effect.
    void set(const time& key, const value_type& value)
    {
      for (size_t i = 0; i < m_size; ++i) {
        if (m_keys[i] == key) {
          m_values[i] = value;
          break;
        }
      }
    }

    /// Insert new value to new/exising key.
    void insert(const time& key, const value_type& value)
    {
      // empty
      if (empty()) {
        assert(m_keys);
        assert(m_values);
        // extend capacity
        extend_by_one();
        // move sentinel
        m_keys[1]   = std::move(m_keys[0]);
        m_values[1] = std::move(m_values[0]);
        // set value
        m_keys[0]   = key;
        m_values[0] = value;
        m_size      = 1;
        return;
      }

      // out of range
      if (key < m_keys[0]) {
        // extend capacity
        extend_by_one();
        // shift elements
        for (size_t i = 0; i < m_size + 1; ++i) {
          auto j = m_size - i; // reverse
          {
            m_keys[j + 1]   = std::move(m_keys[j]);
            m_values[j + 1] = std::move(m_values[j]);
          }
        }
        // insert
        m_keys[0]   = key;
        m_values[0] = value;
        m_size      = m_size + 1;
        return;
      }

      // inside ranges
      for (size_t i = 0; i < m_size; ++i) {
        if (m_keys[i] < key && key < m_keys[i + 1]) {
          // extend capacity
          extend_by_one();
          // shift elements
          for (size_t j = m_size + 1; i < j; --j) {
            m_keys[j]   = std::move(m_keys[j - 1]);
            m_values[j] = std::move(m_values[j - 1]);
          }
          // insert
          m_keys[i]   = key;
          m_values[i] = value;
          m_size      = m_size + 1;
          return;
        }
      }
    }

    /// erase value
    void erase(const time& key)
    {
      for (size_t i = 0; i < m_size; ++i) {
        if (m_keys[i] == key) {
          // move keys
          for (size_t j = i; j < m_size; ++j) {
            m_keys[j]   = std::move(m_keys[j + 1]);
            m_values[j] = std::move(m_values[j + 1]);
          }
          m_size = m_size - 1;
          shrink_by_one();
          return;
        }
      }
    }

    /// Get list of keys.
    auto keys() -> std::vector<key_type> const
    {
      return std::vector<key_type>(m_keys, m_keys + m_size);
    }

    /// Get list of values.
    auto values() -> std::vector<value_type> const
    {
      return std::vector<value_type>(m_values, m_values + m_size);
    }

    /// Get list of pairs
    auto pairs() const -> std::vector<std::pair<key_type, value_type>>
    {
      std::vector<std::pair<k_t, v_t>> ret;
      ret.reserve(m_size);
      for (size_t i = 0; i < m_size; ++i) {
        ret.emplace_back(m_keys[i], m_values[i]);
      }
      return ret;
    }

    auto kbegin() const -> const_key_iterator
    {
      return const_key_iterator(m_keys);
    }

    auto kend() const -> const_key_iterator
    {
      return const_key_iterator(m_keys + m_size);
    }

    auto vbegin() -> value_iterator
    {
      return value_iterator(m_values);
    }

    auto vend() -> value_iterator
    {
      return value_iterator(m_values + m_size);
    }

    auto vbegin() const -> const_value_iterator
    {
      return const_value_iterator(m_values);
    }

    auto vend() const -> const_value_iterator
    {
      return const_value_iterator(m_values + m_size);
    }

    /// Get size.
    uint64_t size() const noexcept
    {
      return m_size;
    }

    /// Clear values
    void clear()
    {
      auto new_keys   = new k_t[1] {time::max()};
      auto new_values = new v_t[1] {};

      std::swap(m_keys, new_keys);
      std::swap(m_values, new_values);
      m_size = 0;

      delete[] new_keys;
      delete[] new_values;
    }

  private:
    /// Extend buffers by one element. Assume m_size is not incremented yet.
    void extend_by_one()
    {
      // realloc
      auto new_keys   = new k_t[m_size + 2];
      auto new_values = new v_t[m_size + 2];

      // move element
      for (size_t i = 0; i < m_size + 1; ++i) {
        new_keys[i]   = std::move(m_keys[i]);
        new_values[i] = std::move(m_values[i]);
      }

      std::swap(m_keys, new_keys);
      std::swap(m_values, new_values);

      delete[] new_keys;
      delete[] new_values;
    }

    /// Shrink buffers by one element.
    void shrink_by_one()
    {
      // Assume m_size is already decreased by one.
      auto new_keys   = new k_t[m_size + 1];
      auto new_values = new v_t[m_size + 1];

      // leave last element in old buffer.
      for (size_t i = 0; i < m_size + 1; ++i) {
        new_keys[i]   = std::move(m_keys[i]);
        new_values[i] = std::move(m_values[i]);
      }

      std::swap(m_keys, new_keys);
      std::swap(m_values, new_values);

      delete[] new_keys;
      delete[] new_values;
    }

  private:
    // value
    //   |               v[1]-----(v[size] -> v[1])
    //   |-----v[0]-------|           |
    //   |      |         |           |
    // --+------+---------+-----------+  time
    //   |    key[0]    key[1]    (key[size] == time::max)

    /// number of key-value pairs
    uint64_t m_size;
    /// pointer to key array
    time* m_keys;
    /// pointer to value array
    value_type* m_values;
  };
} // namespace yave