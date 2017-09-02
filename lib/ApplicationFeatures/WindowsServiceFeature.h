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
/// @author Wilfried Goesgens
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADODB_APPLICATION_FEATURES_WINDOWS_SERVICE_FEATURE_H
#define AVOCADODB_APPLICATION_FEATURES_WINDOWS_SERVICE_FEATURE_H 1

#include "ApplicationFeatures/ApplicationFeature.h"

extern SERVICE_STATUS_HANDLE ServiceStatus;

void SetServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode,
                      DWORD dwCheckPoint, DWORD dwWaitHint, DWORD exitCode);

void WINAPI ServiceCtrl(DWORD dwCtrlCode);

namespace avocadodb {
class WindowsServiceFeature final : public application_features::ApplicationFeature {
 public:
  explicit WindowsServiceFeature(application_features::ApplicationServer* server);

  void collectOptions(std::shared_ptr<options::ProgramOptions>) override final;
  void validateOptions(std::shared_ptr<options::ProgramOptions>) override final;

 private:
  void installService();
  void StartAvocadoService (bool WaitForRunning);
  void StopAvocadoService (bool WaitForShutdown);
  void startupProgress ();

  void startupFinished ();

  void shutDownBegins ();
  void shutDownComplete ();
  void shutDownFailure ();
  void abortFailure();
  static void abortService();
 public:
  bool _installService = false;
  bool _unInstallService = false;
  bool _forceUninstall = false;
  
  bool _startAsService = false;

  bool _startService = false;
  bool _startWaitService = false;
  
  bool _stopService = false;
  bool _stopWaitService = false;

  application_features::ApplicationServer* _server;

 private:
  uint16_t _progress;
};
};

#endif
