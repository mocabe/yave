//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/data/lib/time.hpp>

using namespace yave;

TEST_CASE("time")
{
  static_assert(std::is_same_v<time::value_type, int64_t>);

  SECTION("formats")
  {
    /* common video fps */

    static_assert(time::is_compatible_rate(1));
    static_assert(time::is_compatible_rate(2));
    static_assert(time::is_compatible_rate(3));
    static_assert(time::is_compatible_rate(5));
    static_assert(time::is_compatible_rate(10));
    static_assert(time::is_compatible_rate(15));
    static_assert(time::is_compatible_rate(24));
    static_assert(time::is_compatible_rate(25));
    static_assert(time::is_compatible_rate(30));
    static_assert(time::is_compatible_rate(32));
    static_assert(time::is_compatible_rate(48));
    static_assert(time::is_compatible_rate(50));
    static_assert(time::is_compatible_rate(60));
    static_assert(time::is_compatible_rate(64));
    static_assert(time::is_compatible_rate(90));
    static_assert(time::is_compatible_rate(96));

    /* high-end video fps */

    static_assert(time::is_compatible_rate(100));
    static_assert(time::is_compatible_rate(120));
    static_assert(time::is_compatible_rate(144));
    static_assert(time::is_compatible_rate(180));
    static_assert(time::is_compatible_rate(192));
    static_assert(time::is_compatible_rate(240));

    /* NTSC fps */

    static_assert(time::is_compatible_rate(1000 * 24, 1001));
    // static_assert(time::is_compatible_rate(1000 * 25, 1001));  Not supported!
    static_assert(time::is_compatible_rate(1000 * 30, 1001));
    static_assert(time::is_compatible_rate(1000 * 60, 1001));
    static_assert(time::is_compatible_rate(1000 * 120, 1001));

    /* common audio frequency */

    static_assert(time::is_compatible_rate(1000));
    static_assert(time::is_compatible_rate(4000));
    static_assert(time::is_compatible_rate(8000));
    static_assert(time::is_compatible_rate(16000));
    static_assert(time::is_compatible_rate(22050));
    static_assert(time::is_compatible_rate(24000));
    static_assert(time::is_compatible_rate(32000));
    static_assert(time::is_compatible_rate(44100));
    static_assert(time::is_compatible_rate(48000));
    static_assert(time::is_compatible_rate(88200));
    static_assert(time::is_compatible_rate(96000));
    static_assert(time::is_compatible_rate(192000));

    /* high-end audio frequency */

    static_assert(time::is_compatible_rate(352800));
    static_assert(time::is_compatible_rate(384000));
    static_assert(time::is_compatible_rate(768000));
  }

  SECTION("overflow")
  {
    constexpr yave::time t;

    STATIC_REQUIRE(t.int_value() == 0);
    STATIC_REQUIRE(t == t.zero());
    STATIC_REQUIRE(t >= t.zero());
    STATIC_REQUIRE(t <= t.zero());

    STATIC_REQUIRE(t + t == t);
    STATIC_REQUIRE(t * t == t);
    STATIC_REQUIRE(t - t == t);

    STATIC_REQUIRE(-t.max() == t.min() + 1);
    STATIC_REQUIRE(-t.min() == t.max());

    STATIC_REQUIRE(t.max() + t.max() == t.max());
    STATIC_REQUIRE(t.min() + t.min() == t.min());
    STATIC_REQUIRE(t.zero() + t.min() == t.min());
    STATIC_REQUIRE(t.zero() + t.max() == t.max());
    STATIC_REQUIRE(t.min() + t.max() == t.zero() - 1);
    STATIC_REQUIRE(t.max() + t.min() == t.zero() - 1);

    STATIC_REQUIRE(t.max() - t.max() == t.zero());
    STATIC_REQUIRE(t.min() - t.min() == t.zero());
    STATIC_REQUIRE(t.max() - t.min() == t.max());
    STATIC_REQUIRE(t.min() - t.max() == t.min());
    STATIC_REQUIRE(t.zero() - t.max() == t.min() + 1);
    STATIC_REQUIRE(t.min() - t.zero() == t.min());

    STATIC_REQUIRE(t.max() * (t.zero() + 1) == t.max());
    STATIC_REQUIRE(t.min() * (t.zero() + 1) == t.min());
    STATIC_REQUIRE((t.zero() + 1) * t.max() == t.max());
    STATIC_REQUIRE((t.zero() + 1) * t.min() == t.min());
    STATIC_REQUIRE(t.max() * (t.zero() - 1) == t.min() + 1);
    STATIC_REQUIRE(t.min() * (t.zero() - 1) == t.max());
    STATIC_REQUIRE((t.zero() - 1) * t.max() == t.min() + 1);
    STATIC_REQUIRE((t.zero() - 1) * t.min() == t.max());

    STATIC_REQUIRE(t.max() * t.max() == t.max());
    STATIC_REQUIRE(t.max() * t.min() == t.min());
    STATIC_REQUIRE(t.min() * t.min() == t.max());
    STATIC_REQUIRE(t.min() * t.max() == t.min());

    STATIC_REQUIRE(t.max() / (t.zero() - 1) == t.min() + 1);
    STATIC_REQUIRE(t.min() / (t.zero() - 1) == t.max());
    STATIC_REQUIRE((t.zero() - 1) / t.max() == -(1 / t.max()));
    STATIC_REQUIRE((t.zero() - 1) / t.min() == -(1 / t.max()));

    STATIC_REQUIRE(++t.max() == t.max());
    STATIC_REQUIRE(t.max()++ == t.max());
    STATIC_REQUIRE(--t.min() == t.min());
    STATIC_REQUIRE(t.min()-- == t.min());
  }
}