//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/obj/node/argument.hpp>

namespace yave {

  /// generic node argument holder
  class node_argument_holder
  {
  public:
    virtual ~node_argument_holder() noexcept = default;

    /// create runtime representation
    virtual auto compile() const -> object_ptr<const Object> = 0;
  };

  /// data holder which contains data and property for node arguments.
  class node_argument_holder_object_value
  {
    std::unique_ptr<node_argument_holder> m_holder;

  public:
    node_argument_holder_object_value(
      std::unique_ptr<node_argument_holder> holder)
      : m_holder {std::move(holder)}
    {
    }

    node_argument_holder_object_value(const node_argument_holder_object_value&)
      : m_holder {}
    {
      assert(false && "This value should not be cloned");
    }

    /// get object
    auto compile() const
    {
      return m_holder->compile();
    }
  };

  /// node argument holder wrapper (not ABI stable)
  using NodeArgumentHolder = Box<node_argument_holder_object_value>;

  // node argument which generate data constructor
  class data_node_argument : public node_argument_holder
  {
    object_ptr<NodeArgument> m_arg;

  public:
    data_node_argument(object_ptr<NodeArgument> arg)
      : m_arg {std::move(arg)}
    {
    }

    data_node_argument(const data_node_argument&) = delete;

    auto compile() const -> object_ptr<const Object> override
    {
      return m_arg->generate(m_arg);
    }
  };

  [[nodiscard]] auto make_argument_holder(const object_ptr<NodeArgument>& arg)
  {
    return make_object<NodeArgumentHolder>(
      std::make_unique<data_node_argument>(arg));
  }

  // argument prop for variables
  class variable_node_argument : public node_argument_holder
  {
    object_ptr<const Variable> m_var;

  public:
    variable_node_argument(object_ptr<const Variable> var)
      : m_var {std::move(var)}
    {
    }

    auto compile() const -> object_ptr<const Object> override
    {
      return m_var;
    }
  };

  [[nodiscard]] auto make_argument_holder(object_ptr<const Variable> var)
  {
    return make_object<NodeArgumentHolder>(
      std::make_unique<variable_node_argument>(std::move(var)));
  }

} // namespace yave

YAVE_DECL_TYPE(yave::NodeArgumentHolder, "783be464-01d9-4c90-b2cb-40bdfc3d13fa");