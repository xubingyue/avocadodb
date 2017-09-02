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

#include "AvocadoGlobalContext.h"

#ifdef _WIN32
#include <DbgHelp.h>
#endif

#include "Basics/FileUtils.h"
#include "Basics/StringUtils.h"
#include "Basics/files.h"
#include "Logger/LogAppender.h"
#include "Logger/Logger.h"
#include "Rest/InitializeRest.h"

#include <regex>

using namespace avocadodb;
using namespace avocadodb::basics;

namespace {

/// @brief quick test of regex functionality of the underlying stdlib
static bool supportsStdRegex() {
  try {
    // compile a relatively simple regex... 
    std::regex re("^[ \t]*([#;].*)?$", std::regex::nosubs | std::regex::ECMAScript);
    // ...and test whether it matches a static string
    std::string test(" # AvocadoDB");
    if (std::regex_match(test, re)) {
      // compiler properly supports std::regex
      return true;
    }
  } catch (...) {
  }
  // compiler does not support std::regex properly, though pretending to
  return false;
}

static void abortHandler(int signum) {
  TRI_PrintBacktrace();
#ifdef _WIN32
  exit(255 + signum);
#else
  signal(signum, SIG_DFL);
  kill(getpid(), signum);
#endif
}

#ifndef _WIN32
static void ReopenLog(int) { LogAppender::reopen(); }
#endif

#ifdef _WIN32
static std::string miniDumpFilename = "c:\\avocadodpanic.dmp";

LONG CALLBACK unhandledExceptionHandler(EXCEPTION_POINTERS* e) {
#if AVOCADODB_ENABLE_BACKTRACE

  if ((e != nullptr) && (e->ExceptionRecord != nullptr)) {
    LOG_FATAL_WINDOWS("Unhandled exception: %d",
                      (int)e->ExceptionRecord->ExceptionCode);
  } else {
    LOG_FATAL_WINDOWS("Unhandled exception without ExceptionCode!");
  }

  std::string bt;
  TRI_GetBacktrace(bt);
  std::cerr << bt << std::endl;
  LOG_FATAL_WINDOWS(bt.c_str());

  HANDLE hFile =
      CreateFile(miniDumpFilename.c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0,
                 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

  if (hFile == INVALID_HANDLE_VALUE) {
    LOG_FATAL_WINDOWS("could not open minidump file : %lu", GetLastError());
    return EXCEPTION_CONTINUE_SEARCH;
  }

  MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
  exceptionInfo.ThreadId = GetCurrentThreadId();
  exceptionInfo.ExceptionPointers = e;
  exceptionInfo.ClientPointers = FALSE;

  MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
                    MINIDUMP_TYPE(MiniDumpWithIndirectlyReferencedMemory |
                                  MiniDumpScanMemory | MiniDumpWithFullMemory),
                    e ? &exceptionInfo : nullptr, nullptr, nullptr);

  if (hFile) {
    CloseHandle(hFile);
    hFile = nullptr;
  }

  LOG_FATAL_WINDOWS("wrote minidump: %s", miniDumpFilename.c_str());
#endif

  if ((e != nullptr) && (e->ExceptionRecord != nullptr)) {
    LOG_FATAL_WINDOWS("Unhandled exception: %d - will crash now.",
                      (int)e->ExceptionRecord->ExceptionCode);
  } else {
    LOG_FATAL_WINDOWS(
        "Unhandled exception without ExceptionCode - will crash now.!");
  }

  return EXCEPTION_CONTINUE_SEARCH;
}
#endif

}

AvocadoGlobalContext* AvocadoGlobalContext::CONTEXT = nullptr;

AvocadoGlobalContext::AvocadoGlobalContext(int argc, char* argv[],
                                         char const* InstallDirectory)
    : _binaryName(TRI_BinaryName(argv[0])),
      _binaryPath(TRI_LocateBinaryPath(argv[0])),
      _runRoot(
          TRI_GetInstallRoot(TRI_LocateBinaryPath(argv[0]), InstallDirectory)),
      _ret(EXIT_FAILURE),
      _useEventLog(true) {
  // allow failing memory allocations for the global context thread (i.e. main program thread)
  TRI_AllowMemoryFailures();

  static char const* serverName = "avocadod";
  if (_binaryName.size() < strlen(serverName) ||
      _binaryName.substr(_binaryName.size() - strlen(serverName)) !=
          serverName) {
    // turn off event-logging for all binaries except avocadod
    // the reason is that all other tools are client tools that will directly
    // print all errors so the user can handle them
    _useEventLog = false;
  }

  ADB_WindowsEntryFunction();

#ifdef _WIN32
  // SetUnhandledExceptionFilter(unhandledExceptionHandler);
#endif

  TRIAGENS_REST_INITIALIZE();
  CONTEXT = this;
}

AvocadoGlobalContext::~AvocadoGlobalContext() {
  CONTEXT = nullptr;

#ifndef _WIN32
  signal(SIGHUP, SIG_IGN);
#endif

  TRIAGENS_REST_SHUTDOWN;
  ADB_WindowsExitFunction(_ret, nullptr);
}

int AvocadoGlobalContext::exit(int ret) {
  _ret = ret;
  return _ret;
}

void AvocadoGlobalContext::installHup() {
#ifndef _WIN32
  signal(SIGHUP, ReopenLog);
#endif
}

void AvocadoGlobalContext::installSegv() { signal(SIGSEGV, abortHandler); }

void AvocadoGlobalContext::maskAllSignals() {
#ifdef TRI_HAVE_POSIX_THREADS
  sigset_t all;
  sigfillset(&all);
  pthread_sigmask(SIG_SETMASK, &all, 0);
#endif
}

void AvocadoGlobalContext::unmaskStandardSignals() {
#ifdef TRI_HAVE_POSIX_THREADS
  sigset_t all;
  sigfillset(&all);
  pthread_sigmask(SIG_UNBLOCK, &all, 0);
#endif
}

void AvocadoGlobalContext::runStartupChecks() {
  // test if this binary uses and stdlib that supports std::regex properly
  if (!supportsStdRegex()) {
    LOG_TOPIC(FATAL, avocadodb::Logger::FIXME) << "the required std::regex functionality required to run "
               << "AvocadoDB is not provided by this build. please try "
               << "rebuilding AvocadoDB in a build environment that properly "
               << "supports std::regex";
    FATAL_ERROR_EXIT();
  }

#ifdef __arm__
  // detect alignment settings for ARM
  {
    LOG_TOPIC(TRACE, avocadodb::Logger::FIXME)
        << "running CPU alignment check";
    // To change the alignment trap behavior, simply echo a number into
    // /proc/cpu/alignment.  The number is made up from various bits:
    //
    // bit             behavior when set
    // ---             -----------------
    //
    // 0               A user process performing an unaligned memory access
    //                 will cause the kernel to print a message indicating
    //                 process name, pid, pc, instruction, address, and the
    //                 fault code.
    //
    // 1               The kernel will attempt to fix up the user process
    //                 performing the unaligned access.  This is of course
    //                 slow (think about the floating point emulator) and
    //                 not recommended for production use.
    //
    // 2               The kernel will send a SIGBUS signal to the user process
    //                 performing the unaligned access.
    bool alignmentDetected = false;

    std::string const filename("/proc/cpu/alignment");
    try {
      std::string const cpuAlignment =
          avocadodb::basics::FileUtils::slurp(filename);
      auto start = cpuAlignment.find("User faults:");

      if (start != std::string::npos) {
        start += strlen("User faults:");
        size_t end = start;
        while (end < cpuAlignment.size()) {
          if (cpuAlignment[end] == ' ' || cpuAlignment[end] == '\t') {
            ++end;
          } else {
            break;
          }
        }
        while (end < cpuAlignment.size()) {
          ++end;
          if (cpuAlignment[end] < '0' || cpuAlignment[end] > '9') {
            break;
          }
        }

        int64_t alignment =
            std::stol(std::string(cpuAlignment.c_str() + start, end - start));
        if ((alignment & 2) == 0) {
          LOG_TOPIC(FATAL, avocadodb::Logger::FIXME)
              << "possibly incompatible CPU alignment settings found in '"
              << filename << "'. this may cause avocadod to abort with "
                             "SIGBUS. please set the value in '"
              << filename << "' to 2";
          FATAL_ERROR_EXIT();
        }

        alignmentDetected = true;
      }

    } catch (...) {
      // ignore that we cannot detect the alignment
      LOG_TOPIC(TRACE, avocadodb::Logger::FIXME)
          << "unable to detect CPU alignment settings. could not process file '"
          << filename << "'";
    }

    if (!alignmentDetected) {
      LOG_TOPIC(WARN, avocadodb::Logger::FIXME)
          << "unable to detect CPU alignment settings. could not process file '"
          << filename
          << "'. this may cause avocadod to abort with SIGBUS. it may be "
             "necessary to set the value in '"
          << filename << "' to 2";
    }
    std::string const proc_cpuinfo_filename("/proc/cpuinfo");
    try {
      std::string const cpuInfo =
          avocadodb::basics::FileUtils::slurp(proc_cpuinfo_filename);
      auto start = cpuInfo.find("ARMv6");

      if (start != std::string::npos) {
          LOG_TOPIC(FATAL, avocadodb::Logger::FIXME)
              << "possibly incompatible ARMv6 CPU detected.";
          FATAL_ERROR_EXIT();
        }
    } catch (...) {
      // ignore that we cannot detect the alignment
      LOG_TOPIC(TRACE, avocadodb::Logger::FIXME)
          << "unable to detect CPU type '"
          << filename << "'";
    }
  }
#endif
}

void AvocadoGlobalContext::createMiniDumpFilename() {
#ifdef _WIN32
  miniDumpFilename = TRI_GetTempPath();

  miniDumpFilename +=
    "\\minidump_" + std::to_string(GetCurrentProcessId()) + ".dmp";
#endif
}

void AvocadoGlobalContext::normalizePath(std::vector<std::string>& paths,
                                        char const* whichPath, bool fatal) {
  for (auto& path : paths) {
    normalizePath(path, whichPath, fatal);
  }
}

void AvocadoGlobalContext::normalizePath(std::string& path,
                                        char const* whichPath, bool fatal) {
  StringUtils::rTrimInPlace(path, TRI_DIR_SEPARATOR_STR);

  if (!avocadodb::basics::FileUtils::exists(path)) {
    std::string directory = avocadodb::basics::FileUtils::buildFilename(_runRoot, path);
    if (!avocadodb::basics::FileUtils::exists(directory)) {
      if (!fatal) {
        return;
      }
      LOG_TOPIC(FATAL, avocadodb::Logger::FIXME) << "failed to locate " << whichPath
                 << " directory, its neither available in '" << path
                 << "' nor in '" << directory << "'";
      FATAL_ERROR_EXIT();
    }
    avocadodb::basics::FileUtils::normalizePath(directory);
    path = directory;
  } else {
    if (!TRI_PathIsAbsolute(path)) {
      avocadodb::basics::FileUtils::makePathAbsolute(path);
    }
  }
}
