////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2016 AvocadoDB GmbH, Cologne, Germany
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
/// @author Dr. Frank Celler
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADODB_APPLICATION_FEATURES_HTTP_ENDPOINT_PROVIDER_H
#define AVOCADODB_APPLICATION_FEATURES_HTTP_ENDPOINT_PROVIDER_H 1

#include "Basics/Common.h"

namespace avocadodb {
class HttpEndpointProvider {
 public:
  virtual ~HttpEndpointProvider() = default;
  virtual std::vector<std::string> httpEndpoints() = 0;
};
}

#endif
