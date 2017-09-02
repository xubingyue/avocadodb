////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2017-2017 AvocadoDB GmbH, Cologne, Germany
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
/// @author Michael Hackstein
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_GRAPH_TRAVERSER_CACHE_FACTORY_H
#define AVOCADOD_GRAPH_TRAVERSER_CACHE_FACTORY_H 1

#include "Basics/Common.h"
#include "Cluster/ClusterInfo.h"
#include "Cluster/TraverserEngineRegistry.h"

namespace avocadodb {
namespace transaction {
class Methods;
}
namespace graph {
class TraverserCache;

namespace cacheFactory {
TraverserCache* CreateCache(
    avocadodb::transaction::Methods* trx, bool activateDocumentCache,
    std::unordered_map<ServerID, traverser::TraverserEngineID> const* engines);

}  // namespace cacheFactory
}  // namespace traverser
}  // namespace avocadodb
#endif
