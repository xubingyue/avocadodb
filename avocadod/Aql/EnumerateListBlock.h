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
/// @author Max Neunhoeffer
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_AQL_ENUMERATE_LIST_BLOCK_H
#define AVOCADOD_AQL_ENUMERATE_LIST_BLOCK_H 1

#include "ExecutionBlock.h"
#include "Aql/ExecutionNode.h"

namespace avocadodb {
namespace aql {

class AqlItemBlock;

class ExecutionEngine;

class EnumerateListBlock : public ExecutionBlock {
 public:
  EnumerateListBlock(ExecutionEngine*, EnumerateListNode const*);
  ~EnumerateListBlock();

  // here we release our docs from this collection
  int initializeCursor(AqlItemBlock* items, size_t pos) override;

  AqlItemBlock* getSome(size_t atLeast, size_t atMost) override final;

  // skip between atLeast and atMost returns the number actually skipped . . .
  // will only return less than atLeast if there aren't atLeast many
  // things to skip overall.
  size_t skipSome(size_t atLeast, size_t atMost) override final;

 private:
  // cppcheck-suppress *
  AqlValue getAqlValue(AqlValue const&, bool& mustDestroy);

  // cppcheck-suppress *
  void throwArrayExpectedException(AqlValue const& value);

 private:
  // current position in the _inVariable
  size_t _index;

  // current block in DOCVEC
  size_t _thisBlock;
  
  // number of elements in DOCVEC before the current block
  size_t _seen;

  // total number of elements in DOCVEC
  size_t _docVecSize;

  // the register index containing the inVariable of the
  // EnumerateListNode
  RegisterId _inVarRegId;
};

}
}

#endif
