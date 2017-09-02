
#ifndef AVOCADODB_TEST_ICU_HELPER
#define AVOCADODB_TEST_ICU_HELPER 1

#include "Basics/Common.h"

struct IcuInitializer {
  IcuInitializer();
  ~IcuInitializer();

  static void setup(char const* path);

  static void* icuDataPtr;
  static bool initialized;
};

#endif
