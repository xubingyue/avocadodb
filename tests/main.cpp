#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "Logger/Logger.h"
#include "Logger/LogAppender.h"
#include "Random/RandomGenerator.h"

char const* ARGV0 = "";

int main( int argc, char* argv[] )
{
  ARGV0 = argv[0];
  avocadodb::RandomGenerator::initialize(avocadodb::RandomGenerator::RandomType::MERSENNE);
  // global setup...
  avocadodb::Logger::initialize(false);
  avocadodb::LogAppender::addTtyAppender();
  int result = Catch::Session().run( argc, argv );
  avocadodb::Logger::shutdown();
  // global clean-up...

  return ( result < 0xff ? result : 0xff );
}
