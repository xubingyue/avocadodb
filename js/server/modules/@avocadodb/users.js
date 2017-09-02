/* jshint strict: false */
/* global AvocadoAgency */

'use strict';

////////////////////////////////////////////////////////////////////////////////
// @brief User management
//
// @file
//
// DISCLAIMER
//
// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
//
// Licensed under the Apache License, Version 2.0 (the "License")
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright holder is triAGENS GmbH, Cologne, Germany
//
// @author Jan Steemann
// @author Simon Grätzer
// @author Copyright 2012-2017, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

const internal = require('internal'); // OK: reloadAuth
const avocadodb = require("@avocadodb");

// //////////////////////////////////////////////////////////////////////////////
// / @brief constructor
// //////////////////////////////////////////////////////////////////////////////

var AvocadoUsers = internal.AvocadoUsers;
exports.save = AvocadoUsers.save;
exports.replace = AvocadoUsers.replace;
exports.update = AvocadoUsers.update;
exports.remove = AvocadoUsers.remove;
exports.document = AvocadoUsers.document;
exports.reload = AvocadoUsers.reload;
exports.grantDatabase = AvocadoUsers.grantDatabase;
exports.revokeDatabase = AvocadoUsers.revokeDatabase;
exports.grantCollection = AvocadoUsers.grantCollection;
exports.revokeCollection = AvocadoUsers.revokeCollection;
exports.updateConfigData = AvocadoUsers.updateConfigData;
exports.revokeCollection = AvocadoUsers.revokeCollection;
exports.updateConfigData = AvocadoUsers.updateConfigData;
exports.configData = AvocadoUsers.configData;
exports.permission = AvocadoUsers.permission;
exports.currentUser = AvocadoUsers.currentUser;
exports.isAuthActive = AvocadoUsers.isAuthActive;
exports.exists = function (username) {
  try {
    exports.document(username);
    return true;
  } catch (e) {
    if (e.errorNum === avocadodb.errors.ERROR_USER_NOT_FOUND.code) {
      return false;
    }
    throw e;
  }
};