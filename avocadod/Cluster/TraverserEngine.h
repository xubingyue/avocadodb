////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
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
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Michael Hackstein
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_CLUSTER_TRAVERSER_ENGINE_H
#define AVOCADOD_CLUSTER_TRAVERSER_ENGINE_H 1

#include "Aql/Collections.h"
#include "Basics/Common.h"

struct TRI_vocbase_t;

namespace avocadodb {

namespace transaction {
class Methods;
}

namespace transaction {
class Context;
}

namespace aql {
class Collections;
class Query;
}

namespace graph {
struct ShortestPathOptions;
}

namespace velocypack {
class Builder;
class Slice;
}

namespace traverser {
struct TraverserOptions;

class BaseEngine {
  friend class TraverserEngineRegistry;

 public:
  enum EngineType { TRAVERSER, SHORTESTPATH };

 protected:
  // These are private on purpose.
  // Only the Registry (friend) is allowed
  // to create and destroy engines.
  // We can get into undefined state if sth.
  // deletes an engine but the registry
  // does not get informed properly

  static std::unique_ptr<BaseEngine> BuildEngine(TRI_vocbase_t* vocbase,
                                                 avocadodb::velocypack::Slice);

  BaseEngine(TRI_vocbase_t*, avocadodb::velocypack::Slice);

 public:
  virtual ~BaseEngine();

  // The engine is NOT copyable.
  BaseEngine(BaseEngine const&) = delete;

  void getVertexData(avocadodb::velocypack::Slice,
                     avocadodb::velocypack::Builder&);

  bool lockCollection(std::string const&);

  std::shared_ptr<transaction::Context> context() const;

  virtual EngineType getType() const = 0;

 protected:
  avocadodb::aql::Query* _query;
  transaction::Methods* _trx;
  avocadodb::aql::Collections _collections;
  std::unordered_map<std::string, std::vector<std::string>> _vertexShards;
};

class BaseTraverserEngine : public BaseEngine {
 public:
  // Only the Registry (friend) is allowed
  // to create and destroy engines.
  // We can get into undefined state if sth.
  // deletes an engine but the registry
  // does not get informed properly

  BaseTraverserEngine(TRI_vocbase_t*, avocadodb::velocypack::Slice);

  virtual ~BaseTraverserEngine();

  void getEdges(avocadodb::velocypack::Slice, size_t,
                avocadodb::velocypack::Builder&);

  void getVertexData(avocadodb::velocypack::Slice, size_t,
                     avocadodb::velocypack::Builder&);

  virtual void smartSearch(avocadodb::velocypack::Slice,
                           avocadodb::velocypack::Builder&) = 0;

  virtual void smartSearchBFS(avocadodb::velocypack::Slice,
                              avocadodb::velocypack::Builder&) = 0;

  EngineType getType() const override { return TRAVERSER; }

 protected:
  std::unique_ptr<traverser::TraverserOptions> _opts;
};


class ShortestPathEngine : public BaseEngine {
 public:
  // Only the Registry (friend) is allowed
  // to create and destroy engines.
  // We can get into undefined state if sth.
  // deletes an engine but the registry
  // does not get informed properly

  ShortestPathEngine(TRI_vocbase_t*, avocadodb::velocypack::Slice);

  virtual ~ShortestPathEngine();

  void getEdges(avocadodb::velocypack::Slice,
                bool backward,
                avocadodb::velocypack::Builder&);

  EngineType getType() const override { return SHORTESTPATH; }

 protected:
  std::unique_ptr<graph::ShortestPathOptions> _opts;
};

class TraverserEngine : public BaseTraverserEngine {
 public:
  // Only the Registry (friend) is allowed
  // to create and destroy engines.
  // We can get into undefined state if sth.
  // deletes an engine but the registry
  // does not get informed properly

  TraverserEngine(TRI_vocbase_t*, avocadodb::velocypack::Slice);

  ~TraverserEngine();

  void smartSearch(avocadodb::velocypack::Slice,
                   avocadodb::velocypack::Builder&) override;

  void smartSearchBFS(avocadodb::velocypack::Slice,
                      avocadodb::velocypack::Builder&) override;
};

}  // namespace traverser
}  // namespace avocadodb

#endif
