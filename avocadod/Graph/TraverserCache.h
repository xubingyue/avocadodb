////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2017-2017 AvocadoDB GmbH, Cologne, Germany
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
/// @author Michael Hackstein
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_GRAPH_TRAVERSER_CACHE_H
#define AVOCADOD_GRAPH_TRAVERSER_CACHE_H 1

#include "Basics/Common.h"
#include "Basics/StringRef.h"

namespace avocadodb {
class ManagedDocumentResult;
class StringHeap;

namespace transaction {
class Methods;
}

namespace velocypack {
class Builder;
class Slice;
}

namespace aql {
  struct AqlValue;
}
  
namespace graph {

struct EdgeDocumentToken;
struct SingleServerEdgeDocumentToken;

class TraverserCache {

  public:
   explicit TraverserCache(transaction::Methods* trx);

   virtual ~TraverserCache();

   //////////////////////////////////////////////////////////////////////////////
   /// @brief Inserts the real document stored within the token
   ///        into the given builder.
   ///        The document will be looked up in the StorageEngine
   //////////////////////////////////////////////////////////////////////////////

   virtual void insertIntoResult(StringRef idString,
                                 avocadodb::velocypack::Builder& builder);

   virtual void insertIntoResult(EdgeDocumentToken const* etkn,
                                 avocadodb::velocypack::Builder& builder);

   //////////////////////////////////////////////////////////////////////////////
   /// @brief Return AQL value containing the result
   ///        The document will be looked up in the StorageEngine
   //////////////////////////////////////////////////////////////////////////////

   virtual aql::AqlValue fetchAqlResult(avocadodb::graph::EdgeDocumentToken const*);

   //////////////////////////////////////////////////////////////////////////////
   /// @brief Return AQL value containing the result
   ///        The document will be looked up in the StorageEngine
   //////////////////////////////////////////////////////////////////////////////
  
   virtual aql::AqlValue fetchAqlResult(StringRef idString);

   //////////////////////////////////////////////////////////////////////////////
   /// @brief Insert value into store
   //////////////////////////////////////////////////////////////////////////////

   virtual void insertDocument(StringRef idString,
                               avocadodb::velocypack::Slice const& document);

   //////////////////////////////////////////////////////////////////////////////
   /// @brief Throws the document referenced by the token into the filter
   ///        function and returns it result.
   ///        The document will be looked up in the StorageEngine
   //////////////////////////////////////////////////////////////////////////////

   virtual bool validateFilter(StringRef idString,
                               std::function<bool(avocadodb::velocypack::Slice const&)> filterFunc);
  
   size_t getAndResetInsertedDocuments() {
     size_t tmp = _insertedDocuments;
     _insertedDocuments = 0;
     return tmp;
   }

   size_t getAndResetFilteredDocuments() {
     size_t tmp = _filteredDocuments;
     _filteredDocuments = 0;
     return tmp;
   }

   //////////////////////////////////////////////////////////////////////////////
   /// @brief Persist the given id string. The return value is guaranteed to
   ///        stay valid as long as this cache is valid
   //////////////////////////////////////////////////////////////////////////////
   StringRef persistString(StringRef const idString);

   void increaseFilterCounter() {
     _filteredDocuments++;
   }

   void increaseCounter() {
     _insertedDocuments++;
   }
  
  /// Only valid until the next call to this class
  virtual avocadodb::velocypack::Slice lookupToken(EdgeDocumentToken const* token);

  protected:

   //////////////////////////////////////////////////////////////////////////////
   /// @brief Lookup a document from the database.
   ///        The Slice returned here is only valid until the NEXT call of this
   ///        function.
   //////////////////////////////////////////////////////////////////////////////

   avocadodb::velocypack::Slice lookupInCollection(
       StringRef idString);

   avocadodb::velocypack::Slice lookupInCollection(
       SingleServerEdgeDocumentToken const* idToken);

  protected:

   //////////////////////////////////////////////////////////////////////////////
   /// @brief Reusable ManagedDocumentResult that temporarily takes
   ///        responsibility for one document.
   //////////////////////////////////////////////////////////////////////////////
   std::unique_ptr<ManagedDocumentResult> _mmdr;

   //////////////////////////////////////////////////////////////////////////////
   /// @brief Transaction to access data, This class is NOT responsible for it.
   //////////////////////////////////////////////////////////////////////////////
   avocadodb::transaction::Methods* _trx;
  
   //////////////////////////////////////////////////////////////////////////////
   /// @brief Documents inserted in this cache
   //////////////////////////////////////////////////////////////////////////////
   size_t _insertedDocuments;

   //////////////////////////////////////////////////////////////////////////////
   /// @brief Documents filtered
   //////////////////////////////////////////////////////////////////////////////
   size_t _filteredDocuments;

   //////////////////////////////////////////////////////////////////////////////
   /// @brief Stringheap to take care of _id strings, s.t. they stay valid
   ///        during the entire traversal.
   //////////////////////////////////////////////////////////////////////////////
   std::unique_ptr<avocadodb::StringHeap> _stringHeap;

   //////////////////////////////////////////////////////////////////////////////
   /// @brief Set of all strings persisted in the stringHeap. So we can save some
   ///        memory by not storing them twice.
   //////////////////////////////////////////////////////////////////////////////
   std::unordered_set<avocadodb::StringRef> _persistedStrings;
};

}
}

#endif
