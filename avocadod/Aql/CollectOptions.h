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
/// @author Max Neunhoeffer
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_AQL_COLLECT_OPTIONS_H
#define AVOCADOD_AQL_COLLECT_OPTIONS_H 1

#include "Basics/Common.h"

#include <velocypack/Builder.h>
#include <velocypack/Slice.h>

namespace avocadodb {
namespace aql {

/// @brief CollectOptions
struct CollectOptions {
  /// @brief selected aggregation method
  enum CollectMethod {
    COLLECT_METHOD_UNDEFINED,
    COLLECT_METHOD_HASH,
    COLLECT_METHOD_SORTED
  };

  /// @brief constructor, using default values
  CollectOptions() : method(COLLECT_METHOD_UNDEFINED) {}

  /// @brief constructor
  explicit CollectOptions(avocadodb::velocypack::Slice const&);

  /// @brief whether or not the hash method can be used
  bool canUseHashMethod() const;

  /// @brief convert the options to VelocyPack
  void toVelocyPack(avocadodb::velocypack::Builder&) const;

  /// @brief get the aggregation method from a string
  static CollectMethod methodFromString(std::string const&);

  /// @brief stringify the aggregation method
  static std::string methodToString(CollectOptions::CollectMethod method);

  CollectMethod method;
};

}  // namespace avocadodb::aql
}  // namespace avocadodb

#endif
