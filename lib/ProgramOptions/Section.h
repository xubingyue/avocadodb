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
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADODB_PROGRAM_OPTIONS_SECTION_H
#define AVOCADODB_PROGRAM_OPTIONS_SECTION_H 1

#include "Basics/Common.h"
#include "ProgramOptions/Option.h"

namespace avocadodb {
namespace options {

// a single program options section
struct Section {
  // sections are default copy-constructible and default movable

  Section(std::string const& name, std::string const& description,
          std::string const& alias, bool hidden, bool obsolete)
      : name(name),
        description(description),
        alias(alias),
        hidden(hidden),
        obsolete(obsolete) {}

  // adds a program option to the section
  void addOption(Option const& option);

  // get display name for the section
  std::string displayName() const { return alias.empty() ? name : alias; }

  // whether or not the section has (displayable) options
  bool hasOptions() const;

  // print help for a section
  // the special search string "." will show help for all sections, even if hidden
  void printHelp(std::string const& search, size_t tw, size_t ow, bool colors) const;

  // determine display width for a section
  size_t optionsWidth() const;

  std::string name;
  std::string description;
  std::string alias;
  bool hidden;
  bool obsolete;

  // program options of the section
  std::map<std::string, Option> options;
};
}
}

#endif
