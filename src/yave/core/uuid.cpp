//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/core/uuid.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>

#include <cstring>

namespace yave {

  namespace {

    static_assert(sizeof(uuid) == 16);

    boost::uuids::uuid to_boost_uuid(const uuid& id) noexcept
    {
      boost::uuids::uuid ret;
      std::memcpy(ret.data, &id, 16);
      return ret;
    }

    uuid to_uuid(const boost::uuids::uuid& id) noexcept
    {
      uuid ret;
      std::memcpy(&ret, id.data, 16);
      return ret;
    }

  } // namespace

   uuid uuid::random_generate()
  {
    auto gen       = boost::uuids::random_generator_mt19937 {};
    auto generated = gen();
    return to_uuid(generated);
  }

  uuid uuid::from_string(std::string_view str)
  {
    auto gen       = boost::uuids::string_generator {};
    auto generated = gen(str.begin(), str.end());
    return to_uuid(generated);
  }

   bool operator<(const uuid& lhs, const uuid& rhs) noexcept
  {
    return to_boost_uuid(lhs) < to_boost_uuid(rhs);
  }

   bool operator>(const uuid& lhs, const uuid& rhs) noexcept
  {
    return to_boost_uuid(lhs) > to_boost_uuid(rhs);
  }

   bool operator<=(const uuid& lhs, const uuid& rhs) noexcept
  {
    return to_boost_uuid(lhs) <= to_boost_uuid(rhs);
  }

   bool operator>=(const uuid& lhs, const uuid& rhs) noexcept
  {
    return to_boost_uuid(lhs) >= to_boost_uuid(rhs);
  }

   bool operator==(const uuid& lhs, const uuid& rhs) noexcept
  {
    return to_boost_uuid(lhs) == to_boost_uuid(rhs);
  }

   bool operator!=(const uuid& lhs, const uuid& rhs) noexcept
  {
    return to_boost_uuid(lhs) != to_boost_uuid(rhs);
  }

   std::string to_string(const uuid& id)
  {
    auto tmp = to_boost_uuid(id);
    return boost::uuids::to_string(tmp);
  }
}