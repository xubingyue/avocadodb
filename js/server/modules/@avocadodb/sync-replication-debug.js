/* jshint strict: false */
/* global AvocadoServerState */

// //////////////////////////////////////////////////////////////////////////////
// / DISCLAIMER
// /
// / Copyright 2017 ArangoDB GmbH, Cologne, Germany
// /
// / Licensed under the Apache License, Version 2.0 (the "License")
// / you may not use this file except in compliance with the License.
// / You may obtain a copy of the License at
// /
// /     http://www.apache.org/licenses/LICENSE-2.0
// /
// / Unless required by applicable law or agreed to in writing, software
// / distributed under the License is distributed on an "AS IS" BASIS,
// / WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// / See the License for the specific language governing permissions and
// / limitations under the License.
// /
// / Copyright holder is triAGENS GmbH, Cologne, Germany
// /
// / @author Andreas Streichardt
// //////////////////////////////////////////////////////////////////////////////

exports.setup = function() {
  global.AvocadoServerState.enableSyncReplicationDebug();
  AvocadoServerState.setRole('PRIMARY');
  global.AvocadoAgency.set = function() { return true; };
  global.AvocadoAgency.write = function() { return true; };
  global.AvocadoAgency.increaseVersion = function() { return true; };
  global.AvocadoAgency.get = function(path) {
    let value = {};
    let pathSegments = path.split('/');
    let keyValue = 1;
    value.avocado = pathSegments.reverse().reduce((v, key) => {
      let kv = {};
      kv[key] = v;
      return kv;
    }, keyValue);
    return value;
  };
};
