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

#ifndef AVOCADOD_AQL_QUERY_EXECUTION_STATE_H
#define AVOCADOD_AQL_QUERY_EXECUTION_STATE_H 1

#include "Basics/Common.h"

#include <iosfwd>

namespace avocadodb {
namespace aql {
namespace QueryExecutionState {

/// @brief execution states
enum class ValueType {
  INITIALIZATION = 0,
  PARSING,
  AST_OPTIMIZATION,
  LOADING_COLLECTIONS,
  PLAN_INSTANTIATION,
  PLAN_OPTIMIZATION,
  EXECUTION,
  FINALIZATION,
  FINISHED,

  INVALID_STATE
};

QueryExecutionState::ValueType fromNumber(size_t value);
size_t toNumber(QueryExecutionState::ValueType value);
std::string toString(QueryExecutionState::ValueType state);
std::string toStringWithPrefix(QueryExecutionState::ValueType state);

}
}
}

std::ostream& operator<<(std::ostream&, avocadodb::aql::QueryExecutionState::ValueType);

#endif
