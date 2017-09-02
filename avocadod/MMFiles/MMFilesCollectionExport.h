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

#ifndef AVOCADOD_MMFILES_MMFILES_COLLECTION_EXPORT_H
#define AVOCADOD_MMFILES_MMFILES_COLLECTION_EXPORT_H 1

#include "Basics/Common.h"
#include "Utils/CollectionExport.h"
#include "Utils/CollectionNameResolver.h"
#include "VocBase/voc-types.h"

struct TRI_vocbase_t;

namespace avocadodb {

class CollectionGuard;
class MMFilesDocumentDitch;

class MMFilesCollectionExport {
  friend class MMFilesExportCursor;

 public:
  MMFilesCollectionExport(MMFilesCollectionExport const&) = delete;
  MMFilesCollectionExport& operator=(MMFilesCollectionExport const&) = delete;

  MMFilesCollectionExport(TRI_vocbase_t*, std::string const&, CollectionExport::Restrictions const&);

  ~MMFilesCollectionExport();

 public:
  void run(uint64_t, size_t);

 private:
  std::unique_ptr<avocadodb::CollectionGuard> _guard;
  LogicalCollection* _collection;
  avocadodb::MMFilesDocumentDitch* _ditch;
  std::string const _name;
  avocadodb::CollectionNameResolver _resolver;
  CollectionExport::Restrictions _restrictions;
  std::vector<uint8_t const*> _vpack;
};
}

#endif
