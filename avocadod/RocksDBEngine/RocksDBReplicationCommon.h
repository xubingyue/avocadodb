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
/// @author Daniel H. Larkin
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADO_ROCKSDB_ROCKSDB_REPLICATION_COMMON_H
#define AVOCADO_ROCKSDB_ROCKSDB_REPLICATION_COMMON_H 1

#include "Basics/Common.h"
#include "Basics/Result.h"

namespace avocadodb {

class RocksDBReplicationResult : public Result {
 public:
  RocksDBReplicationResult(int, uint64_t);
  uint64_t maxTick() const;
  bool fromTickIncluded() const;

  void includeFromTick();

 private:
  uint64_t _maxTick;
  bool _fromTickIncluded;
};

}  // namespace avocadodb

#endif
