//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/data/lib/keyframe.hpp>
#include <yave/data/obj/primitive.hpp>

#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("Keyframe")
{
  SECTION("Int")
  {
    keyframe<Int> i;

    REQUIRE(i.empty());
    REQUIRE(i.size() == 0);
    REQUIRE(i.keys().empty());
    REQUIRE(i.values().empty());

    SECTION("insert to empty")
    {
      i.insert(yave::time(24), 42);
      REQUIRE(!i.empty());
      REQUIRE(i.size() == 1);
      REQUIRE(i.keys()[0] == yave::time(24));
      REQUIRE(i.values().size() == 1);

      SECTION("remove")
      {
        i.remove(yave::time(24));

        REQUIRE(i.empty());
        REQUIRE(i.size() == 0);
        REQUIRE(i.keys().empty());
        REQUIRE(i.values().empty());
      }

      SECTION("insert another key")
      {
        i.insert(yave::time(42), 24);
        REQUIRE(!i.empty());
        REQUIRE(i.size() == 2);
        REQUIRE(i.keys()[0].count() == 24);
        REQUIRE(i.keys()[1].count() == 42);
        REQUIRE(i.keys()[0] == yave::time(24));
        REQUIRE(i.keys()[1] == yave::time(42));
        REQUIRE(*i.values()[0] == 42);
        REQUIRE(*i.values()[1] == 24);

        SECTION("remove both")
        {
          i.remove(42);
          i.remove(24);

          REQUIRE(i.empty());
          REQUIRE(i.size() == 0);
          REQUIRE(i.keys().empty());
          REQUIRE(i.values().empty());
        }
      }

      SECTION("find")
      {
        REQUIRE(i.find_value(24));
      }
    }
  }
}
