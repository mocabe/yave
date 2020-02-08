//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/rts/rts.hpp>
#include <catch2/catch.hpp>

using namespace yave;

namespace yave {
  using Int    = yave::Box<int>;
  using Double = yave::Box<double>;
} // namespace yave

YAVE_DECL_TYPE(Int, "7d27665a-c56a-40d1-8e2e-844cb48de9e9");
YAVE_DECL_TYPE(Double, "9cc69b38-8766-44f1-93e9-337cfb3d3bc5");

TEST_CASE("runtime_error", "[rts][exception]")
{
  struct F : Function<F, Int, Int>
  {
    return_type code() const
    {
      throw std::runtime_error("Hello, Exception!");
    }
  };

  auto f = make_object<F>();
  auto i = make_object<Int>();

  SECTION("0")
  {
    auto app = f << i;
    REQUIRE_THROWS_AS(eval(app), exception_result);
    REQUIRE_THROWS_AS(eval(app), exception_result);

    try {
      (void)eval(app);
    } catch (exception_result& e) {
      REQUIRE(e.exception()->message() == "Hello, Exception!");
    }
  }

  SECTION("1")
  {
    auto app = f << (f << i);
    REQUIRE_THROWS_AS(eval(app), exception_result);
    REQUIRE_THROWS_AS(eval(app), exception_result);

    try {
      (void)eval(app);
    } catch (exception_result& e) {
      REQUIRE(e.exception()->message() == "Hello, Exception!");
    }
  }
}

TEST_CASE("Null return", "[rts][exception]")
{
  struct F : Function<F, Int, Int>
  {
    return_type code() const
    {
      return object_ptr<Int>();
    }
  };

  auto f = make_object<F>();
  auto i = make_object<Int>();

  auto app = f << i;

  REQUIRE_NOTHROW(type_of(app));
  REQUIRE_THROWS_AS(eval(app), exception_result);
  REQUIRE_THROWS_AS(eval(app), exception_result);

  try {
    (void)eval(app);
    REQUIRE(false);
  } catch (exception_result& e) {
    REQUIRE(
      value_cast<ResultError>(e.exception()->error())->error_type ==
      result_error_type::null_result);
  }
}

TEST_CASE("unknown exception", "[rts][exception]")
{
  struct F : Function<F, Int, Int>
  {
    return_type code() const
    {
      auto msg = make_object<String>("custom exception");
      auto err = make_object<Double>(3.14);

      return make_object<Exception>(msg, err);
      return make_object<const Exception>(msg, err);
      return new (std::pmr::new_delete_resource()) Exception(msg, err);
      return new (std::pmr::new_delete_resource()) const Exception(msg, err);
    }
  };

  auto f = make_object<F>();
  auto i = make_object<Int>();

  SECTION("0")
  {
    auto app = f << i;
    REQUIRE_THROWS_AS(eval(app), exception_result);
    REQUIRE_THROWS_AS(eval(app), exception_result);

    try {
      (void)eval(app);
    } catch (exception_result& e) {
      REQUIRE(e.exception()->error());
      REQUIRE(*value_cast<Double>(e.exception()->error()) == 3.14);
    }
  }

  SECTION("1")
  {
    auto app = f << (f << i);
    REQUIRE_THROWS_AS(eval(app), exception_result);
    REQUIRE_THROWS_AS(eval(app), exception_result);

    try {
      (void)eval(app);
    } catch (exception_result& e) {
      REQUIRE(e.exception()->error());
      REQUIRE(*value_cast<Double>(e.exception()->error()) == 3.14);
    }
  }
}