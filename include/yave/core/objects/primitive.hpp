//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/core/rts/box.hpp>
#include <yave/core/data_types/primitive.hpp>

namespace yave {

  // primitive_t

  using Primitive          = Box<primitive_t>;
  using PrimitiveContainer = Box<primitive_container>;

  // primitive value types

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

YAVE_DECL_TYPE(yave::Primitive, "081a420e-452a-4424-bb06-03df13301030");
YAVE_DECL_TYPE(yave::PrimitiveContainer, "3f3fb6df-fb90-46d8-bab4-df77caee3001");

YAVE_DECL_TYPE(yave::Int8, "50366ffd-5e1b-41a6-bd60-a9f67ca5dd59");
YAVE_DECL_TYPE(yave::Int16, "5548b6f1-8400-45c7-a169-02478cae9235");
YAVE_DECL_TYPE(yave::Int32, "33ba418a-7249-496f-9926-e127e18a1b5d");
YAVE_DECL_TYPE(yave::Int64, "98e3899c-af35-46f4-bd0e-a5e588f8fd41");

YAVE_DECL_TYPE(yave::UInt8, "ce802703-9ec3-4fc0-82e7-7f3423dc2657");
YAVE_DECL_TYPE(yave::UInt16, "90851c4f-e92c-4bd4-ab38-d6bc8dd0218f");
YAVE_DECL_TYPE(yave::UInt32, "25460543-4575-4def-b3a1-5bd0ff62596a");
YAVE_DECL_TYPE(yave::UInt64, "39616672-79f6-4a20-ac75-e2bcf9a760df");

YAVE_DECL_TYPE(yave::Float, "2c907962-4976-4649-ab2f-4a755e17e52d");
YAVE_DECL_TYPE(yave::Double, "fa73b107-dd48-4a96-831d-e28b34afb9b8");

YAVE_DECL_TYPE(yave::Bool, "82832489-0578-45db-8cf6-8953d017e398");

YAVE_DECL_TYPE(yave::Int8Ptr, "1530a115-eae6-4bec-ba64-a9b8b3d04f0b");
YAVE_DECL_TYPE(yave::Int16Ptr, "16150086-5f37-462b-81b1-fc256eef74a0");
YAVE_DECL_TYPE(yave::Int32Ptr, "b2cbe7e3-33fa-43aa-934d-d31ae58226b0");
YAVE_DECL_TYPE(yave::Int64Ptr, "01a288b4-98ea-4be2-80a2-dfcc4c1da976");

YAVE_DECL_TYPE(yave::UInt8Ptr, "59f072c6-9b46-46c6-b138-2d0e3811913f");
YAVE_DECL_TYPE(yave::UInt16Ptr, "11f0e8c0-d14e-42f6-a8ae-f1426d3e221e");
YAVE_DECL_TYPE(yave::UInt32Ptr, "90bca9e7-76fb-49dd-8c2b-298dd6b2d5fc");
YAVE_DECL_TYPE(yave::UInt64Ptr, "5beb2383-c4d1-422f-adf1-cb046368752e");

YAVE_DECL_TYPE(yave::FloatPtr, "aeb878d2-9742-4553-ac67-85fac62e09b7");
YAVE_DECL_TYPE(yave::DoublePtr, "4ed6ada2-dd81-4eb9-b7e8-0b30bbd5d38a");

YAVE_DECL_TYPE(yave::BoolPtr, "980aa646-a6a5-48fc-bb97-5c33fda3a18a");

YAVE_DECL_TYPE(yave::VoidPtr, "dd25384f-e86a-403a-81ff-fd4ff0d01a09");

YAVE_DECL_TYPE(yave::CStr, "28ef2895-67ca-4bcd-877d-84bc1e171a48");