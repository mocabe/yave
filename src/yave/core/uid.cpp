//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/core/uid.hpp>

#include <boost/random/random_device.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <fmt/format.h>

#include <random>

namespace yave {

  namespace detail {

    uint64_t uid_random()
    {
      static_assert(std::is_same_v<uint64_t, std::uint_fast64_t>);
      boost::random::random_device seed_gen;
      boost::random::mt19937_64 engine(seed_gen());
      return engine();
    }

    std::string uid_to_string(uint64_t id)
    {
      return fmt::format("{:0>16x}", id);
    }

  } // namespace detail

} // namespace yave