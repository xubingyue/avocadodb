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
/// @author Dr. Frank Celler
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_BASICS_TERMINAL__UTILS_H
#define ARANGODB_BASICS_TERMINAL__UTILS_H 1

#include "Basics/Common.h"

#ifdef TRI_HAVE_TERMIOS_H
#include <termios.h>
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief default value for number of columns of a terminal
////////////////////////////////////////////////////////////////////////////////

#define TRI_DEFAULT_COLUMNS (80)

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the columns width
////////////////////////////////////////////////////////////////////////////////

int TRI_ColumnsWidth(void);

////////////////////////////////////////////////////////////////////////////////
/// @brief set the visibility of stdin inputs (turn off for password entry etc.)
////////////////////////////////////////////////////////////////////////////////

void TRI_SetStdinVisibility(bool);

#endif