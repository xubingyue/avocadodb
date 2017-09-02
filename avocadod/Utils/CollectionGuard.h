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

#ifndef AVOCADOD_UTILS_COLLECTION_GUARD_H
#define AVOCADOD_UTILS_COLLECTION_GUARD_H 1

#include "Basics/Common.h"
#include "Basics/Exceptions.h"
#include "Basics/StringUtils.h"
#include "VocBase/vocbase.h"

namespace avocadodb {

class CollectionGuard {
 public:
  CollectionGuard(CollectionGuard const&) = delete;
  CollectionGuard& operator=(CollectionGuard const&) = delete;

  /// @brief create the guard, using a collection id
  CollectionGuard(TRI_vocbase_t* vocbase, TRI_voc_cid_t id,
                  bool restoreOriginalStatus = false)
      : _vocbase(vocbase),
        _collection(nullptr),
        _originalStatus(TRI_VOC_COL_STATUS_CORRUPTED),
        _restoreOriginalStatus(restoreOriginalStatus) {
    _collection = _vocbase->useCollection(id, _originalStatus);

    if (_collection == nullptr) {
      THROW_AVOCADO_EXCEPTION(TRI_ERROR_AVOCADO_COLLECTION_NOT_FOUND);
    }
  }

  CollectionGuard(TRI_vocbase_t* vocbase, TRI_voc_cid_t id, std::string const& name)
      : _vocbase(vocbase),
        _collection(nullptr),
        _originalStatus(TRI_VOC_COL_STATUS_CORRUPTED),
        _restoreOriginalStatus(false) {
    _collection = _vocbase->useCollection(id, _originalStatus);

    if (_collection == nullptr && !name.empty()) {
      _collection = _vocbase->useCollection(name, _originalStatus);
    }

    if (_collection == nullptr) {
      THROW_AVOCADO_EXCEPTION(TRI_ERROR_AVOCADO_COLLECTION_NOT_FOUND);
    }
  }

  /// @brief create the guard, using a collection name
  CollectionGuard(TRI_vocbase_t* vocbase, std::string const& name,
                  bool restoreOriginalStatus = false)
      : _vocbase(vocbase),
        _collection(nullptr),
        _originalStatus(TRI_VOC_COL_STATUS_CORRUPTED),
        _restoreOriginalStatus(restoreOriginalStatus) {
    if (!name.empty() && name[0] >= '0' && name[0] <= '9') {
      TRI_voc_cid_t id = avocadodb::basics::StringUtils::uint64(name);
      _collection = _vocbase->useCollection(id, _originalStatus);
    } else {
      _collection = _vocbase->useCollection(name, _originalStatus);
    }

    if (_collection == nullptr) {
      THROW_AVOCADO_EXCEPTION(TRI_ERROR_AVOCADO_COLLECTION_NOT_FOUND);
    }
  }

  /// @brief destroy the guard
  ~CollectionGuard() {
    if (_collection != nullptr) {
      _vocbase->releaseCollection(_collection);

      if (_restoreOriginalStatus &&
          (_originalStatus == TRI_VOC_COL_STATUS_UNLOADING ||
           _originalStatus == TRI_VOC_COL_STATUS_UNLOADED)) {
        // re-unload the collection
        _vocbase->unloadCollection(_collection, false);
      }
    }
  }

 public:
  /// @brief return the collection pointer
  inline avocadodb::LogicalCollection* collection() const { return _collection; }

  /// @brief return the status of the collection at the time of using the guard
  inline TRI_vocbase_col_status_e originalStatus() const {
    return _originalStatus;
  }

 private:
  /// @brief pointer to vocbase
  TRI_vocbase_t* _vocbase;

  /// @brief pointer to collection
  avocadodb::LogicalCollection* _collection;

  /// @brief status of collection when invoking the guard
  TRI_vocbase_col_status_e _originalStatus;

  /// @brief whether or not to restore the original collection status
  bool _restoreOriginalStatus;
};
}

#endif
