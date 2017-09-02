////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2017 AvocadoDB GmbH, Cologne, Germany
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
/// @author Andreas Streichardt
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_CLUSTER_CLUSTER_HELPERS_H
#define AVOCADOD_CLUSTER_CLUSTER_HELPERS_H 1

#include <velocypack/Slice.h>

using namespace avocadodb::velocypack;

namespace avocadodb {
class ClusterHelpers {
 public:
  static bool compareServerLists(Slice plan, Slice current);
  static bool compareServerLists(std::vector<std::string>, std::vector<std::string>);
};
}

#endif