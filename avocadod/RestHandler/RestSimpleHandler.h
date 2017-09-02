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

#ifndef AVOCADOD_REST_HANDLER_REST_SIMPLE_HANDLER_H
#define AVOCADOD_REST_HANDLER_REST_SIMPLE_HANDLER_H 1

#include "Basics/Common.h"
#include "Basics/Mutex.h"
#include "Aql/QueryResult.h"
#include "RestHandler/RestVocbaseBaseHandler.h"

namespace avocadodb {
namespace aql {
class Query;
class QueryRegistry;
}

class RestSimpleHandler : public RestVocbaseBaseHandler {
 public:
  RestSimpleHandler(GeneralRequest*, GeneralResponse*, aql::QueryRegistry*);

 public:
  RestStatus execute() override final;
  char const* name() const override final { return "RestSimpleHandler"; }
  bool cancel() override;

 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief register the currently running query
  //////////////////////////////////////////////////////////////////////////////

  void registerQuery(avocadodb::aql::Query*);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief unregister the currently running query
  //////////////////////////////////////////////////////////////////////////////

  void unregisterQuery();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief cancel the currently running query
  //////////////////////////////////////////////////////////////////////////////

  bool cancelQuery();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not the query was canceled
  //////////////////////////////////////////////////////////////////////////////

  bool wasCanceled();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief execute a batch remove operation
  //////////////////////////////////////////////////////////////////////////////

  void removeByKeys(VPackSlice const&);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief execute a batch lookup operation
  //////////////////////////////////////////////////////////////////////////////

  void lookupByKeys(VPackSlice const&);

 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief our query registry
  //////////////////////////////////////////////////////////////////////////////

  avocadodb::aql::QueryRegistry* _queryRegistry;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief lock for currently running query
  //////////////////////////////////////////////////////////////////////////////

  Mutex _queryLock;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief currently running query
  //////////////////////////////////////////////////////////////////////////////

  avocadodb::aql::Query* _query;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not the query was killed
  //////////////////////////////////////////////////////////////////////////////

  bool _queryKilled;
};
}

#endif
