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

  auto node_declaration_traits<node::Animation::MergeOr>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Animation.MergeOr",
      "Parallel OR merge animations.\n"
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

  auto node_declaration_traits<
    node::Animation::MergeAnd>::get_node_declaration() -> node_declaration
  {
    return function_node_declaration(
      "Animation.MergeAnd",
      "Parallel AND merge animations.\n"
      "\n"
      "pseudocode:\n"
      "(match (a1.value , a2.value)\n"
      "  | (a, blank) OR (blank, a) => blank \n"
      "  | (a, b) => fn a b,\n"
      " min a1.len a2.len)\n"
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

    // helper: lift constant length to signal
    struct ConstLen : SignalFunction<ConstLen, FrameTime>
    {
      object_ptr<const FrameTime> m_len;

      ConstLen(time len)
        : m_len {make_object<FrameTime>(len)}
      {
      }

      auto code() const -> return_type
      {
        return m_len;
      }
    };

    class X;
    class Y;

    struct Animation : SignalFunction<Animation, X, FrameTime, Anim<X>>
    {
      auto code() const -> return_type
      {
        auto v = arg_signal<0>();
        auto l = arg_signal<1>();
        assert(v && l);
        return make_object<Anim<X>>(std::move(v), std::move(l));
      }
    };

    struct Blank : SignalFunction<Blank, FrameTime, Anim<X>>
    {
      auto code() const -> return_type
      {
        auto l = arg_signal<0>();
        return make_object<Anim<VarValueProxy<X>>>(std::move(l));
      }
    };

    struct IsBlank : SignalFunction<IsBlank, Anim<X>, Bool>
    {
      auto code() const -> return_type
      {
        auto v = eval_arg<0>();
        auto b = make_object<Bool>(true);

        if (v->is_blank())
          return b;

        auto l = eval(v->length() << arg_demand());

        if (*l == time::zero())
          return b;

        if (*l < *arg_time())
          return b;

        *b = false;
        return b;
      }
    };

    struct GetValue : SignalFunction<GetValue, Anim<X>, X>
    {
      auto code() const -> return_type
      {
        auto l = eval_arg<0>();

        if (l->is_blank())
          throw std::runtime_error("Animation is blank");

        if (*eval(l->length() << arg_demand()) < *arg_time())
          throw std::runtime_error("Animation out of range");

        return l->value() << arg_demand();
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
      auto code() const -> return_type
      {
        auto a = eval_arg<0>();

        if (a->is_blank())
          return make_object<Anim<Y>>(a->length());

        auto f = arg_signal<1>();
        return make_object<Anim<Y>>(std::move(f) << a->value(), a->length());
      }
    };

    struct Concat : SignalFunction<Concat, Anim<X>, Anim<X>, Anim<X>>
    {
      struct DelayedValue : SignalFunction<DelayedValue, X, X>
      {
        time m_delay;

        DelayedValue(time delay)
          : m_delay {delay}
        {
        }

        auto code() const -> return_type
        {
          return arg_signal<0>() << make_object<FrameDemand>(
                   make_object<FrameTime>(*arg_time() - m_delay));
        }
      };

      auto code() const -> return_type
      {
        auto a1 = eval_arg<0>();
        auto a2 = eval_arg<1>();

        auto l1 = eval(a1->length() << arg_demand());
        auto l2 = eval(a2->length() << arg_demand());

        // first animation
        if (*arg_time() <= *l1)
          return a1;

        auto lout = make_object<ConstLen>(*l1 + *l2);

        if (a2->is_blank())
          return make_object<Anim<X>>(lout);

        return make_object<Anim<X>>(
          make_object<DelayedValue>(*l1) << a2->value(), lout);
      }
    };

    struct MergeOr
      : SignalFunction<MergeOr, Anim<X>, Anim<X>, sf<X, X, X>, Anim<X>>
    {
      auto code() const -> return_type
      {
        auto a1 = eval_arg<0>();
        auto a2 = eval_arg<1>();

        if (a1->is_blank())
          return a2;
        if (a2->is_blank())
          return a1;

        // max(a1.len, a2.len) is used for new length
        auto l1  = eval(a1->length() << arg_demand());
        auto l2  = eval(a2->length() << arg_demand());
        auto len = *l1 > *l2 ? a1->length() : a2->length();

        return make_object<Anim<X>>(
          arg_signal<2>() << a1->value() << a2->value(), len);
      }
    };

    struct MergeAnd
      : SignalFunction<MergeAnd, Anim<X>, Anim<X>, sf<X, X, X>, Anim<X>>
    {
      auto code() const -> return_type
      {
        auto a1 = eval_arg<0>();
        auto a2 = eval_arg<1>();

        if (a1->is_blank())
          return a1;
        if (a2->is_blank())
          return a2;

        // min(a1.len, a2.len) is used for new length
        auto l1  = eval(a1->length() << arg_demand());
        auto l2  = eval(a2->length() << arg_demand());
        auto len = *l1 > *l2 ? a2->length() : a1->length();

        return make_object<Anim<X>>(
          arg_signal<2>() << a1->value() << a2->value(), len);
      }
    };

    struct Stretch : SignalFunction<Stretch, Anim<X>, FrameTime, Anim<X>>
    {
      // remapper
      struct Remap : SignalFunction<Remap, X, X>
      {
        time m_old, m_new;

        Remap(time o, time n)
          : m_old {o}
          , m_new {n}
        {
        }

        auto code() const -> return_type
        {
          if (m_new == time::zero())
            return arg<0>();

          auto r = m_old.seconds() / m_new.seconds();
          auto t = make_object<FrameTime>(*arg_time() * r);

          return arg_signal<0>() << make_object<FrameDemand>(std::move(t));
        }
      };

      auto code() const -> return_type
      {
        auto a = eval_arg<0>();

        auto oldlen = *eval(a->length() << arg_demand());
        auto newlen = *eval_arg<1>();

        if (newlen < data::frame_time::zero())
          newlen = data::frame_time::zero();

        auto lout = make_object<ConstLen>(newlen);

        if (a->is_blank())
          return make_object<Anim<X>>(lout);

        return make_object<Anim<X>>(
          make_object<Remap>(oldlen, newlen) << a->value(), lout);
      }
    };

    struct Extend : SignalFunction<Extend, Anim<X>, FrameTime, Anim<X>>
    {
      auto code() const -> return_type
      {
        auto a    = eval_arg<0>();
        auto alen = eval(a->length() << arg_demand());
        auto l    = eval_arg<1>();

        auto len = *l;

        if (len < time::zero())
          len = time::zero();

        auto lout = make_object<ConstLen>(len);

        if (a->is_blank())
          return make_object<Anim<X>>(lout);

        if (*alen >= *arg_time())
          return make_object<Anim<X>>(a->value(), lout);

        return make_object<Anim<X>>(lout);
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
  ANIM_NODE_DEFINITION(MergeOr);
  ANIM_NODE_DEFINITION(MergeAnd);
  ANIM_NODE_DEFINITION(Stretch);
  ANIM_NODE_DEFINITION(Extend);

} // namespace yave
