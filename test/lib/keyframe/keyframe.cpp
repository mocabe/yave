//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/keyframe/keyframe.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("keyframe")
{
  keyframe<int> kf;
  auto zero = time::zero();
  auto max  = time::max();
  auto min  = time::min();

  SECTION("")
  {
    REQUIRE(kf.size() == 0);
    REQUIRE(kf.empty());
    REQUIRE(kf.keys().empty());
    REQUIRE(kf.values().empty());
    REQUIRE(kf.pairs().empty());
    REQUIRE(kf.find_range(zero) == std::pair {min, max});
    REQUIRE(kf.find_range(min) == std::pair {min, max});
    REQUIRE(kf.find_range(max) == std::pair {min, max});
    REQUIRE(kf.find_value(zero) == 0);
    REQUIRE(kf.find_value(max) == 0);
    REQUIRE(kf.find_value(min) == 0);
  }

  SECTION("set 0")
  {
    kf.set(zero, 42); // no effect
    REQUIRE(kf.empty());
  }

  SECTION("insert 0")
  {
    kf.insert(zero, 42);

    SECTION("")
    {
      REQUIRE(kf.size() == 1);
      REQUIRE(!kf.empty());
      REQUIRE(kf.keys().size() == 1);
      REQUIRE(kf.keys()[0] == zero);
      REQUIRE(kf.values().size() == 1);
      REQUIRE(kf.values()[0] == 42);
      REQUIRE(kf.pairs()[0] == std::pair {zero, 42});
      REQUIRE(kf.find_range(zero) == std::pair {zero, max});
      REQUIRE(kf.find_range(min) == std::pair {min, zero});
      REQUIRE(kf.find_range(max) == std::pair {zero, max});
      REQUIRE(kf.find_value(zero) == 42);
      REQUIRE(kf.find_value(min) == 42);
      REQUIRE(kf.find_value(max) == 42);
    }

    SECTION("set 0")
    {
      kf.set(zero, 24);
      REQUIRE(kf.find_value(zero) == 24);
      REQUIRE(kf.find_value(min) == 24);
      REQUIRE(kf.find_value(max) == 24);
      REQUIRE(kf.size() == 1);
    }

    SECTION("insert 0")
    {
      kf.insert(zero, 24);
      REQUIRE(kf.size() == 1);
      REQUIRE(kf.find_value(zero) == 42);
      REQUIRE(kf.find_value(min) == 42);
      REQUIRE(kf.find_value(max) == 42);
    }

    SECTION("insert min")
    {
      kf.insert(time::min(), 24);

      SECTION("")
      {
        REQUIRE(kf.size() == 2);
        REQUIRE(kf.find_range(min) == std::pair {min, zero});
        REQUIRE(kf.find_range(zero) == std::pair {zero, max});
        REQUIRE(kf.find_range(max) == std::pair {zero, max});
        REQUIRE(kf.find_value(min) == 24);
        REQUIRE(kf.find_value(zero) == 42);
        REQUIRE(kf.find_value(max) == 42);
      }

      SECTION("eraze 0")
      {
        kf.erase(zero);
        REQUIRE(kf.size() == 1);
        REQUIRE(kf.find_value(min) == 24);
        REQUIRE(kf.find_value(zero) == 24);
        REQUIRE(kf.find_value(max) == 24);
        REQUIRE(kf.find_range(zero) == std::pair {min, max});
      }

      SECTION("copy")
      {
        keyframe<int> cp = kf;
        REQUIRE(cp.size() == 2);
        kf.clear();
        REQUIRE(cp.find_value(min) == 24);
        REQUIRE(cp.find_value(zero) == 42);
      }

      SECTION("move")
      {
        keyframe<int> mv = std::move(kf);
        REQUIRE(mv.size() == 2);
        REQUIRE(kf.empty());
      }
    }

    SECTION("insert max")
    {
      kf.insert(time::max(), 666);
      REQUIRE(kf.size() == 1);
      REQUIRE(kf.find_value(zero) == 42);
      REQUIRE(kf.find_value(min) == 42);
      REQUIRE(kf.find_value(max) == 42);
    }

    SECTION("erase 0")
    {
      kf.erase(zero);
      REQUIRE(kf.size() == 0);
      REQUIRE(kf.empty());
      REQUIRE(kf.find_value(zero) == 0);
      REQUIRE(kf.find_value(min) == 0);
      REQUIRE(kf.find_value(max) == 0);
    }

    SECTION("key iter")
    {
      for (auto iter = kf.kbegin(); iter != kf.kend(); ++iter) {
        REQUIRE(*iter == zero);
      }
    }

    SECTION("operator=")
    {
      SECTION("copy")
      {
        auto tmp = kf;
        REQUIRE(tmp.size() == 1);
        REQUIRE(kf.size() == 1);
      }

      SECTION("move")
      {
        auto tmp = std::move(kf);
        REQUIRE(kf.empty());
        REQUIRE(tmp.size() == 1);
      }

      SECTION("self copy")
      {
        kf = kf;
        REQUIRE(kf.size() == 1);
      }

      SECTION("self move")
      {
        kf = std::move(kf);
        REQUIRE(kf.size() == 1);
      }
    }
  }
}
