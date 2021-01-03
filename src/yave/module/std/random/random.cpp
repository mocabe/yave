//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/random/random.hpp>
#include <yave/signal/function.hpp>
#include <yave/obj/primitive/property.hpp>
#include <random>

namespace yave {

  auto node_declaration_traits<node::Random::Uniform>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Random.Uniform",
      "Generate random number with normal distribution\n"
      "\n"
      "params:\n"
      " idx: index in random number sequence\n"
      " min: min of distribution\n"
      " max: max of distribution\n"
      " seed: seed of random number sequence\n",
      node_declaration_visibility::_public,
      {"idx", "min", "max", "seed"},
      {"int"},
      {{0, make_node_argument<Int>(0)},
       {1, make_node_argument<Float>(0.0)},
       {2, make_node_argument<Float>(1.0)},
       {3, make_node_argument<Int>(0.0)}});
  }

  auto node_declaration_traits<node::Random::Normal>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Random.Normal",
      "Generate random number with normal distribution\n"
      "\n"
      "params:\n"
      " idx: index in random number sequence\n"
      " mean: mean of distribution\n"
      " stddev: standard deviation of distribution\n"
      " seed: seed of random number sequence\n",
      node_declaration_visibility::_public,
      {"idx", "mean", "stddev", "seed"},
      {"int"},
      {{0, make_node_argument<Int>(0)},
       {1, make_node_argument<Float>(0.0)},
       {2, make_node_argument<Float>(1.0)},
       {3, make_node_argument<Int>(0)}});
  }

  namespace node::Random::detail {

    struct Uniform : SignalFunction<Uniform, Int, Float, Float, Int, Float>
    {
      auto code() const -> return_type
      {
        auto idx  = *eval_arg<0>();
        auto min  = *eval_arg<1>();
        auto max  = *eval_arg<2>();
        auto seed = *eval_arg<3>();

        idx  = idx < 0 ? 0 : idx;
        seed = seed < 0 ? 0 : seed;

        if (min > max)
          std::swap(min, max);

        auto engine = std::mt19937_64(static_cast<uint64_t>(seed));
        auto dist   = std::uniform_real_distribution(min, max);

        engine.discard(idx);
        return make_object<Float>(dist(engine));
      }
    };

    struct Normal : SignalFunction<Normal, Int, Float, Float, Int, Float>
    {
      auto code() const -> return_type
      {
        auto idx    = *eval_arg<0>();
        auto mean   = *eval_arg<1>();
        auto stddev = *eval_arg<2>();
        auto seed   = *eval_arg<3>();

        idx    = idx < 0 ? 0 : idx;
        seed   = seed < 0 ? 0 : seed;
        stddev = stddev <= 0.0 ? 0.0 : stddev;

        auto engine = std::mt19937_64(static_cast<uint64_t>(seed));
        auto dist   = std::normal_distribution(mean, stddev);

        engine.discard(static_cast<uint64_t>(idx));
        return make_object<Float>(dist(engine));
      }
    };
  } // namespace node::Random::detail

#define RANDOM_NODE_DEFINITION(NAME)                                    \
  auto node_definition_traits<node::Random::NAME, modules::_std::tag>:: \
    get_node_definitions()                                              \
      ->std::vector<node_definition>                                    \
  {                                                                     \
    auto info = get_node_declaration<node::Random::NAME>();             \
    return {node_definition(                                            \
      info.full_name(), 0, make_object<node::Random::detail::NAME>())}; \
  }

  RANDOM_NODE_DEFINITION(Normal);
  RANDOM_NODE_DEFINITION(Uniform);

} // namespace yave