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
/// @author Simon Grätzer
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_ROCKSDB_ENGINE_ROCKSDB_EDGE_INDEX_H
#define ARANGOD_ROCKSDB_ENGINE_ROCKSDB_EDGE_INDEX_H 1

#include "Basics/Common.h"
#include "Indexes/Index.h"
#include "Indexes/IndexIterator.h"
#include "RocksDBEngine/RocksDBCuckooIndexEstimator.h"
#include "RocksDBEngine/RocksDBIndex.h"
#include "RocksDBEngine/RocksDBKey.h"
#include "RocksDBEngine/RocksDBKeyBounds.h"
#include "RocksDBEngine/RocksDBToken.h"
#include "VocBase/voc-types.h"
#include "VocBase/vocbase.h"

#include <velocypack/Iterator.h>
#include <velocypack/Slice.h>

namespace rocksdb {
class TransactionDB;
class Iterator;
}  // namespace rocksdb

namespace avocadodb {
class RocksDBEdgeIndex;

class RocksDBEdgeIndexIterator final : public IndexIterator {
 public:
  RocksDBEdgeIndexIterator(LogicalCollection* collection,
                           transaction::Methods* trx,
                           ManagedDocumentResult* mmdr,
                           avocadodb::RocksDBEdgeIndex const* index,
                           std::unique_ptr<VPackBuilder>& keys,
                           std::shared_ptr<cache::Cache>);
  ~RocksDBEdgeIndexIterator();
  char const* typeName() const override { return "edge-index-iterator"; }
  bool hasExtra() const override { return true; }
  bool next(TokenCallback const& cb, size_t limit) override;
  bool nextExtra(ExtraCallback const& cb, size_t limit) override;
  void reset() override;

 private:
  void resetInplaceMemory();
  avocadodb::StringRef getFromToFromIterator(
      avocadodb::velocypack::ArrayIterator const&);
  void lookupInRocksDB(StringRef edgeKey);

  std::unique_ptr<avocadodb::velocypack::Builder> _keys;
  avocadodb::velocypack::ArrayIterator _keysIterator;
  RocksDBEdgeIndex const* _index;

  // the following 2 values are required for correct batch handling
  std::unique_ptr<rocksdb::Iterator> _iterator;  // iterator position in rocksdb
  RocksDBKeyBounds _bounds;
  std::shared_ptr<cache::Cache> _cache;
  avocadodb::velocypack::ArrayIterator _builderIterator;
  avocadodb::velocypack::Builder _builder;
};

class RocksDBEdgeIndex final : public RocksDBIndex {
  friend class RocksDBEdgeIndexIterator;

 public:
  static uint64_t HashForKey(const rocksdb::Slice& key);

  RocksDBEdgeIndex() = delete;

  RocksDBEdgeIndex(TRI_idx_iid_t, avocadodb::LogicalCollection*,
                   velocypack::Slice const& info, std::string const&);

  ~RocksDBEdgeIndex();

  IndexType type() const override { return Index::TRI_IDX_TYPE_EDGE_INDEX; }

  char const* typeName() const override { return "edge"; }

  bool allowExpansion() const override { return false; }

  bool canBeDropped() const override { return false; }

  bool isSorted() const override { return false; }

  bool hasSelectivityEstimate() const override { return true; }

  double selectivityEstimateLocal(
      avocadodb::StringRef const* = nullptr) const override;

  void toVelocyPack(VPackBuilder&, bool, bool) const override;

  void batchInsert(
      transaction::Methods*,
      std::vector<std::pair<TRI_voc_rid_t, avocadodb::velocypack::Slice>> const&,
      std::shared_ptr<avocadodb::basics::LocalTaskQueue> queue) override;

  bool hasBatchInsert() const override { return false; }

  bool supportsFilterCondition(avocadodb::aql::AstNode const*,
                               avocadodb::aql::Variable const*, size_t, size_t&,
                               double&) const override;

  IndexIterator* iteratorForCondition(transaction::Methods*,
                                      ManagedDocumentResult*,
                                      avocadodb::aql::AstNode const*,
                                      avocadodb::aql::Variable const*,
                                      bool) override;

  avocadodb::aql::AstNode* specializeCondition(
      avocadodb::aql::AstNode*, avocadodb::aql::Variable const*) const override;

  /// @brief Transform the list of search slices to search values.
  ///        This will multiply all IN entries and simply return all other
  ///        entries.
  void expandInSearchValues(avocadodb::velocypack::Slice const,
                            avocadodb::velocypack::Builder&) const override;

  /// @brief Warmup the index caches.
  void warmup(avocadodb::transaction::Methods* trx) override;

  void serializeEstimate(std::string& output) const override;

  bool deserializeEstimate(avocadodb::RocksDBCounterManager* mgr) override;

  void recalculateEstimates() override;

  Result insertInternal(transaction::Methods*, RocksDBMethods*, TRI_voc_rid_t,
                        avocadodb::velocypack::Slice const&) override;

  Result removeInternal(transaction::Methods*, RocksDBMethods*, TRI_voc_rid_t,
                        avocadodb::velocypack::Slice const&) override;

 protected:
  Result postprocessRemove(transaction::Methods* trx, rocksdb::Slice const& key,
                           rocksdb::Slice const& value) override;

 private:
  /// @brief create the iterator
  IndexIterator* createEqIterator(transaction::Methods*, ManagedDocumentResult*,
                                  avocadodb::aql::AstNode const*,
                                  avocadodb::aql::AstNode const*) const;

  IndexIterator* createInIterator(transaction::Methods*, ManagedDocumentResult*,
                                  avocadodb::aql::AstNode const*,
                                  avocadodb::aql::AstNode const*) const;

  /// @brief add a single value node to the iterator's keys
  void handleValNode(VPackBuilder* keys,
                     avocadodb::aql::AstNode const* valNode) const;
  
  void warmupInternal(transaction::Methods* trx,
                      rocksdb::Slice const& lower, rocksdb::Slice const& upper);
  
 private:

  std::string _directionAttr;
  bool _isFromIndex;

  /// @brief A fixed size library to estimate the selectivity of the index.
  /// On insertion of a document we have to insert it into the estimator,
  /// On removal we have to remove it in the estimator as well.
  std::unique_ptr<RocksDBCuckooIndexEstimator<uint64_t>> _estimator;
};
}  // namespace avocadodb

#endif
