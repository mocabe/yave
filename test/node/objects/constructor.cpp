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
    REQUIRE(has_type<Int>(v->data()));
  }

  SECTION("eval")
  {
    auto v = make_node_argument<Int>();
    check_type_dynamic<NodeArgument>(v);
    check_type_dynamic<closure<NodeArgument, FrameDemand, Int>>(v->ctor());
    check_type_dynamic<Int>(v->data());
    check_type_dynamic<IntDataProperty>(v->property());
    v->set_data(make_object<Int>(42));
    auto app = (v->get_data_constructor(v)) << make_object<FrameDemand>();
    check_type_dynamic<Int>(app);
    REQUIRE(*value_cast<Int>(eval(app)) == 42);
  }
}