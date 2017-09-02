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
/// @author Michael Hackstein
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_GRAPH_CONSTANT_WEIGHT_SHORTEST_PATH_FINDER_H
#define ARANGODB_GRAPH_CONSTANT_WEIGHT_SHORTEST_PATH_FINDER_H 1

#include "Basics/StringRef.h"
#include "Basics/VelocyPackHelper.h"
#include "Graph/ShortestPathFinder.h"

namespace avocadodb {

class ManagedDocumentResult;
class StringRef;

namespace aql {
class ShortestPathBlock;
}

namespace velocypack {
class Slice;
}

namespace graph {

struct EdgeDocumentToken;
struct ShortestPathOptions;

class ConstantWeightShortestPathFinder : public ShortestPathFinder {
 private:
  struct PathSnippet {
    avocadodb::StringRef const _pred;
    std::unique_ptr<graph::EdgeDocumentToken> _path;

    PathSnippet(avocadodb::StringRef& pred,
                std::unique_ptr<graph::EdgeDocumentToken>&& path);
  };

  typedef std::deque<avocadodb::StringRef> Closure;
  typedef std::unordered_map<avocadodb::StringRef, PathSnippet*> Snippets;

 public:
  explicit ConstantWeightShortestPathFinder(ShortestPathOptions* options);

  ~ConstantWeightShortestPathFinder();

  bool shortestPath(avocadodb::velocypack::Slice const& start,
                    avocadodb::velocypack::Slice const& end,
                    avocadodb::graph::ShortestPathResult& result,
                    std::function<void()> const& callback) override;

 private:
  void expandVertex(bool backward, avocadodb::StringRef vertex);

  void clearVisited();

  bool expandClosure(Closure& sourceClosure, Snippets& sourceSnippets,
                     Snippets& targetSnippets, bool direction,
                     StringRef& result);

  void fillResult(avocadodb::StringRef& n,
                  avocadodb::graph::ShortestPathResult& result);

 private:
  Snippets _leftFound;
  Closure _leftClosure;

  Snippets _rightFound;
  Closure _rightClosure;

  Closure _nextClosure;

  std::vector<avocadodb::StringRef> _neighbors;
  std::vector<std::unique_ptr<graph::EdgeDocumentToken>> _edges;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief The options to modify this shortest path computation
  //////////////////////////////////////////////////////////////////////////////
  avocadodb::graph::ShortestPathOptions* _options;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Reusable ManagedDocumentResult that temporarily takes
  ///        responsibility for one document.
  //////////////////////////////////////////////////////////////////////////////
  std::unique_ptr<ManagedDocumentResult> _mmdr;
};

}  // namespace graph
}  // namespace avocadodb
#endif