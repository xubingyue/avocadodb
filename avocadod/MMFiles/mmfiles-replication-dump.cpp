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
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#include "mmfiles-replication-dump.h"
#include "Basics/ReadLocker.h"
#include "Basics/StaticStrings.h"
#include "Basics/StringRef.h"
#include "Basics/VPackStringBufferAdapter.h"
#include "Logger/Logger.h"
#include "MMFiles/MMFilesLogfileManager.h" 
#include "MMFiles/MMFilesCompactionLocker.h"
#include "MMFiles/MMFilesDitch.h"
#include "VocBase/LogicalCollection.h"
#include "VocBase/vocbase.h"

#include <velocypack/Dumper.h>
#include <velocypack/Options.h>
#include <velocypack/Slice.h>
#include <velocypack/velocypack-aliases.h>

using namespace avocadodb;

/// @brief append values to a string buffer
static void Append(MMFilesReplicationDumpContext* dump, uint64_t value) {
  int res = TRI_AppendUInt64StringBuffer(dump->_buffer, value);

  if (res != TRI_ERROR_NO_ERROR) {
    THROW_AVOCADO_EXCEPTION(res);
  }
}

static void Append(MMFilesReplicationDumpContext* dump, char const* value) {
  int res = TRI_AppendStringStringBuffer(dump->_buffer, value);

  if (res != TRI_ERROR_NO_ERROR) {
    THROW_AVOCADO_EXCEPTION(res);
  }
}

/// @brief translate a (local) collection id into a collection name
static char const* NameFromCid(MMFilesReplicationDumpContext* dump,
                               TRI_voc_cid_t cid) {
  auto it = dump->_collectionNames.find(cid);

  if (it != dump->_collectionNames.end()) {
    // collection name is in cache already
    return (*it).second.c_str();
  }

  // collection name not in cache yet
  std::string name(dump->_vocbase->collectionName(cid));

  if (!name.empty()) {
    // insert into cache
    try {
      dump->_collectionNames.emplace(cid, std::move(name));
    } catch (...) {
      return nullptr;
    }

    // and look it up again
    return NameFromCid(dump, cid);
  }

  return nullptr;
}

/// @brief whether or not a marker should be replicated
static inline bool MustReplicateWalMarkerType(MMFilesMarker const* marker) {
  MMFilesMarkerType type = marker->getType();
  return (type == TRI_DF_MARKER_VPACK_DOCUMENT ||
          type == TRI_DF_MARKER_VPACK_REMOVE ||
          type == TRI_DF_MARKER_VPACK_BEGIN_TRANSACTION ||
          type == TRI_DF_MARKER_VPACK_COMMIT_TRANSACTION ||
          type == TRI_DF_MARKER_VPACK_ABORT_TRANSACTION ||
          type == TRI_DF_MARKER_VPACK_CREATE_COLLECTION ||
          type == TRI_DF_MARKER_VPACK_DROP_COLLECTION ||
          type == TRI_DF_MARKER_VPACK_RENAME_COLLECTION ||
          type == TRI_DF_MARKER_VPACK_CHANGE_COLLECTION ||
          type == TRI_DF_MARKER_VPACK_CREATE_INDEX ||
          type == TRI_DF_MARKER_VPACK_DROP_INDEX ||
          type == TRI_DF_MARKER_VPACK_CREATE_VIEW ||
          type == TRI_DF_MARKER_VPACK_DROP_VIEW ||
          type == TRI_DF_MARKER_VPACK_CHANGE_VIEW);
}

/// @brief whether or not a marker belongs to a transaction
static inline bool IsTransactionWalMarkerType(MMFilesMarker const* marker) {
  MMFilesMarkerType type = marker->getType();
  return (type == TRI_DF_MARKER_VPACK_BEGIN_TRANSACTION ||
          type == TRI_DF_MARKER_VPACK_COMMIT_TRANSACTION ||
          type == TRI_DF_MARKER_VPACK_ABORT_TRANSACTION);
}

/// @brief translate a marker type to a replication type
static TRI_replication_operation_e TranslateType(
    MMFilesMarker const* marker) {
  switch (marker->getType()) {
    case TRI_DF_MARKER_VPACK_DOCUMENT:
      return REPLICATION_MARKER_DOCUMENT;
    case TRI_DF_MARKER_VPACK_REMOVE:
      return REPLICATION_MARKER_REMOVE;
    case TRI_DF_MARKER_VPACK_BEGIN_TRANSACTION:
      return REPLICATION_TRANSACTION_START;
    case TRI_DF_MARKER_VPACK_COMMIT_TRANSACTION:
      return REPLICATION_TRANSACTION_COMMIT;
    case TRI_DF_MARKER_VPACK_ABORT_TRANSACTION:
      return REPLICATION_TRANSACTION_ABORT;
    case TRI_DF_MARKER_VPACK_CREATE_COLLECTION:
      return REPLICATION_COLLECTION_CREATE;
    case TRI_DF_MARKER_VPACK_DROP_COLLECTION:
      return REPLICATION_COLLECTION_DROP;
    case TRI_DF_MARKER_VPACK_RENAME_COLLECTION:
      return REPLICATION_COLLECTION_RENAME;
    case TRI_DF_MARKER_VPACK_CHANGE_COLLECTION:
      return REPLICATION_COLLECTION_CHANGE;
    case TRI_DF_MARKER_VPACK_CREATE_INDEX:
      return REPLICATION_INDEX_CREATE;
    case TRI_DF_MARKER_VPACK_DROP_INDEX:
      return REPLICATION_INDEX_DROP;
    case TRI_DF_MARKER_VPACK_CREATE_VIEW:
      return REPLICATION_VIEW_CREATE;
    case TRI_DF_MARKER_VPACK_DROP_VIEW:
      return REPLICATION_VIEW_DROP;
    case TRI_DF_MARKER_VPACK_CHANGE_VIEW:
      return REPLICATION_VIEW_CHANGE;

    default:
      return REPLICATION_INVALID;
  }
}

/// @brief stringify a raw marker from a logfile for a log dump or logger
/// follow command
static int StringifyMarker(MMFilesReplicationDumpContext* dump,
                           TRI_voc_tick_t databaseId,
                           TRI_voc_cid_t collectionId,
                           MMFilesMarker const* marker, bool isDump,
                           bool withTicks, bool isEdgeCollection) {
  TRI_ASSERT(MustReplicateWalMarkerType(marker));
  MMFilesMarkerType const type = marker->getType();

  if (!isDump) {
    // logger-follow command
    Append(dump, "{\"tick\":\"");
    Append(dump, static_cast<uint64_t>(marker->getTick()));
    // for debugging use the following
    // Append(dump, "\",\"typeName\":\"");
    // Append(dump, TRI_NameMarkerDatafile(marker));

    if (dump->_compat28 && (type == TRI_DF_MARKER_VPACK_DOCUMENT ||
                            type == TRI_DF_MARKER_VPACK_REMOVE)) {
      // 2.8-compatible format
      VPackSlice slice(reinterpret_cast<char const*>(marker) +
                       MMFilesDatafileHelper::VPackOffset(type));
      avocadodb::basics::VPackStringBufferAdapter adapter(dump->_buffer);
      VPackDumper dumper(
          &adapter,
          &dump->_vpackOptions);  // note: we need the CustomTypeHandler here

      // additionally dump "key" and "rev" attributes on the top-level
      Append(dump, "\",\"key\":");
      dumper.dump(slice.get(StaticStrings::KeyString));
      if (slice.hasKey(StaticStrings::RevString)) {
        Append(dump, ",\"rev\":");
        dumper.dump(slice.get(StaticStrings::RevString));
      }
      // convert 2300 markers to 2301 markers for edges
      Append(dump, ",\"type\":");
      if (type == TRI_DF_MARKER_VPACK_DOCUMENT && isEdgeCollection) {
        Append(dump, 2301);
      } else {
        Append(dump, static_cast<uint64_t>(TranslateType(marker)));
      }
    } else {
      // 3.0 dump
      Append(dump, "\",\"type\":");
      Append(dump, static_cast<uint64_t>(TranslateType(marker)));
    }

    if (type == TRI_DF_MARKER_VPACK_DOCUMENT ||
        type == TRI_DF_MARKER_VPACK_REMOVE ||
        type == TRI_DF_MARKER_VPACK_BEGIN_TRANSACTION ||
        type == TRI_DF_MARKER_VPACK_COMMIT_TRANSACTION ||
        type == TRI_DF_MARKER_VPACK_ABORT_TRANSACTION) {
      // transaction id
      Append(dump, ",\"tid\":\"");
      Append(dump, MMFilesDatafileHelper::TransactionId(marker));
      Append(dump, "\"");
    }

    if (databaseId > 0) {
      Append(dump, ",\"database\":\"");
      Append(dump, databaseId);
      Append(dump, "\"");

      if (collectionId > 0) {
        Append(dump, ",\"cid\":\"");
        Append(dump, collectionId);
        Append(dump, "\"");
        // also include collection name
        char const* cname = NameFromCid(dump, collectionId);

        if (cname != nullptr) {
          Append(dump, ",\"cname\":\"");
          Append(dump, cname);
          Append(dump, "\"");
        }
      }
    }
  } else {
    // collection dump
    if (withTicks) {
      Append(dump, "{\"tick\":\"");
      Append(dump, static_cast<uint64_t>(marker->getTick()));
      Append(dump, "\",");
    } else {
      Append(dump, "{");
    }

    if (dump->_compat28 && (type == TRI_DF_MARKER_VPACK_DOCUMENT ||
                            type == TRI_DF_MARKER_VPACK_REMOVE)) {
      // 2.8-compatible format
      VPackSlice slice(reinterpret_cast<char const*>(marker) +
                       MMFilesDatafileHelper::VPackOffset(type));
      avocadodb::basics::VPackStringBufferAdapter adapter(dump->_buffer);
      VPackDumper dumper(
          &adapter,
          &dump->_vpackOptions);  // note: we need the CustomTypeHandler here

      // additionally dump "key" and "rev" attributes on the top-level
      Append(dump, "\"key\":");
      dumper.dump(slice.get(StaticStrings::KeyString));
      if (slice.hasKey(StaticStrings::RevString)) {
        Append(dump, ",\"rev\":");
        dumper.dump(slice.get(StaticStrings::RevString));
      }
      // convert 2300 markers to 2301 markers for edges
      Append(dump, ",\"type\":");
      if (type == TRI_DF_MARKER_VPACK_DOCUMENT && isEdgeCollection) {
        Append(dump, 2301);
      } else {
        Append(dump, static_cast<uint64_t>(TranslateType(marker)));
      }
    } else {
      Append(dump, "\"type\":");
      Append(dump, static_cast<uint64_t>(TranslateType(marker)));
    }
  }

  switch (type) {
    case TRI_DF_MARKER_VPACK_DOCUMENT:
    case TRI_DF_MARKER_VPACK_REMOVE:
    case TRI_DF_MARKER_VPACK_CREATE_DATABASE:
    case TRI_DF_MARKER_VPACK_CREATE_COLLECTION:
    case TRI_DF_MARKER_VPACK_CREATE_INDEX:
    case TRI_DF_MARKER_VPACK_CREATE_VIEW:
    case TRI_DF_MARKER_VPACK_RENAME_COLLECTION:
    case TRI_DF_MARKER_VPACK_CHANGE_COLLECTION:
    case TRI_DF_MARKER_VPACK_CHANGE_VIEW:
    case TRI_DF_MARKER_VPACK_DROP_DATABASE:
    case TRI_DF_MARKER_VPACK_DROP_COLLECTION:
    case TRI_DF_MARKER_VPACK_DROP_INDEX: 
    case TRI_DF_MARKER_VPACK_DROP_VIEW: {
      Append(dump, ",\"data\":");

      VPackSlice slice(reinterpret_cast<char const*>(marker) +
                       MMFilesDatafileHelper::VPackOffset(type));
      avocadodb::basics::VPackStringBufferAdapter adapter(dump->_buffer);
      VPackDumper dumper(
          &adapter,
          &dump->_vpackOptions);  // note: we need the CustomTypeHandler here
      dumper.dump(slice);
      break;
    }

    case TRI_DF_MARKER_VPACK_BEGIN_TRANSACTION:
    case TRI_DF_MARKER_VPACK_COMMIT_TRANSACTION:
    case TRI_DF_MARKER_VPACK_ABORT_TRANSACTION: {
      // nothing to do
      break;
    }

    default: {
      TRI_ASSERT(false);
      LOG_TOPIC(ERR, avocadodb::Logger::FIXME) << "got invalid marker of type " << static_cast<int>(type); 
      return TRI_ERROR_INTERNAL;
    }
  }

  Append(dump, "}\n");
  return TRI_ERROR_NO_ERROR;
}

static int SliceifyMarker(MMFilesReplicationDumpContext* dump,
                          TRI_voc_tick_t databaseId, TRI_voc_cid_t collectionId,
                          MMFilesMarker const* marker, bool isDump,
                          bool withTicks, bool isEdgeCollection) {
  TRI_ASSERT(MustReplicateWalMarkerType(marker));
  MMFilesMarkerType const type = marker->getType();

  VPackBuffer<uint8_t> buffer;
  std::shared_ptr<VPackBuffer<uint8_t>> bufferPtr;
  bufferPtr.reset(&buffer, avocadodb::velocypack::BufferNonDeleter<uint8_t>());

  VPackBuilder builder(bufferPtr, &dump->_vpackOptions);
  builder.openObject();

  if (!isDump) {
    // logger-follow command
    builder.add("tick", VPackValue(static_cast<uint64_t>(marker->getTick())));

    if (dump->_compat28 && (type == TRI_DF_MARKER_VPACK_DOCUMENT ||
                            type == TRI_DF_MARKER_VPACK_REMOVE)) {
      // 2.8-compatible format
      VPackSlice slice(reinterpret_cast<char const*>(marker) +
                       MMFilesDatafileHelper::VPackOffset(type));
      // additionally dump "key" and "rev" attributes on the top-level
      builder.add("key", slice.get(StaticStrings::KeyString));
      if (slice.hasKey(StaticStrings::RevString)) {
        builder.add("rev", slice.get(StaticStrings::RevString));
      }
      // convert 2300 markers to 2301 markers for edges
      if (type == TRI_DF_MARKER_VPACK_DOCUMENT && isEdgeCollection) {
        builder.add("type", VPackValue(2301));
      } else {
        builder.add("type",
                    VPackValue(static_cast<uint64_t>(TranslateType(marker))));
      }
    } else {
      // 3.0 dump
      builder.add("type",
                  VPackValue(static_cast<uint64_t>(TranslateType(marker))));
    }

    if (type == TRI_DF_MARKER_VPACK_DOCUMENT ||
        type == TRI_DF_MARKER_VPACK_REMOVE ||
        type == TRI_DF_MARKER_VPACK_BEGIN_TRANSACTION ||
        type == TRI_DF_MARKER_VPACK_COMMIT_TRANSACTION ||
        type == TRI_DF_MARKER_VPACK_ABORT_TRANSACTION) {
      // transaction id
      builder.add("tid", VPackValue(MMFilesDatafileHelper::TransactionId(marker)));
    }
    if (databaseId > 0) {
      builder.add("database", VPackValue(databaseId));
      if (collectionId > 0) {
        builder.add("cid", VPackValue(collectionId));
        // also include collection name
        char const* cname = NameFromCid(dump, collectionId);
        if (cname != nullptr) {
          builder.add("cname", VPackValue(cname));
        }
      }
    }
  } else {
    // collection dump
    if (withTicks) {
      builder.add("tick", VPackValue(static_cast<uint64_t>(marker->getTick())));
    }

    if (dump->_compat28 && (type == TRI_DF_MARKER_VPACK_DOCUMENT ||
                            type == TRI_DF_MARKER_VPACK_REMOVE)) {
      // 2.8-compatible format
      VPackSlice slice(reinterpret_cast<char const*>(marker) +
                       MMFilesDatafileHelper::VPackOffset(type));
      builder.add("key", slice.get(StaticStrings::KeyString));
      if (slice.hasKey(StaticStrings::RevString)) {
        builder.add("rev", slice.get(StaticStrings::RevString));
      }
      // convert 2300 markers to 2301 markers for edges
      if (type == TRI_DF_MARKER_VPACK_DOCUMENT && isEdgeCollection) {
        builder.add("type", VPackValue(2301));
      } else {
        builder.add("type",
                    VPackValue(static_cast<uint64_t>(TranslateType(marker))));
      }
    } else {
      builder.add("type",
                  VPackValue(static_cast<uint64_t>(TranslateType(marker))));
    }
  }

  switch (type) {
    case TRI_DF_MARKER_VPACK_DOCUMENT:
    case TRI_DF_MARKER_VPACK_REMOVE:
    case TRI_DF_MARKER_VPACK_CREATE_DATABASE:
    case TRI_DF_MARKER_VPACK_CREATE_COLLECTION:
    case TRI_DF_MARKER_VPACK_CREATE_INDEX:
    case TRI_DF_MARKER_VPACK_CREATE_VIEW:
    case TRI_DF_MARKER_VPACK_RENAME_COLLECTION:
    case TRI_DF_MARKER_VPACK_CHANGE_COLLECTION:
    case TRI_DF_MARKER_VPACK_CHANGE_VIEW:
    case TRI_DF_MARKER_VPACK_DROP_DATABASE:
    case TRI_DF_MARKER_VPACK_DROP_COLLECTION:
    case TRI_DF_MARKER_VPACK_DROP_INDEX: 
    case TRI_DF_MARKER_VPACK_DROP_VIEW: { 
      VPackSlice slice(reinterpret_cast<char const*>(marker) +
                       MMFilesDatafileHelper::VPackOffset(type));
      builder.add("data", slice);
      break;
    }

    case TRI_DF_MARKER_VPACK_BEGIN_TRANSACTION:
    case TRI_DF_MARKER_VPACK_COMMIT_TRANSACTION:
    case TRI_DF_MARKER_VPACK_ABORT_TRANSACTION: {
      // nothing to do
      break;
    }

    default: {
      TRI_ASSERT(false);
      LOG_TOPIC(ERR, avocadodb::Logger::FIXME) << "got invalid marker of type " << static_cast<int>(type); 
      return TRI_ERROR_INTERNAL;
    }
  }

  builder.close();

  dump->_slices.push_back(std::move(buffer));

  return TRI_ERROR_NO_ERROR;
}
/// @brief whether or not a marker belongs to a transaction
static bool IsTransactionWalMarker(MMFilesReplicationDumpContext* dump,
                                   MMFilesMarker const* marker) {
  // first check the marker type
  if (!IsTransactionWalMarkerType(marker)) {
    return false;
  }

  // then check if the marker belongs to the "correct" database
  if (dump->_vocbase->id() != MMFilesDatafileHelper::DatabaseId(marker)) {
    return false;
  }

  return true;
}

/// @brief whether or not a marker is replicated
static bool MustReplicateWalMarker(
    MMFilesReplicationDumpContext* dump, MMFilesMarker const* marker,
    TRI_voc_tick_t databaseId, TRI_voc_cid_t collectionId,
    TRI_voc_tick_t firstRegularTick,
    std::unordered_set<TRI_voc_tid_t> const& transactionIds) {
  // first check the marker type
  if (!MustReplicateWalMarkerType(marker)) {
    return false;
  }

  // then check if the marker belongs to the "correct" database
  if (dump->_vocbase->id() != databaseId) {
    return false;
  }

  // finally check if the marker is for a collection that we want to ignore
  TRI_voc_cid_t cid = collectionId;

  if (cid != 0) {
    char const* name = NameFromCid(dump, cid);

    if (name != nullptr &&
        TRI_ExcludeCollectionReplication(name, dump->_includeSystem)) {
      return false;
    }
  }

  if (dump->_restrictCollection > 0 &&
      (cid != dump->_restrictCollection &&
       !IsTransactionWalMarker(dump, marker))) {
    // restrict output to a single collection, but a different one
    return false;
  }

  if (marker->getTick() >= firstRegularTick) {
    return true;
  }

  if (!transactionIds.empty()) {
    TRI_voc_tid_t tid = MMFilesDatafileHelper::TransactionId(marker);
    if (tid == 0 || transactionIds.find(tid) == transactionIds.end()) {
      return false;
    }
  }

  return true;
}

/// @brief dump data from a collection
static int DumpCollection(MMFilesReplicationDumpContext* dump,
                          LogicalCollection* collection,
                          TRI_voc_tick_t databaseId, TRI_voc_cid_t collectionId,
                          TRI_voc_tick_t dataMin, TRI_voc_tick_t dataMax,
                          bool withTicks, bool useVst = false) {
  LOG_TOPIC(TRACE, avocadodb::Logger::FIXME) << "dumping collection " << collection->cid() << ", tick range "
             << dataMin << " - " << dataMax;

  bool const isEdgeCollection = (collection->type() == TRI_COL_TYPE_EDGE);

  // setup some iteration state
  TRI_voc_tick_t lastFoundTick = 0;
  bool bufferFull = false;

  auto callback = [&dump, &lastFoundTick, &databaseId, &collectionId,
                   &withTicks, &isEdgeCollection, &bufferFull, &useVst,
                   &collection](
      TRI_voc_tick_t foundTick, MMFilesMarker const* marker) {
    // note the last tick we processed
    lastFoundTick = foundTick;

    int res;
    if (useVst) {
      res = SliceifyMarker(dump, databaseId, collectionId, marker, true,
                           withTicks, isEdgeCollection);
    } else {
      res = StringifyMarker(dump, databaseId, collectionId, marker, true,
                            withTicks, isEdgeCollection);
    }

    if (res != TRI_ERROR_NO_ERROR) {
      LOG_TOPIC(ERR, avocadodb::Logger::FIXME) << "got error during dump dump of collection '" << collection->name() << "': " << TRI_errno_string(res);
      THROW_AVOCADO_EXCEPTION(res);
    }

    // TODO if vstcase find out slice lenght of _slices.back()
    if (static_cast<uint64_t>(TRI_LengthStringBuffer(dump->_buffer)) >
        dump->_chunkSize) {
      // abort the iteration
      bufferFull = true;
      return false;  // stop iterating
    }

    return true;  // continue iterating
  };

  try {
    bool hasMore = static_cast<MMFilesCollection*>(collection->getPhysical())->applyForTickRange(dataMin, dataMax, callback);

    if (lastFoundTick > 0) {
      // data available for requested range
      dump->_lastFoundTick = lastFoundTick;
      dump->_hasMore = hasMore;
      dump->_bufferFull = bufferFull;
    } else {
      // no data available for requested range
      dump->_lastFoundTick = 0;
      dump->_hasMore = false;
      dump->_bufferFull = false;
    }

    return TRI_ERROR_NO_ERROR;
  } catch (basics::Exception const& ex) {
    LOG_TOPIC(ERR, avocadodb::Logger::FIXME) << "caught exception during dump of collection '" << collection->name() << "': " << ex.what();
    return ex.code();
  } catch (std::exception const& ex) {
    LOG_TOPIC(ERR, avocadodb::Logger::FIXME) << "caught exception during dump of collection '" << collection->name() << "': " << ex.what();
    return TRI_ERROR_INTERNAL;
  } catch (...) {
    LOG_TOPIC(ERR, avocadodb::Logger::FIXME) << "caught unknown exception during dump of collection '" << collection->name() << "'";
    return TRI_ERROR_INTERNAL;
  }
}

/// @brief dump data from a collection
int MMFilesDumpCollectionReplication(MMFilesReplicationDumpContext* dump,
                                  avocadodb::LogicalCollection* collection,
                                  TRI_voc_tick_t dataMin,
                                  TRI_voc_tick_t dataMax, bool withTicks) {
  TRI_ASSERT(collection != nullptr);

  // get a custom type handler
  auto customTypeHandler = dump->_transactionContext->orderCustomTypeHandler();
  dump->_vpackOptions.customTypeHandler = customTypeHandler.get();

  auto mmfiles = avocadodb::MMFilesCollection::toMMFilesCollection(collection);
  // create a barrier so the underlying collection is not unloaded
  auto b = mmfiles->ditches()->createMMFilesReplicationDitch(__FILE__, __LINE__);

  if (b == nullptr) {
    return TRI_ERROR_OUT_OF_MEMORY;
  }

  // block compaction
  int res;
  {
    auto mmfiles = avocadodb::MMFilesCollection::toMMFilesCollection(collection);
    MMFilesCompactionPreventer compactionPreventer(mmfiles);

    try {
      res = DumpCollection(dump, collection, collection->vocbase()->id(),
                           collection->cid(), dataMin, dataMax, withTicks);
    } catch (...) {
      res = TRI_ERROR_INTERNAL;
    }
  }

  // always execute this
  mmfiles->ditches()->freeDitch(b);

  return res;
}

/// @brief dump data from the replication log
int MMFilesDumpLogReplication(
    MMFilesReplicationDumpContext* dump,
    std::unordered_set<TRI_voc_tid_t> const& transactionIds,
    TRI_voc_tick_t firstRegularTick, TRI_voc_tick_t tickMin,
    TRI_voc_tick_t tickMax, bool outputAsArray) {
  LOG_TOPIC(TRACE, avocadodb::Logger::FIXME) << "dumping log, tick range " << tickMin << " - " << tickMax;

  // get a custom type handler
  auto customTypeHandler = dump->_transactionContext->orderCustomTypeHandler();
  dump->_vpackOptions.customTypeHandler = customTypeHandler.get();

  // ask the logfile manager which datafiles qualify
  bool fromTickIncluded = false;
  std::vector<avocadodb::MMFilesWalLogfile*> logfiles =
      MMFilesLogfileManager::instance()->getLogfilesForTickRange(
          tickMin, tickMax, fromTickIncluded);

  // setup some iteration state
  int res = TRI_ERROR_NO_ERROR;
  TRI_voc_tick_t lastFoundTick = 0;
  TRI_voc_tick_t lastDatabaseId = 0;
  TRI_voc_cid_t lastCollectionId = 0;
  bool hasMore = true;
  bool bufferFull = false;

  try {
    if (outputAsArray) {
      Append(dump, "[");
    }
    bool first = true;

    // iterate over the datafiles found
    size_t const n = logfiles.size();

    for (size_t i = 0; i < n; ++i) {
      avocadodb::MMFilesWalLogfile* logfile = logfiles[i];

      char const* ptr;
      char const* end;
      MMFilesLogfileManager::instance()->getActiveLogfileRegion(
          logfile, ptr, end);

      while (ptr < end) {
        auto const* marker = reinterpret_cast<MMFilesMarker const*>(ptr);

        if (marker->getSize() == 0) {
          // end of datafile
          break;
        }

        MMFilesMarkerType type = marker->getType();

        if (type <= TRI_DF_MARKER_MIN || type >= TRI_DF_MARKER_MAX) {
          break;
        }

        // handle special markers
        if (type == TRI_DF_MARKER_PROLOGUE) {
          lastDatabaseId = MMFilesDatafileHelper::DatabaseId(marker);
          lastCollectionId = MMFilesDatafileHelper::CollectionId(marker);
        } else if (type == TRI_DF_MARKER_HEADER ||
                   type == TRI_DF_MARKER_FOOTER) {
          lastDatabaseId = 0;
          lastCollectionId = 0;
        } else if (type == TRI_DF_MARKER_VPACK_CREATE_COLLECTION) {
          // fill collection name cache
          TRI_voc_tick_t databaseId = MMFilesDatafileHelper::DatabaseId(marker);
          TRI_ASSERT(databaseId != 0);
          TRI_voc_cid_t collectionId = MMFilesDatafileHelper::CollectionId(marker);
          TRI_ASSERT(collectionId != 0);

          if (dump->_vocbase->id() == databaseId) {
            VPackSlice slice(reinterpret_cast<char const*>(marker) +
                             MMFilesDatafileHelper::VPackOffset(type));
            VPackSlice name = slice.get("name");
            if (name.isString()) {
              dump->_collectionNames[collectionId] = name.copyString();
            }
          }
        } else if (type == TRI_DF_MARKER_VPACK_RENAME_COLLECTION) {
          // invalidate collection name cache because this is a
          // rename operation
          dump->_collectionNames.clear();
        }

        ptr += MMFilesDatafileHelper::AlignedMarkerSize<size_t>(marker);

        // get the marker's tick and check whether we should include it
        TRI_voc_tick_t foundTick = marker->getTick();

        if (foundTick <= tickMin) {
          // marker too old
          continue;
        }

        if (foundTick >= tickMax) {
          hasMore = false;

          if (foundTick > tickMax) {
            // marker too new
            break;
          }
        }

        TRI_voc_tick_t databaseId;
        TRI_voc_cid_t collectionId;

        if (type == TRI_DF_MARKER_VPACK_DOCUMENT ||
            type == TRI_DF_MARKER_VPACK_REMOVE) {
          databaseId = lastDatabaseId;
          collectionId = lastCollectionId;
        } else {
          databaseId = MMFilesDatafileHelper::DatabaseId(marker);
          collectionId = MMFilesDatafileHelper::CollectionId(marker);
        }

        if (!MustReplicateWalMarker(dump, marker, databaseId, collectionId,
                                    firstRegularTick, transactionIds)) {
          continue;
        }

        // note the last tick we processed
        lastFoundTick = foundTick;

        if (outputAsArray) {
          if (!first) {
            Append(dump, ",");
          } else {
            first = false;
          }
        }

        if (dump->_useVst) {
          res = SliceifyMarker(dump, databaseId, collectionId, marker, false,
                               true, false);
        } else {
          res = StringifyMarker(dump, databaseId, collectionId, marker, false,
                                true, false);
        }

        if (res != TRI_ERROR_NO_ERROR) {
          THROW_AVOCADO_EXCEPTION(res);
        }

        if (static_cast<uint64_t>(TRI_LengthStringBuffer(dump->_buffer)) >=
            dump->_chunkSize) {
          // abort the iteration
          bufferFull = true;
          break;
        }
      }

      if (!hasMore || bufferFull) {
        break;
      }
    }

    if (outputAsArray) {
      Append(dump, "]");
    }
  } catch (avocadodb::basics::Exception const& ex) {
    LOG_TOPIC(ERR, avocadodb::Logger::FIXME) << "caught exception while dumping replication log: " << ex.what();
    res = ex.code();
  } catch (std::exception const& ex) {
    LOG_TOPIC(ERR, avocadodb::Logger::FIXME) << "caught exception while dumping replication log: " << ex.what();
    res = TRI_ERROR_INTERNAL;
  } catch (...) {
    LOG_TOPIC(ERR, avocadodb::Logger::FIXME) << "caught unknown exception while dumping replication log";
    res = TRI_ERROR_INTERNAL;
  }

  // always return the logfiles we have used
  MMFilesLogfileManager::instance()->returnLogfiles(logfiles);

  dump->_fromTickIncluded = fromTickIncluded;

  if (res == TRI_ERROR_NO_ERROR) {
    if (lastFoundTick > 0) {
      // data available for requested range
      dump->_lastFoundTick = lastFoundTick;
      dump->_hasMore = hasMore;
      dump->_bufferFull = bufferFull;
    } else {
      // no data available for requested range
      dump->_lastFoundTick = 0;
      dump->_hasMore = false;
      dump->_bufferFull = false;
    }
  }

  return res;
}

/// @brief determine the transactions that were open at a given point in time
int MMFilesDetermineOpenTransactionsReplication(MMFilesReplicationDumpContext* dump,
                                             TRI_voc_tick_t tickMin,
                                             TRI_voc_tick_t tickMax,
                                             bool useVst) {
  LOG_TOPIC(TRACE, avocadodb::Logger::FIXME) << "determining transactions, tick range " << tickMin << " - "
             << tickMax;

  std::unordered_map<TRI_voc_tid_t, TRI_voc_tick_t> transactions;

  // ask the logfile manager which datafiles qualify
  bool fromTickIncluded = false;
  std::vector<avocadodb::MMFilesWalLogfile*> logfiles =
      MMFilesLogfileManager::instance()->getLogfilesForTickRange(
          tickMin, tickMax, fromTickIncluded);

  // setup some iteration state
  TRI_voc_tick_t lastFoundTick = 0;
  int res = TRI_ERROR_NO_ERROR;

  // LOG_TOPIC(INFO, avocadodb::Logger::FIXME) << "found logfiles: " << logfiles.size();

  try {
    // iterate over the datafiles found
    size_t const n = logfiles.size();
    for (size_t i = 0; i < n; ++i) {
      avocadodb::MMFilesWalLogfile* logfile = logfiles[i];

      char const* ptr;
      char const* end;
      MMFilesLogfileManager::instance()->getActiveLogfileRegion(
          logfile, ptr, end);

      // LOG_TOPIC(INFO, avocadodb::Logger::FIXME) << "scanning logfile " << i;
      while (ptr < end) {
        auto const* marker = reinterpret_cast<MMFilesMarker const*>(ptr);

        if (marker->getSize() == 0) {
          // end of datafile
          break;
        }

        MMFilesMarkerType const type = marker->getType();

        if (type <= TRI_DF_MARKER_MIN || type >= TRI_DF_MARKER_MAX) {
          // somehow invalid
          break;
        }

        ptr += MMFilesDatafileHelper::AlignedMarkerSize<size_t>(marker);

        // get the marker's tick and check whether we should include it
        TRI_voc_tick_t const foundTick = marker->getTick();

        if (foundTick <= tickMin) {
          // marker too old
          continue;
        }

        if (foundTick > tickMax) {
          // marker too new
          break;
        }

        // note the last tick we processed
        if (foundTick > lastFoundTick) {
          lastFoundTick = foundTick;
        }

        if (!IsTransactionWalMarker(dump, marker)) {
          continue;
        }

        TRI_voc_tid_t tid = MMFilesDatafileHelper::TransactionId(marker);
        TRI_ASSERT(tid > 0);

        if (type == TRI_DF_MARKER_VPACK_BEGIN_TRANSACTION) {
          transactions.emplace(tid, foundTick);
        } else if (type == TRI_DF_MARKER_VPACK_COMMIT_TRANSACTION ||
                   type == TRI_DF_MARKER_VPACK_ABORT_TRANSACTION) {
          transactions.erase(tid);
        } else {
          THROW_AVOCADO_EXCEPTION_MESSAGE(TRI_ERROR_INTERNAL,
                                         "found invalid marker type");
        }
      }
    }

    // LOG_TOPIC(INFO, avocadodb::Logger::FIXME) << "found transactions: " << transactions.size();
    // LOG_TOPIC(INFO, avocadodb::Logger::FIXME) << "last tick: " << lastFoundTick;

    VPackBuffer<uint8_t> buffer;
    VPackBuilder builder(buffer);
    if (useVst) {
      if (transactions.empty()) {
        builder.add(VPackSlice::emptyArraySlice());
      } else {
        builder.openArray();
        for (auto const& it : transactions) {
          if (it.second - 1 < lastFoundTick) {
            lastFoundTick = it.second - 1;
          }
          builder.add(VPackValue(it.first));
        }
        builder.close();
      }

    } else {
      if (transactions.empty()) {
        Append(dump, "[]");
      } else {
        bool first = true;
        Append(dump, "[\"");

        for (auto const& it : transactions) {
          if (it.second - 1 < lastFoundTick) {
            lastFoundTick = it.second - 1;
          }

          if (first) {
            first = false;
          } else {
            Append(dump, "\",\"");
          }

          Append(dump, it.first);
        }

        Append(dump, "\"]");
      }
    }

    dump->_fromTickIncluded = fromTickIncluded;
    dump->_lastFoundTick = lastFoundTick;
    // LOG_TOPIC(INFO, avocadodb::Logger::FIXME) << "last tick2: " << lastFoundTick;

    dump->_slices.push_back(std::move(buffer));

  } catch (avocadodb::basics::Exception const& ex) {
    LOG_TOPIC(ERR, avocadodb::Logger::FIXME) << "caught exception while determining open transactions: " << ex.what();
    res = ex.code();
  } catch (std::exception const& ex) {
    LOG_TOPIC(ERR, avocadodb::Logger::FIXME) << "caught exception while determining open transactions: " << ex.what();
    res = TRI_ERROR_INTERNAL;
  } catch (...) {
    LOG_TOPIC(ERR, avocadodb::Logger::FIXME) << "caught unknown exception while determining open transactions";
    res = TRI_ERROR_INTERNAL;
  }

  // always return the logfiles we have used
  MMFilesLogfileManager::instance()->returnLogfiles(logfiles);

  return res;
}
