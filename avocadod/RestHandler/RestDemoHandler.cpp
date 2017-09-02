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

#include "RestDemoHandler.h"

#include <velocypack/Builder.h>
#include <velocypack/velocypack-aliases.h>

#include "Rest/HttpRequest.h"
#include "Rest/Version.h"

using namespace avocadodb;
using namespace avocadodb::basics;
using namespace avocadodb::rest;

RestDemoHandler::RestDemoHandler(GeneralRequest* request,
                                 GeneralResponse* response)
    : RestBaseHandler(request, response) {}

RestStatus RestDemoHandler::execute() {
  return RestStatus::QUEUE
      .then([]() { LOG_TOPIC(INFO, avocadodb::Logger::FIXME) << "demo handler going to sleep"; })
      .then([]() { sleep(5); })
      .then([]() { LOG_TOPIC(INFO, avocadodb::Logger::FIXME) << "demo handler done sleeping"; })
      .then([this]() { doSomeMoreWork(); })
      .then([this]() { return evenMoreWork(); });
}

void RestDemoHandler::doSomeMoreWork() {
  LOG_TOPIC(INFO, avocadodb::Logger::FIXME) << "demo handler working very hard";
}

RestStatus RestDemoHandler::evenMoreWork() {
  LOG_TOPIC(INFO, avocadodb::Logger::FIXME) << "demo handler almost done";

  VPackBuilder result;
  result.add(VPackValue(VPackValueType::Object));
  result.add("server", VPackValue("avocado"));
  result.add("version", VPackValue(ARANGODB_VERSION));
  result.close();

  generateResult(rest::ResponseCode::OK, result.slice());

  return RestStatus::DONE
      .then([]() { LOG_TOPIC(INFO, avocadodb::Logger::FIXME) << "demo handler keeps working"; })
      .then([]() { sleep(5); })
      .then(
          []() { LOG_TOPIC(INFO, avocadodb::Logger::FIXME) << "even if the result has already been returned"; })
      .then([]() { LOG_TOPIC(INFO, avocadodb::Logger::FIXME) << "finally done"; });
}
