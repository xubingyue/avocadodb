/*jshint globalstrict:false, strict:false */
/*global assertEqual, avocado */

////////////////////////////////////////////////////////////////////////////////
/// @brief test the require which is canceled
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2015 AvocadoDB GmbH, Cologne, Germany
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
/// @author Copyright 2015, AvocadoDB GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

var jsunity = require('jsunity');

////////////////////////////////////////////////////////////////////////////////
/// @brief test suite for 'require-canceled'
////////////////////////////////////////////////////////////////////////////////

function RequireCanceledTestSuite() {
  'use strict';

  return {
    setUp() {
        avocado.POST_RAW("/_admin/execute",
          "require('module').globalPaths.unshift(require('path').resolve('./js/common/test-data/modules'));", {
            'x-avocado-v8-context': 0
          });
      },

      tearDown() {
        avocado.POST_RAW("/_admin/execute",
          "require('module').globalPaths.splice(0,1);", {
            'x-avocado-v8-context': 0
          });
      },

      testRequireJson() {
        var internal = require("internal");
        var a = avocado.POST_RAW("/_admin/execute",
          'return Object.keys(require("a"));', {
            'x-avocado-async': "store",
            'x-avocado-v8-context': 0
          });

        internal.sleep(3);

        var id = a.headers['x-avocado-async-id'];
        avocado.PUT_RAW("/_api/job/" + id + "/cancel", '');

        var c = avocado.POST_RAW("/_admin/execute?returnAsJSON=true",
          'return Object.keys(require("a"));', {
            'x-avocado-async': "false",
            'x-avocado-v8-context': 0
          });

        var d;

        try {
          d = JSON.parse(c.body);
        } catch (err) {
          require("internal").print(c.body);
          throw err;
        }

        assertEqual(2, d.length);
      }
  };
}


jsunity.run(RequireCanceledTestSuite);

return jsunity.done();
