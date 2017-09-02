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

#ifndef AVOCADOD_ROCKSDB_ENGINE_ROCKSDB_PRIMARY_INDEX_H
#define AVOCADOD_ROCKSDB_ENGINE_ROCKSDB_PRIMARY_INDEX_H 1

#include "Basics/Common.h"
#include "Indexes/Index.h"
#include "Indexes/IndexIterator.h"
#include "RocksDBEngine/RocksDBIndex.h"
#include "RocksDBEngine/RocksDBKeyBounds.h"
#include "RocksDBEngine/RocksDBToken.h"
#include "VocBase/voc-types.h"
#include "VocBase/vocbase.h"

#include <velocypack/Iterator.h>
#include <velocypack/Slice.h>
#include <velocypack/velocypack-aliases.h>

namespace rocksdb {
class Iterator;
class Comparator;
}

namespace avocadodb {

class RocksDBPrimaryIndex;
namespace transaction {
class Methods;
}

class RocksDBPrimaryIndexIterator final : public IndexIterator {
 public:
  RocksDBPrimaryIndexIterator(LogicalCollection* collection,
                              transaction::Methods* trx,
                              ManagedDocumentResult* mmdr,
                              RocksDBPrimaryIndex* index,
                              std::unique_ptr<VPackBuilder>& keys);

  ~RocksDBPrimaryIndexIterator();

  char const* typeName() const override { return "primary-index-iterator"; }

  bool next(TokenCallback const& cb, size_t limit) override;

  void reset() override;

 private:
  RocksDBPrimaryIndex* _index;
  std::unique_ptr<VPackBuilder> _keys;
  avocadodb::velocypack::ArrayIterator _iterator;
};

class RocksDBPrimaryIndex final : public RocksDBIndex {
  friend class RocksDBPrimaryIndexIterator;
  friend class RocksDBAllIndexIterator;
  friend class RocksDBAnyIndexIterator;

 public:
  RocksDBPrimaryIndex() = delete;

  RocksDBPrimaryIndex(avocadodb::LogicalCollection*,
                      VPackSlice const& info);

  ~RocksDBPrimaryIndex();

 public:
  IndexType type() const override { return Index::TRI_IDX_TYPE_PRIMARY_INDEX; }

  char const* typeName() const override { return "primary"; }

  bool allowExpansion() const override { return false; }

  bool canBeDropped() const override { return false; }

  bool isSorted() const override { return false; }

  bool hasSelectivityEstimate() const override { return true; }

  double selectivityEstimateLocal(
      avocadodb::StringRef const* = nullptr) const override {
    return 1.0;
  }

  void toVelocyPack(VPackBuilder&, bool, bool) const override;

  RocksDBToken lookupKey(transaction::Methods* trx,
                         avocadodb::StringRef key) const;

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

  void invokeOnAllElements(
      transaction::Methods* trx,
      std::function<bool(DocumentIdentifierToken const&)> callback) const;

  /// insert index elements into the specified write batch.
  Result insertInternal(transaction::Methods* trx, RocksDBMethods*,
                        TRI_voc_rid_t,
                        avocadodb::velocypack::Slice const&) override;

  /// remove index elements and put it in the specified write batch.
  Result removeInternal(transaction::Methods*, RocksDBMethods*, TRI_voc_rid_t,
                        avocadodb::velocypack::Slice const&) override;

 protected:
  Result postprocessRemove(transaction::Methods* trx, rocksdb::Slice const& key,
                           rocksdb::Slice const& value) override;

 private:
  /// @brief create the iterator, for a single attribute, IN operator
  IndexIterator* createInIterator(transaction::Methods*, ManagedDocumentResult*,
                                  avocadodb::aql::AstNode const*,
                                  avocadodb::aql::AstNode const*);

  /// @brief create the iterator, for a single attribute, EQ operator
  IndexIterator* createEqIterator(transaction::Methods*, ManagedDocumentResult*,
                                  avocadodb::aql::AstNode const*,
                                  avocadodb::aql::AstNode const*);

  /// @brief add a single value node to the iterator's keys
  void handleValNode(transaction::Methods* trx, VPackBuilder* keys,
                     avocadodb::aql::AstNode const* valNode, bool isId) const;
};
}  // namespace avocadodb

#endif
