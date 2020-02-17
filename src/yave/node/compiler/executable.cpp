//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/executable.hpp>
#include <yave/obj/behaviour/frame_demand.hpp>

#include <yave/rts/rts.hpp>

namespace yave {

  executable::executable(
    object_ptr<const Object> obj,
    object_ptr<const Type> type)
    : m_obj {std::move(obj)}
    , m_type {std::move(type)}
  {
  }

  executable::executable(const executable& other)
    : m_obj {other.m_obj}
    , m_type {other.m_type}
  {
  }

  executable::executable(executable&& other) noexcept
    : m_obj {std::move(other.m_obj)}
    , m_type {std::move(other.m_type)}
  {
  }

  executable& executable::operator=(const executable& other)
  {
    m_obj  = other.m_obj;
    m_type = other.m_type;
    return *this;
  }

  executable& executable::operator=(executable&& other) noexcept
  {
    m_obj  = std::move(other.m_obj);
    m_type = std::move(other.m_type);
    return *this;
  }

  auto executable::object() const -> const object_ptr<const Object>&
  {
    return m_obj;
  }

  auto executable::type() const -> const object_ptr<const Type>&
  {
    return m_type;
  }

  auto executable::execute(const frame_demand& demand)
    -> object_ptr<const Object>
  {
    return eval(m_obj << make_object<FrameDemand>(demand));
  }
}