////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2017 AvocadoDB GmbH, Cologne, Germany
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
/// @author Simon Grätzer
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_VOC_BASE_API_INDEXES_H
#define AVOCADOD_VOC_BASE_API_INDEXES_H 1

#include <velocypack/Builder.h>
#include <velocypack/Slice.h>
#include "Basics/Result.h"
#include "VocBase/voc-types.h"

struct TRI_vocbase_t;

namespace avocadodb {
class LogicalCollection;
class CollectionNameResolver;
namespace methods {

/// Common code for ensureIndexes and api-index.js
struct Indexes {
  static avocadodb::Result getIndex(
      avocadodb::LogicalCollection const* collection,
      avocadodb::velocypack::Slice const& indexId,
      avocadodb::velocypack::Builder&);

  static avocadodb::Result getAll(avocadodb::LogicalCollection const* collection,
                                 bool withFigures,
                                 avocadodb::velocypack::Builder&);

  static avocadodb::Result ensureIndex(
      avocadodb::LogicalCollection* collection,
      avocadodb::velocypack::Slice const& definition, bool create,
      avocadodb::velocypack::Builder& output);

  static avocadodb::Result drop(avocadodb::LogicalCollection const* collection,
                               avocadodb::velocypack::Slice const& indexArg);

  static avocadodb::Result extractHandle(
      avocadodb::LogicalCollection const* collection,
      avocadodb::CollectionNameResolver const* resolver,
      avocadodb::velocypack::Slice const& val, TRI_idx_iid_t& iid);

 private:
  static avocadodb::Result ensureIndexCoordinator(
      avocadodb::LogicalCollection const* collection,
      avocadodb::velocypack::Slice const& indexDef, bool create,
      avocadodb::velocypack::Builder& resultBuilder);

#ifdef USE_ENTERPRISE
  static avocadodb::Result ensureIndexCoordinatorEE(
      avocadodb::LogicalCollection const* collection,
      avocadodb::velocypack::Slice const slice, bool create,
      avocadodb::velocypack::Builder& resultBuilder);
  static avocadodb::Result dropCoordinatorEE(
      avocadodb::LogicalCollection const* collection, TRI_idx_iid_t const iid);
#endif
};
}
}

#endif
