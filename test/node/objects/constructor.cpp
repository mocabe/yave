//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/obj/primitive/property.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/rts/rts.hpp>

using namespace yave;

TEST_CASE("Constructor")
{
  SECTION("make_object")
  {
    auto v = make_node_argument<Int>();
    auto p = v->prop_tree();
    REQUIRE(same_type(p->type(), object_type<Int>()));
    REQUIRE(has_type<Int>(p->value()));
  }

  SECTION("misc")
  {
    REQUIRE(make_node_argument<Int>()->prop_tree()->value());
    REQUIRE(make_node_argument<Float>()->prop_tree()->value());
    REQUIRE(make_node_argument<String>()->prop_tree()->value());
    REQUIRE(make_node_argument<Bool>()->prop_tree()->value());
  }

  SECTION("eval")
  {
    auto v = make_node_argument<Int>();
    auto p = v->prop_tree();
    check_type_dynamic<NodeArgument>(v);
    check_type_dynamic<Int>(p->value());
    p->set_value(make_object<Int>(42));
    REQUIRE(*value_cast<Int>(p->value()) == 42);
    REQUIRE(get_node_argument_value<Int>(p) == 42);
    set_node_argument_value<Int>(p, 24);
    REQUIRE(get_node_argument_value<Int>(p) == 24);
    set_node_argument_value<Int>(p, 42);
    auto app = v->generate(v) << make_object<FrameDemand>();
    check_type_dynamic<Int>(app);
    REQUIRE(*value_cast<Int>(eval(app)) == 42);
  }
}