//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/data/filesystem/path.hpp>
#include <catch2/catch.hpp>

using namespace yave::data::filesystem;

TEST_CASE("yave::filesystem::path")
{
  path p1                    = "";
  auto p2                    = p1;
  auto p3                    = std::move(p2);
  p2                         = p3;
  p3                         = std::move(p2);
  std::filesystem::path pstd = path(std::filesystem::path());
  REQUIRE(pstd.empty());
}