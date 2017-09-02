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

#ifndef ARANGOD_REST_SERVER_CONSOLE_THREAD_H
#define ARANGOD_REST_SERVER_CONSOLE_THREAD_H 1

#include "Basics/Thread.h"
#include "V8Server/V8Context.h"

struct TRI_vocbase_t;

namespace avocadodb {
class V8LineEditor;
}

namespace avocadodb {
namespace application_features {
class ApplicationServer;
}

class ConsoleThread : public Thread {
  ConsoleThread(const ConsoleThread&) = delete;
  ConsoleThread& operator=(const ConsoleThread&) = delete;

 public:
  static avocadodb::V8LineEditor* serverConsole;
  static avocadodb::Mutex serverConsoleMutex;

 public:
  ConsoleThread(application_features::ApplicationServer*, TRI_vocbase_t*);

  ~ConsoleThread();

 public:
  void run() override;
  bool isSilent() override { return true; }

 public:
  void userAbort() { _userAborted.store(true); }

 private:
  void inner();

 private:
  application_features::ApplicationServer* _applicationServer;
  V8Context* _context;
  TRI_vocbase_t* _vocbase;
  std::atomic<bool> _userAborted;
};
}

#endif
