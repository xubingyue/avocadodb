/*jshint globalstrict:false, strict:false, strict: false, maxlen: 500 */
/*global assertEqual */

////////////////////////////////////////////////////////////////////////////////
/// @brief tests for query language, variables
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2010-2012 triagens GmbH, Cologne, Germany
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
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Jan Steemann
/// @author Copyright 2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

var errors = require("internal").errors;
var jsunity = require("jsunity");
var helper = require("@avocadodb/aql-helper");
var getQueryResults = helper.getQueryResults;
var assertQueryError = helper.assertQueryError;

////////////////////////////////////////////////////////////////////////////////
/// @brief test suite
////////////////////////////////////////////////////////////////////////////////

function ahuacatlVariablesTestSuite () {

  return {

////////////////////////////////////////////////////////////////////////////////
/// @brief set up
////////////////////////////////////////////////////////////////////////////////

    setUp : function () {
    },

////////////////////////////////////////////////////////////////////////////////
/// @brief tear down
////////////////////////////////////////////////////////////////////////////////

    tearDown : function () {
    },

////////////////////////////////////////////////////////////////////////////////
/// @brief test valid declaration
////////////////////////////////////////////////////////////////////////////////

    testValid1 : function () {
      var result = getQueryResults("LET a = 1 RETURN a");

      assertEqual([ 1 ], result);
    },

////////////////////////////////////////////////////////////////////////////////
/// @brief test valid declaration
////////////////////////////////////////////////////////////////////////////////

    testValid2 : function () {
      var result = getQueryResults("LET a = 1 LET b = 2 RETURN [ a, b ]");

      assertEqual([ [ 1, 2 ] ], result);
    },

////////////////////////////////////////////////////////////////////////////////
/// @brief test valid declaration
////////////////////////////////////////////////////////////////////////////////

    testValid3 : function () {
      var result = getQueryResults("LET `a b c` = 1 RETURN `a b c`");

      assertEqual([ 1 ], result);
    },

////////////////////////////////////////////////////////////////////////////////
/// @brief test undeclared
////////////////////////////////////////////////////////////////////////////////

    testUndeclared : function () {
      assertQueryError(errors.ERROR_AVOCADO_COLLECTION_NOT_FOUND.code, "LET a = nonexisting + 1 RETURN 0"); 
    },

////////////////////////////////////////////////////////////////////////////////
/// @brief test redeclaration
////////////////////////////////////////////////////////////////////////////////

    testRedeclare : function () {
      assertQueryError(errors.ERROR_QUERY_VARIABLE_REDECLARED.code, "LET a = 1 FOR a IN [ 1 ] RETURN 0"); 
    }

  };
}

////////////////////////////////////////////////////////////////////////////////
/// @brief executes the test suite
////////////////////////////////////////////////////////////////////////////////

jsunity.run(ahuacatlVariablesTestSuite);

return jsunity.done();

