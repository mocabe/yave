//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/executable.hpp>
#include <yave/core/objects/frame.hpp>

#include <yave/core/rts.hpp>

namespace yave {

  Executable::Executable(
    object_ptr<const Object> obj,
    object_ptr<const Type> type)
    : m_obj {std::move(obj)}
    , m_type {std::move(type)}
  {
  }

  Executable::Executable(const Executable& other)
    : m_obj {other.m_obj}
    , m_type {other.m_type}
  {
  }

  Executable::Executable(Executable&& other)
    : m_obj {std::move(other.m_obj)}
    , m_type {std::move(other.m_type)}
  {
  }

  Executable& Executable::operator=(const Executable& other)
  {
    m_obj  = other.m_obj;
    m_type = other.m_type;
    return *this;
  }

  Executable& Executable::operator=(Executable&& other)
  {
    m_obj  = std::move(other.m_obj);
    m_type = std::move(other.m_type);
    return *this;
  }

  object_ptr<const Type> Executable::type() const
  {
    return m_type;
  }

  object_ptr<const Object> Executable::execute(yave::frame frame) const
  {
    auto app = m_obj << make_object<Frame>(frame);
    return eval(app);
  }
}