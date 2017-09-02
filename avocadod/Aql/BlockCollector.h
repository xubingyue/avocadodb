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

#ifndef AVOCADOD_AQL_BLOCK_COLLECTOR_H
#define AVOCADOD_AQL_BLOCK_COLLECTOR_H 1

#include "Basics/Common.h"
#include "Basics/SmallVector.h"
#include "Aql/types.h"

namespace avocadodb {
namespace aql {
class AqlItemBlock;
class AqlItemBlockManager;

class BlockCollector {
  friend class AqlItemBlock;

 public:
  BlockCollector(BlockCollector const&) = delete;
  BlockCollector& operator=(BlockCollector const&) = delete;

  explicit BlockCollector(AqlItemBlockManager*);
  ~BlockCollector();

  size_t totalSize() const;
  RegisterId nrRegs() const;

  void clear();
  
  void add(std::unique_ptr<AqlItemBlock> block);
  void add(AqlItemBlock* block);

  AqlItemBlock* steal();

 private:
  AqlItemBlockManager* _blockManager;
  SmallVector<AqlItemBlock*>::allocator_type::arena_type _arena;
  SmallVector<AqlItemBlock*> _blocks;
  size_t _totalSize;
};

}  // namespace avocadodb::aql
}  // namespace avocadodb

#endif
