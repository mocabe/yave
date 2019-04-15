#include <catch2/catch.hpp>

#include <yave/time/time.hpp>

TEST_CASE("time")
{
  // assume int64_t for this test
  static_assert(std::is_same_v<yave::time::value_type, yave::int64_t>);

  yave::time t;

  REQUIRE(t.int_value() == 0);
  REQUIRE(t == t.zero());
  REQUIRE(t >= t.zero());
  REQUIRE(t <= t.zero());

  // FIXME
  REQUIRE(t.max() + t.max() == t.max());
  REQUIRE(t.min() + t.min() == t.min());
  REQUIRE(t.min() - t.max() == t.min());
  REQUIRE(t.max() - t.min() == t.max());
}