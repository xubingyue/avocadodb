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

#ifndef AVOCADOD_INDEXES_INDEX_FACTORY_H
#define AVOCADOD_INDEXES_INDEX_FACTORY_H 1

#include "Basics/Common.h"

namespace avocadodb {

class Index;
class LogicalCollection;

namespace velocypack {
class Builder;
class Slice;
}

class IndexFactory {
 public:
  IndexFactory() = default;
  IndexFactory(IndexFactory const&) = delete;
  IndexFactory& operator=(IndexFactory const&) = delete;

  virtual ~IndexFactory() = default;

  virtual int enhanceIndexDefinition(
      avocadodb::velocypack::Slice const definition,
      avocadodb::velocypack::Builder& enhanced, bool isCreation,
      bool isCoordinator) const = 0;

  virtual std::shared_ptr<avocadodb::Index> prepareIndexFromSlice(
      avocadodb::velocypack::Slice info, bool generateKey,
      avocadodb::LogicalCollection* col, bool isClusterConstructor) const = 0;

  virtual void fillSystemIndexes(
      avocadodb::LogicalCollection* col,
      std::vector<std::shared_ptr<avocadodb::Index>>& systemIndexes) const = 0;

  virtual std::vector<std::string> supportedIndexes() const = 0;
};

}  // namespace avocadodb

#endif
