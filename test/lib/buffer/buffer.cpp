//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/buffer/buffer.hpp>
#include <yave/lib/buffer/buffer_manager.hpp>

#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("", "[lib][buffer]")
{
  buffer_manager mng;
}

TEST_CASE("create 0", "[lib][buffer]")
{
  buffer_manager mng;
  auto id = mng.create(0);
  REQUIRE(id != uid());
  REQUIRE(mng.use_count(id) == 1);
  REQUIRE(mng.size(id) == 0);
  REQUIRE(mng.data(id) == nullptr);
  mng.unref(id);
}

TEST_CASE("create 42", "[lib][buffer]")
{
  buffer_manager mng;
  auto id = mng.create(42);
  REQUIRE(id != uid());
  REQUIRE(mng.use_count(id) == 1);
  REQUIRE(mng.size(id) == 42);
  REQUIRE(mng.data(id) != nullptr);
  mng.unref(id);
}

TEST_CASE("create -1", "[lib][buffer]")
{
  buffer_manager mng;
  auto id = mng.create((uint64_t)-1); // overflow
  REQUIRE(id == uid());
  REQUIRE(mng.use_count(id) == 0);
  REQUIRE(mng.size(id) == 0);
  REQUIRE(mng.data(id) == nullptr);
  mng.unref(id);
}

TEST_CASE("ref/unref", "[lib][buffer]")
{
  buffer_manager mng;
  auto id = mng.create(42);
  REQUIRE(mng.use_count(id) == 1);

  mng.ref(id);
  mng.unref(id);

  REQUIRE(mng.use_count(id) == 1);

  mng.ref(id);
  mng.ref(id);

  REQUIRE(mng.use_count(id) == 3);

  mng.unref(id);
  mng.unref(id);

  REQUIRE(mng.use_count(id) == 1);

  mng.unref(id);

  REQUIRE(mng.use_count(id) == 0);
  REQUIRE(mng.data(id) == nullptr);
  REQUIRE(mng.size(id) == 0);

  mng.ref(id);
  mng.unref(id);
}

TEST_CASE("create_from invalid", "[lib][buffer]")
{
  buffer_manager mng;
  auto id = mng.create_from(uid {42});
  REQUIRE(id == uid());
}

TEST_CASE("create_from 0", "[lib][buffer]")
{
  buffer_manager mng;
  auto parent = mng.create(0);
  auto id     = mng.create_from(parent);
  REQUIRE(id != uid());
  REQUIRE(id != parent);
  REQUIRE(mng.use_count(id) == 1);
  REQUIRE(mng.size(id) == 0);
}

TEST_CASE("create_from str", "[lib][buffer]")
{
  const char str[] = "Hello, World!";
  buffer_manager mng;
  auto parent = mng.create(sizeof(str));
  REQUIRE(mng.size(parent) == sizeof(str));
  std::memcpy(mng.data(parent), str, sizeof(str));
  auto id = mng.create_from(parent);
  REQUIRE(mng.size(id) == mng.size(parent));
  REQUIRE(memcmp(mng.data(id), str, sizeof(str)) == 0);
}