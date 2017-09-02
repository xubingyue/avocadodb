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

#ifndef AVOCADOD_MMFILES_MMFILES_COLLECTION_WRITE_LOCKER_H
#define AVOCADOD_MMFILES_MMFILES_COLLECTION_WRITE_LOCKER_H 1

#include "Basics/Common.h"
#include "Basics/Exceptions.h"
#include "MMFiles/MMFilesCollection.h"

namespace avocadodb {

class MMFilesCollectionWriteLocker {
 public:
  MMFilesCollectionWriteLocker(MMFilesCollectionWriteLocker const&) = delete;
  MMFilesCollectionWriteLocker& operator=(MMFilesCollectionWriteLocker const&) = delete;

  /// @brief create the locker
  MMFilesCollectionWriteLocker(avocadodb::MMFilesCollection* collection,
                               bool useDeadlockDetector, bool doLock)
      : _collection(collection),
        _useDeadlockDetector(useDeadlockDetector),
        _doLock(false) {
    if (doLock) {
      int res = _collection->lockWrite(_useDeadlockDetector);

      if (res != TRI_ERROR_NO_ERROR) {
        THROW_AVOCADO_EXCEPTION(res);
      }

      _doLock = true;
    }
  }

  /// @brief destroy the locker
  ~MMFilesCollectionWriteLocker() { unlock(); }

  /// @brief release the lock
  inline void unlock() {
    if (_doLock) {
      _collection->unlockWrite(_useDeadlockDetector);
      _doLock = false;
    }
  }

 private:
  /// @brief collection pointer
  avocadodb::MMFilesCollection* _collection;

  /// @brief whether or not to use the deadlock detector
  bool const _useDeadlockDetector;

  /// @brief lock flag
  bool _doLock;
};
}

#endif
