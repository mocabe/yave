//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/program_options.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>

int main(int argc, char** argv)
{
  namespace po   = boost::program_options;
  namespace uuid = boost::uuids;

  po::options_description description("UUID tool options");

  description.add_options()("help,h", "show help")(
    "generate,g", "generate random UUID");

  po::variables_map variables;
  po::store(po::parse_command_line(argc, argv, description), variables);
  po::notify(variables);

  if (variables.empty())
    fmt::print("{}\n", description);

  if (variables.count("help"))
    fmt::print("{}\n", description);

  if (variables.count("generate")) {
    auto gen  = uuid::random_generator_mt19937();
    auto uuid = gen();
    fmt::print("{}\n", uuid);
  }
}