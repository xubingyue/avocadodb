////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 AvocadoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is AvocadoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_AQL_QUERY_OPTIONS_H
#define AVOCADOD_AQL_QUERY_OPTIONS_H 1

#include "Basics/Common.h"
#include "Transaction/Options.h"

namespace avocadodb {
namespace velocypack {
class Builder;
class Slice;
}

namespace aql {

struct QueryOptions {
  QueryOptions();

  void fromVelocyPack(avocadodb::velocypack::Slice const&);
  void toVelocyPack(avocadodb::velocypack::Builder&, bool disableOptimizerRules) const;

  size_t memoryLimit;
  size_t maxNumberOfPlans;
  size_t maxWarningCount;
  int64_t literalSizeThreshold;
  int64_t tracing;
  double satelliteSyncWait;
  bool profile;
  bool allPlans;
  bool verbosePlans;
  bool silent;
  bool failOnWarning;
  bool cache;
  bool fullCount;
  bool count;
  bool verboseErrors;
  bool inspectSimplePlans;
  std::vector<std::string> optimizerRules;
  std::unordered_set<std::string> shardIds;

  transaction::Options transactionOptions;
};

}
}

#endif
