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

#ifndef AVOCADOD_MMFILES_MMFILES_COLLECTION_KEYS_H
#define AVOCADOD_MMFILES_MMFILES_COLLECTION_KEYS_H 1

#include "Basics/Common.h"
#include "Utils/CollectionKeys.h"
#include "Utils/CollectionNameResolver.h"
#include "VocBase/voc-types.h"

#include <velocypack/Builder.h>
#include <velocypack/Slice.h>
#include <velocypack/velocypack-aliases.h>

struct TRI_vocbase_t;

namespace avocadodb {
class CollectionGuard;
class MMFilesDocumentDitch;

typedef TRI_voc_tick_t CollectionKeysId;

class MMFilesCollectionKeys final : public CollectionKeys {
 public:
  MMFilesCollectionKeys(MMFilesCollectionKeys const&) = delete;
  MMFilesCollectionKeys& operator=(MMFilesCollectionKeys const&) = delete;

  MMFilesCollectionKeys(TRI_vocbase_t*, std::string const& name, 
                        TRI_voc_tick_t blockerId, double ttl);

  ~MMFilesCollectionKeys();

 public:
  size_t count() const override {
    return _vpack.size();
  }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief initially creates the list of keys
  //////////////////////////////////////////////////////////////////////////////

  void create(TRI_voc_tick_t) override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief hashes a chunk of keys
  //////////////////////////////////////////////////////////////////////////////

  std::tuple<std::string, std::string, uint64_t> hashChunk(size_t,
                                                           size_t) const override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief dumps keys into the result
  //////////////////////////////////////////////////////////////////////////////

  void dumpKeys(avocadodb::velocypack::Builder&, size_t, size_t) const override;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief dumps documents into the result
  //////////////////////////////////////////////////////////////////////////////

  void dumpDocs(avocadodb::velocypack::Builder&, size_t, size_t,
                avocadodb::velocypack::Slice const&) const override;

 private:
  std::unique_ptr<avocadodb::CollectionGuard> _guard;
  avocadodb::MMFilesDocumentDitch* _ditch;
  avocadodb::CollectionNameResolver _resolver;
  TRI_voc_tick_t _blockerId;
  std::vector<uint8_t const*> _vpack;
};
}

#endif
