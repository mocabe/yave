//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/lib/mat/mat.hpp>

namespace yave {

  // Int

  using I8Mat2  = Box<i8mat2>;
  using I16Mat2 = Box<i16mat2>;
  using I32Mat2 = Box<i32mat2>;
  using I64Mat2 = Box<i64mat2>;

  using I8Mat3  = Box<i8mat3>;
  using I16Mat3 = Box<i16mat3>;
  using I32Mat3 = Box<i32mat3>;
  using I64Mat3 = Box<i64mat3>;

  using I8Mat4  = Box<i8mat4>;
  using I16Mat4 = Box<i16mat4>;
  using I32Mat4 = Box<i32mat4>;
  using I64Mat4 = Box<i64mat4>;

  // Uint

  using U8Mat2  = Box<u8mat2>;
  using U16Mat2 = Box<u16mat2>;
  using U32Mat2 = Box<u32mat2>;
  using U64Mat2 = Box<u64mat2>;

  using U8Mat3  = Box<u8mat3>;
  using U16Mat3 = Box<u16mat3>;
  using U32Mat3 = Box<u32mat3>;
  using U64Mat3 = Box<u64mat3>;

  using U8Mat4  = Box<u8mat4>;
  using U16Mat4 = Box<u16mat4>;
  using U32Mat4 = Box<u32mat4>;
  using U64Mat4 = Box<u64mat4>;

  // Float

  using FMat2 = Box<fmat2>;
  using FMat3 = Box<fmat3>;
  using FMat4 = Box<fmat4>;

  // Double

  using DMat2 = Box<dmat2>;
  using DMat3 = Box<dmat3>;
  using DMat4 = Box<dmat4>;

} // namespace yave

YAVE_DECL_TYPE(yave::I8Mat2, "77e34f28-25e8-4f5c-91f1-2648bbcf83b3");
YAVE_DECL_TYPE(yave::I16Mat2, "16a92dad-e283-4c1b-818c-240baaafc45a");
YAVE_DECL_TYPE(yave::I32Mat2, "0efb5cc0-e19c-45f2-b5a0-7fa3da222f5f");
YAVE_DECL_TYPE(yave::I64Mat2, "53580fae-a590-40bc-a82d-799a9f8bb1ba");

YAVE_DECL_TYPE(yave::I8Mat3, "dbd57cd0-809b-4d35-9f1d-2ba07dade7ef");
YAVE_DECL_TYPE(yave::I16Mat3, "b742747e-be96-4cb2-b1bf-4528938d8444");
YAVE_DECL_TYPE(yave::I32Mat3, "a5e76fe4-9df5-4706-a70b-b30b91283cca");
YAVE_DECL_TYPE(yave::I64Mat3, "cf8f398f-5053-4147-8a3d-f8579d39e585");

YAVE_DECL_TYPE(yave::I8Mat4, "b9e8c67a-b2c2-478b-a412-e5279f53c88c");
YAVE_DECL_TYPE(yave::I16Mat4, "b016892b-19d6-4a10-b4cc-4b6b909d3b7d");
YAVE_DECL_TYPE(yave::I32Mat4, "1f71fc8b-46eb-4a6f-a57c-3f05a1b5913c");
YAVE_DECL_TYPE(yave::I64Mat4, "1a452a0c-f341-45d5-9b1f-41d49989e72b");

YAVE_DECL_TYPE(yave::U8Mat2, "0b7bccfe-6513-4ce5-ba8f-9e9f40c5e0c4");
YAVE_DECL_TYPE(yave::U16Mat2, "bd697660-50c9-4b0e-a073-322c70590906");
YAVE_DECL_TYPE(yave::U32Mat2, "b717d438-9017-4797-98cb-1f12f37f62f1");
YAVE_DECL_TYPE(yave::U64Mat2, "a9cd873a-f7b5-4af6-97a0-8bf96b9a8fce");

YAVE_DECL_TYPE(yave::U8Mat3, "5e2f9c45-759f-4aba-8500-b76bab8330c8");
YAVE_DECL_TYPE(yave::U16Mat3, "35559207-94d1-4215-836f-85f56ccccfff");
YAVE_DECL_TYPE(yave::U32Mat3, "8036ab72-b54e-4884-b9a7-27b62d5d4434");
YAVE_DECL_TYPE(yave::U64Mat3, "68160eb2-e538-469f-9427-ee35ae1ba440");

YAVE_DECL_TYPE(yave::U8Mat4, "0901db8f-7732-458d-8317-a4f0183cb8ba");
YAVE_DECL_TYPE(yave::U16Mat4, "f7c7902e-b75c-4965-ae2b-a24d98089b60");
YAVE_DECL_TYPE(yave::U32Mat4, "5412c574-2409-4da7-872f-37ea4e304b5c");
YAVE_DECL_TYPE(yave::U64Mat4, "0f3a5622-55f3-4969-90e8-4dfd15d8e749");

YAVE_DECL_TYPE(yave::FMat2, "b135634c-dc9f-45dc-bc68-49f8fa07c0ab");
YAVE_DECL_TYPE(yave::FMat3, "e2c21995-8cc2-4f37-81c9-e46f8f5fc569");
YAVE_DECL_TYPE(yave::FMat4, "a48f39d2-6f06-44d9-bd9f-63a1af570205");

YAVE_DECL_TYPE(yave::DMat2, "035f8b34-8f2b-4e79-9ebf-319c4a029cb6");
YAVE_DECL_TYPE(yave::DMat3, "f66a0e02-e3b8-4835-969b-e64ce4c18640");
YAVE_DECL_TYPE(yave::DMat4, "fda0796a-473c-4f4d-bba8-535011a934de");