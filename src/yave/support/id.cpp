//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/support/id.hpp>

#include <boost/random/random_device.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <cstring>
#include <random>

namespace yave {

  bool operator<(const uid& lhs, const uid& rhs) noexcept
  {
    return lhs.data < rhs.data;
  }

  bool operator>(const uid& lhs, const uid& rhs) noexcept
  {
    return lhs.data > rhs.data;
  }

  bool operator<=(const uid& lhs, const uid& rhs) noexcept
  {
    return lhs.data <= rhs.data;
  }

  bool operator>=(const uid& lhs, const uid& rhs) noexcept
  {
    return lhs.data >= rhs.data;
  }

  bool operator==(const uid& lhs, const uid& rhs) noexcept
  {
    return lhs.data == rhs.data;
  }

  bool operator!=(const uid& lhs, const uid& rhs) noexcept
  {
    return lhs.data != rhs.data;
  }

  std::string to_string(const uid& id)
  {
    return std::to_string(id.data);
  }

  uid uid::random_generate()
  {
    static_assert(std::is_same_v<uint64_t, std::uint_fast64_t>);
    boost::random::random_device seed_gen;
    boost::random::mt19937_64 engine(seed_gen());
    return uid {engine()};
  }

} // namespace yave