//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/core/data_types/string.hpp>
#include <yave/core/rts/atomic.hpp>

#include <variant>
#include <string>

namespace yave {

  /// primitive type
  using primitive_t //
    = std::variant<
      int8_t,
      int16_t,
      int32_t,
      int64_t,
      uint8_t,
      uint16_t,
      uint32_t,
      uint64_t,
      double,
      float,
      bool,
      yave::string>;

  /// Type trait for primitive types.
  template <class T>
  struct primitive_type_traits
  {
  };

#define YAVE_DECL_PRIMITIVE_TYPENAME(t, tname)  \
  template <>                                   \
  struct primitive_type_traits<t>               \
  {                                             \
    static constexpr const char* name = #tname; \
  }

  YAVE_DECL_PRIMITIVE_TYPENAME(int8_t, Char);
  YAVE_DECL_PRIMITIVE_TYPENAME(int16_t, Short);
  YAVE_DECL_PRIMITIVE_TYPENAME(int32_t, Int);
  YAVE_DECL_PRIMITIVE_TYPENAME(int64_t, Long);
  YAVE_DECL_PRIMITIVE_TYPENAME(uint8_t, UChar);
  YAVE_DECL_PRIMITIVE_TYPENAME(uint16_t, UShort);
  YAVE_DECL_PRIMITIVE_TYPENAME(uint32_t, UInt);
  YAVE_DECL_PRIMITIVE_TYPENAME(uint64_t, ULong);
  YAVE_DECL_PRIMITIVE_TYPENAME(double, Double);
  YAVE_DECL_PRIMITIVE_TYPENAME(float, Float);
  YAVE_DECL_PRIMITIVE_TYPENAME(bool, Bool);
  YAVE_DECL_PRIMITIVE_TYPENAME(yave::string, String);

#undef YAVE_DECL_PRIMITIVE_TYPENAME

  /// Container of primitive_t for multi-thread access.
  class primitive_container
  {
  public:
    /// Constructor
    primitive_container(const primitive_t& prim);

    /// Copy constructor
    primitive_container(const primitive_container& other);

    /// Set primitive_t value
    void set(const primitive_t& prim);

    /// Get primitive_t value
    [[nodiscard]] primitive_t get() const;

  private:
    primitive_t m_prim;
    mutable atomic_spinlock<uint8_t> m_mtx;
  };

} // namespace yave