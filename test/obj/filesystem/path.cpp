//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/obj/filesystem/path.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("path")
{
  auto p = make_object<FilesystemPath>();
  REQUIRE(std::filesystem::path(*p).empty());
}