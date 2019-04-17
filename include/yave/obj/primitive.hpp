//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/rts/box.hpp>

namespace yave {

  // primitive types

  using Int8   = Box<int8_t>;
  using Int16  = Box<int16_t>;
  using Int32  = Box<int32_t>;
  using Int64  = Box<int64_t>;
  using UInt8  = Box<uint8_t>;
  using UInt16 = Box<uint16_t>;
  using UInt32 = Box<uint32_t>;
  using UInt64 = Box<uint64_t>;
  using Float  = Box<float>;
  using Double = Box<double>;

  using Char  = Int8;
  using Short = Int16;
  using Int   = Int32;
  using Long  = Int64;

  using UChar  = UInt8;
  using UShort = UInt16;
  using UInt   = UInt32;
  using ULong  = UInt64;

  using Bool = Box<bool>;

  // primitive pointer types

  using Int8Ptr   = Box<int8_t *>;
  using Int16Ptr  = Box<int16_t *>;
  using Int32Ptr  = Box<int32_t *>;
  using Int64Ptr  = Box<int64_t *>;
  using UInt8Ptr  = Box<uint8_t *>;
  using UInt16Ptr = Box<uint16_t *>;
  using UInt32Ptr = Box<uint32_t *>;
  using UInt64Ptr = Box<uint64_t *>;
  using FloatPtr  = Box<float *>;
  using DoublePtr = Box<double *>;

  using CharPtr  = Int8Ptr;
  using ShortPtr = Int16Ptr;
  using IntPtr   = Int32Ptr;
  using LongPtr  = Int64Ptr;

  using UCharPtr  = UInt8Ptr;
  using UShortPtr = UInt16Ptr;
  using UIntPtr   = UInt32Ptr;
  using ULongPtr  = UInt64Ptr;

  using BoolPtr = Box<bool *>;

  // void*
  using VoidPtr = Box<void *>;

  // const char*
  using CStr = Box<const char *>;

} // namespace yave

YAVE_DECL_TYPE(yave::Int8, _Int8);
YAVE_DECL_TYPE(yave::Int16, _Int16);
YAVE_DECL_TYPE(yave::Int32, _Int32);
YAVE_DECL_TYPE(yave::Int64, _Int64);

YAVE_DECL_TYPE(yave::UInt8, _UInt8);
YAVE_DECL_TYPE(yave::UInt16, _UInt16);
YAVE_DECL_TYPE(yave::UInt32, _UInt32);
YAVE_DECL_TYPE(yave::UInt64, _UInt64);

YAVE_DECL_TYPE(yave::Float, _Float);
YAVE_DECL_TYPE(yave::Double, _Double);

YAVE_DECL_TYPE(yave::Bool, _Bool);

YAVE_DECL_TYPE(yave::Int8Ptr, _Int8Ptr);
YAVE_DECL_TYPE(yave::Int16Ptr, _Int16Ptr);
YAVE_DECL_TYPE(yave::Int32Ptr, _Int32Ptr);
YAVE_DECL_TYPE(yave::Int64Ptr, _Int64Ptr);

YAVE_DECL_TYPE(yave::UInt8Ptr, _UInt8Ptr);
YAVE_DECL_TYPE(yave::UInt16Ptr, _UInt16Ptr);
YAVE_DECL_TYPE(yave::UInt32Ptr, _UInt32Ptr);
YAVE_DECL_TYPE(yave::UInt64Ptr, _UInt64Ptr);

YAVE_DECL_TYPE(yave::FloatPtr, _FloatPtr);
YAVE_DECL_TYPE(yave::DoublePtr, _DoublePtr);

YAVE_DECL_TYPE(yave::BoolPtr, _BoolPtr);

YAVE_DECL_TYPE(yave::VoidPtr, _VoidPtr);

YAVE_DECL_TYPE(yave::CStr, _CStr);