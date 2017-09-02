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

#ifndef ARANGOD_VOCBASE_PHYSICAL_VIEW_H
#define ARANGOD_VOCBASE_PHYSICAL_VIEW_H 1

#include "Basics/Common.h"
#include "VocBase/voc-types.h"

#include <velocypack/Builder.h>
#include <velocypack/Slice.h>

namespace avocadodb {
class LogicalView;
class Result;

class PhysicalView {
 protected:
  PhysicalView(LogicalView* view, avocadodb::velocypack::Slice const& info)
      : _logicalView(view) {}

 public:
  virtual ~PhysicalView() = default;

  // path to logical view
  virtual std::string const& path() const = 0;
  virtual void setPath(std::string const&) = 0;
  virtual avocadodb::Result updateProperties(
      avocadodb::velocypack::Slice const& slice, bool doSync) = 0;
  virtual avocadodb::Result persistProperties() = 0;

  virtual PhysicalView* clone(LogicalView*, PhysicalView*) = 0;

  /// @brief export properties
  virtual void getPropertiesVPack(velocypack::Builder&,
                                  bool includeSystem = false) const = 0;

  /// @brief opens an existing view
  virtual void open() = 0;

  virtual void drop() = 0;

 protected:
  LogicalView* _logicalView;
};

}  // namespace avocadodb

#endif
