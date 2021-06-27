//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/animation/animation.hpp>
#include <yave/signal/function.hpp>
#include <yave/signal/anim.hpp>

namespace yave {

  auto node_declaration_traits<
    node::Animation::Animation>::get_node_declaration() -> node_declaration
  {
    return function_node_declaration(
      "Animation.Animation",
      "Create new animation value.\n"
      "\n"
      "Animation is represented as pair of signal value and its length.\n"
      "Animation value is composable over time.\n"
      "\n"
      "Animation can be blank (nothing)\n"
      "Use Animation.Blank to create blank animation.\n"
      "Use Animation.IsBlank to check if animation is currently blank.\n"
      "When it is not blank you can use GetValue to get content.",
      node_declaration_visibility::_public,
      {"val", "len"},
      {"anim"});
  }

  auto node_declaration_traits<node::Animation::Blank>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Animation.Blank",
      "Create new blank animation of given length.",
      node_declaration_visibility::_public,
      {"len"},
      {"anim"});
  }

  auto node_declaration_traits<node::Animation::IsBlank>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Animation.IsBlank",
      "Is animation blank at current time?",
      node_declaration_visibility::_public,
      {"anim"},
      {"bool"});
  }

  auto node_declaration_traits<
    node::Animation::GetValue>::get_node_declaration() -> node_declaration
  {
    return function_node_declaration(
      "Animation.GetValue",
      "Get content of animation.\n"
      "This node throws when input animation is blank or out of range",
      node_declaration_visibility::_public,
      {"anim"},
      {"value"});
  }

  auto node_declaration_traits<
    node::Animation::GetLength>::get_node_declaration() -> node_declaration
  {
    return function_node_declaration(
      "Animation.GetLength",
      "Get length of animation",
      node_declaration_visibility::_public,
      {"anim"},
      {"length"});
  }

  auto node_declaration_traits<node::Animation::Map>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Animation.Map",
      "Transform animation value\n"
      "param:\n"
      "  a : Animation of type a\n"
      "  fn: Mapping function of (a -> b)\n"
      "return:\n"
      "  Animation of type b",
      node_declaration_visibility::_public,
      {"a", "fn"},
      {"anim"});
  }

  auto node_declaration_traits<node::Animation::Concat>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Animation.Concat",
      "Append animation sequentially.\n"
      "param:\n"
      "  a1, a2: Animation of type a\n"
      "return\n"
      "  New animation which has sum of length of a1 and a2",
      node_declaration_visibility::_public,
      {"a1", "a2"},
      {"anim"});
  }

  auto node_declaration_traits<node::Animation::Merge>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Animation.Merge",
      "Parallel merge animations.\n"
      "\n"
      "pseudocode:\n"
      "(match (a1.value , a2.value)\n"
      "  | (a, blank) or (blank, a) => a \n"
      "  | (a, b) => fn a b,\n"
      " max a1.len a2.len)\n"
      "\n"
      "param:\n"
      "  a1, a2: Animation of type a\n"
      "  fn    : Function of (a -> a -> a)\n"
      "\n"
      "return:\n"
      "  Animation of a",
      node_declaration_visibility::_public,
      {"a1", "a2", "fn"},
      {"anim"});
  }

  auto node_declaration_traits<node::Animation::Stretch>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Animation.Stretch",
      "Stretch animation to specified length\n"
      "\n"
      "param:\n"
      "  a  : Animation of type a\n"
      "  len: New length\n"
      "\n"
      "return:\n"
      "  Animation of a with length len\n",
      node_declaration_visibility::_public,
      {"a", "len"},
      {"anim"});
  }

  auto node_declaration_traits<node::Animation::Extend>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Animation.Extend",
      "Extend or shrink animation into given length.\n"
      "When returned animation is longer than original,\n"
      "it will return blank while extended period.\n"
      "\n"
      "param:\n"
      "  a  : Animation of type a\n"
      "  len: New length of animation\n"
      "\n"
      "return:\n"
      "  Animation of type a with new length\n"
      "\n",
      node_declaration_visibility::_public,
      {"a", "len"},
      {"anim"});
  }

  namespace node::Animation::detail {

    // TODO: Convert some nodes to composed declaration

    class X;
    class Y;

    // helper: lift constant length to signal
    struct ConstLen : SignalFunction<ConstLen, FrameTime>
    {
      object_ptr<const FrameTime> m_len;

      ConstLen(media::time len)
        : m_len {make_object<FrameTime>(len)}
      {
      }

      auto code() const -> return_type
      {
        return m_len;
      }
    };

    // helper: make just
    struct Just : SignalFunction<Just, X, SMaybe<X>>
    {
      auto code() const -> return_type
      {
        return make_object<SMaybe<X>>(arg_signal<0>());
      }
    };

    // helper: make nothing
    struct Nothing : SignalFunction<Nothing, SMaybe<X>>
    {
      auto code() const -> return_type
      {
        return make_object<SMaybe<X>>();
      }
    };

    // helper: delay signal time
    struct Delay : SignalFunction<Delay, X, X>
    {
      media::time m_delay;

      Delay(media::time delay)
        : m_delay {delay}
      {
      }

      auto code() const -> return_type
      {
        // t = t - delay
        auto t = *arg_demand()->time - m_delay;
        return arg_signal<0>()
               << make_object<FrameDemand>(make_object<FrameTime>(t));
      }
    };

    // helper: scale signal time
    struct Scale : SignalFunction<Scale, X, X>
    {
      double m_scale;

      Scale(double scale)
        : m_scale {scale}
      {
      }

      auto code() const -> return_type
      {
        // t * scale
        auto t = *arg_time() * m_scale;
        return arg_signal<0>()
               << make_object<FrameDemand>(make_object<FrameTime>(t));
      }
    };

    struct Animation : SignalFunction<Animation, X, FrameTime, Anim<X>>
    {
      auto code() const -> return_type
      {
        auto v = arg_signal<0>();
        auto l = arg_signal<1>();
        assert(v && l);
        return make_object<Anim<X>>(
          make_object<Just>() << std::move(v), std::move(l));
      }
    };

    struct Blank : SignalFunction<Blank, FrameTime, Anim<X>>
    {
      auto code() const -> return_type
      {
        auto l = arg_signal<0>();
        return make_object<Anim<X>>(std::move(l));
      }
    };

    struct IsBlank : SignalFunction<IsBlank, Anim<X>, Bool>
    {
      auto code() const -> return_type
      {
        auto v = eval_arg<0>();
        auto b = make_object<Bool>(true);

        auto l = eval(v->length() << arg_demand());

        // zero length
        if (*l == media::time::zero())
          return b;

        auto t = arg_time();

        // out of range
        if (*t < media::time::zero() || *l < *t)
          return b;

        // nothing
        if (eval(v->value() << arg_demand())->is_nothing())
          return b;

        *b = false;
        return b;
      }
    };

    struct GetValue : SignalFunction<GetValue, Anim<X>, X>
    {
      auto code() const -> return_type
      {
        auto a = eval_arg<0>();
        auto t = arg_time();

        // check length
        auto l = eval(a->length() << arg_demand());
        if (*t < media::time::zero() || *l < *t)
          throw std::runtime_error("Animation out of range");

        // check value
        auto v = eval(a->value() << arg_demand());

        if (v->is_nothing())
          throw std::runtime_error("Animation is blank");

        return v->value() << arg_demand();
      }
    };

    struct GetLength : SignalFunction<GetLength, Anim<X>, FrameTime>
    {
      auto code() const -> return_type
      {
        return eval_arg<0>()->length() << arg_demand();
      }
    };

    struct Map : SignalFunction<Map, Anim<X>, sf<X, Y>, Anim<Y>>
    {
      struct Thunk : SignalFunction<Thunk, SMaybe<X>, sf<X, Y>, SMaybe<Y>>
      {
        auto code() const -> return_type
        {
          auto v = eval_arg<0>();

          if (v->is_nothing())
            return make_object<SMaybe<Y>>();

          auto r = arg_signal<1>() << v->value();
          return make_object<SMaybe<Y>>(r);
        }
      };

      auto code() const -> return_type
      {
        auto a = eval_arg<0>();
        auto f = arg_signal<1>();
        return make_object<Anim<Y>>(
          make_object<Thunk>() << a->value() << f, a->length());
      }
    };

    struct Concat : SignalFunction<Concat, Anim<X>, Anim<X>, Anim<X>>
    {
      struct Thunk : SignalFunction<Thunk, SMaybe<X>, SMaybe<X>, SMaybe<X>>
      {
        // length of first animation
        media::time m_len;

        Thunk(media::time len)
          : m_len {len}
        {
        }

        auto code() const -> return_type
        {
          auto t = arg_time();

          // first animation
          if (*t < m_len)
            return arg<0>();

          // delay operator
          auto delay = make_object<Delay>(m_len);

          // delayed Maybe value
          auto v = eval((delay << arg_signal<1>()) << arg_demand());

          if (v->is_nothing())
            return v;

          // delay wrapped signal
          return make_object<SMaybe<X>>(delay << v->value());
        }
      };

      auto code() const -> return_type
      {
        auto a1 = eval_arg<0>();
        auto a2 = eval_arg<1>();

        auto l1 = eval(a1->length() << arg_demand());
        auto l2 = eval(a2->length() << arg_demand());

        return make_object<Anim<X>>(
          make_object<Thunk>(*l1) << a1->value() << a2->value(),
          make_object<ConstLen>(*l1 + *l2));
      }
    };

    struct Merge : SignalFunction<Merge, Anim<X>, Anim<X>, sf<X, X, X>, Anim<X>>
    {
      struct Thunk
        : SignalFunction<Thunk, SMaybe<X>, SMaybe<X>, sf<X, X, X>, SMaybe<X>>
      {
        auto code() const -> return_type
        {
          auto v1 = eval_arg<0>();
          auto v2 = eval_arg<1>();
          auto f  = arg_signal<2>();

          if (v1->is_nothing())
            return v2;

          if (v2->is_nothing())
            return v1;

          return make_object<SMaybe<X>>(f << v1->value() << v2->value());
        }
      };

      auto code() const -> return_type
      {
        auto a1 = eval_arg<0>();
        auto a2 = eval_arg<1>();

        // max(a1.len, a2.len) is used for new length
        auto l1  = eval(a1->length() << arg_demand());
        auto l2  = eval(a2->length() << arg_demand());
        auto len = *l1 > *l2 ? a1->length() : a2->length();

        return make_object<Anim<X>>(
          make_object<Thunk>() << a1->value() << a2->value() << arg_signal<2>(),
          len);
      }
    };

    struct Stretch : SignalFunction<Stretch, Anim<X>, FrameTime, Anim<X>>
    {
      // remapper
      struct Thunk : SignalFunction<Thunk, SMaybe<X>, SMaybe<X>>
      {
        media::time m_old, m_new;

        Thunk(media::time o, media::time n)
          : m_old {o}
          , m_new {n}
        {
        }

        auto code() const -> return_type
        {
          if (m_new == media::time::zero())
            return arg<0>();

          auto r = m_old.seconds() / m_new.seconds();

          // scale operator
          auto scale = make_object<Scale>(r);

          auto t = make_object<FrameTime>(*arg_time() * r);

          // get maybe
          auto v = eval((scale << arg_signal<0>()) << arg_demand());

          if (v->is_nothing())
            return v;

          // scale wrapped signal
          return make_object<SMaybe<X>>(scale << v->value());
        }
      };

      auto code() const -> return_type
      {
        auto a = eval_arg<0>();

        auto oldlen = *eval(a->length() << arg_demand());
        auto newlen = *eval_arg<1>();

        if (newlen < data::frame_time::zero())
          newlen = data::frame_time::zero();

        return make_object<Anim<X>>(
          make_object<Thunk>(oldlen, newlen) << a->value(),
          make_object<ConstLen>(newlen));
      }
    };

    struct Extend : SignalFunction<Extend, Anim<X>, FrameTime, Anim<X>>
    {
      struct Thunk : SignalFunction<Thunk, SMaybe<X>, SMaybe<X>>
      {
        media::time m_old_len, m_new_len;

        Thunk(media::time old_len, media::time new_len)
          : m_old_len {old_len}
          , m_new_len {new_len}
        {
        }

        auto code() const -> return_type
        {
          auto v = eval_arg<0>();

          if (m_old_len >= *arg_time())
            return v;

          return make_object<SMaybe<X>>();
        }
      };

      auto code() const -> return_type
      {
        auto a = eval_arg<0>();

        auto old_len = *eval(a->length() << arg_demand());
        auto new_len = *eval_arg<1>();

        if (new_len < media::time::zero())
          new_len = media::time::zero();

        return make_object<Anim<X>>(
          make_object<Thunk>(old_len, new_len) << a->value(),
          make_object<ConstLen>(new_len));
      }
    };

  } // namespace node::Animation::detail

#define ANIM_NODE_DEFINITION(NAME)                                         \
  auto node_definition_traits<node::Animation::NAME, modules::_std::tag>:: \
    get_node_definitions()                                                 \
      ->std::vector<node_definition>                                       \
  {                                                                        \
    auto info = get_node_declaration<node::Animation::NAME>();             \
    return {node_definition(                                               \
      info.full_name(), 0, make_object<node::Animation::detail::NAME>())}; \
  }

  ANIM_NODE_DEFINITION(Animation);
  ANIM_NODE_DEFINITION(Blank);
  ANIM_NODE_DEFINITION(IsBlank);
  ANIM_NODE_DEFINITION(GetValue);
  ANIM_NODE_DEFINITION(GetLength);
  ANIM_NODE_DEFINITION(Map);
  ANIM_NODE_DEFINITION(Concat);
  ANIM_NODE_DEFINITION(Merge);
  ANIM_NODE_DEFINITION(Stretch);
  ANIM_NODE_DEFINITION(Extend);

} // namespace yave
