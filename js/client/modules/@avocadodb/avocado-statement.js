/* jshint strict: false */

// //////////////////////////////////////////////////////////////////////////////
// / @brief AvocadoStatement
// /
// / @file
// /
// / DISCLAIMER
// /
// / Copyright 2013 triagens GmbH, Cologne, Germany
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
// / @author Achim Brandt
// / @author Dr. Frank Celler
// / @author Copyright 2012-2013, triAGENS GmbH, Cologne, Germany
// //////////////////////////////////////////////////////////////////////////////

var internal = require('internal');
var avocadosh = require('@avocadodb/avocadosh');

var AvocadoStatement = require('@avocadodb/avocado-statement-common').AvocadoStatement;
var AvocadoQueryCursor = require('@avocadodb/avocado-query-cursor').AvocadoQueryCursor;

// //////////////////////////////////////////////////////////////////////////////
// / @brief return a string representation of the statement
// //////////////////////////////////////////////////////////////////////////////

AvocadoStatement.prototype.toString = function () {
  return avocadosh.getIdString(this, 'AvocadoStatement');
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief prints the help for AvocadoStatement
// //////////////////////////////////////////////////////////////////////////////

var helpAvocadoStatement = avocadosh.createHelpHeadline('AvocadoStatement help') +
  'Create an AQL query:                                                    ' + '\n' +
  ' > stmt = new AvocadoStatement(db, { "query": "FOR..." })                ' + '\n' +
  ' > stmt = db._createStatement({ "query": "FOR..." })                    ' + '\n' +
  'Set query options:                                                      ' + '\n' +
  ' > stmt.setBatchSize(<value>)           set the max. number of results  ' + '\n' +
  '                                        to be transferred per roundtrip ' + '\n' +
  ' > stmt.setCount(<value>)               set count flag (return number of' + '\n' +
  '                                        results in "count" attribute)   ' + '\n' +
  'Get query options:                                                      ' + '\n' +
  ' > stmt.setBatchSize()                  return the max. number of results' + '\n' +
  '                                        to be transferred per roundtrip ' + '\n' +
  ' > stmt.getCount()                      return count flag (return number' + '\n' +
  '                                        of results in "count" attribute)' + '\n' +
  ' > stmt.getQuery()                      return query string             ' + '\n' +
  '                                        results in "count" attribute)   ' + '\n' +
  'Bind parameters to a query:                                             ' + '\n' +
  ' > stmt.bind(<key>, <value>)            bind single variable            ' + '\n' +
  ' > stmt.bind(<values>)                  bind multiple variables         ' + '\n' +
  'Execute query:                                                          ' + '\n' +
  ' > cursor = stmt.execute()              returns a cursor                ' + '\n' +
  'Get all results in an array:                                            ' + '\n' +
  ' > docs = cursor.toArray()                                              ' + '\n' +
  'Or loop over the result set:                                            ' + '\n' +
  ' > while (cursor.hasNext()) { print(cursor.next()) }                    ';

AvocadoStatement.prototype._help = function () {
  internal.print(helpAvocadoStatement);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief parse a query and return the results
// //////////////////////////////////////////////////////////////////////////////

AvocadoStatement.prototype.parse = function () {
  var body = {
    query: this._query
  };

  var requestResult = this._database._connection.POST(
    '/_api/query',
    JSON.stringify(body));

  avocadosh.checkRequestResult(requestResult);

  var result = {
    bindVars: requestResult.bindVars,
    collections: requestResult.collections,
    ast: requestResult.ast
  };
  return result;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief explain a query and return the results
// //////////////////////////////////////////////////////////////////////////////

AvocadoStatement.prototype.explain = function (options) {
  var opts = this._options || { };
  if (typeof opts === 'object' && typeof options === 'object') {
    Object.keys(options).forEach(function (o) {
      // copy options
      opts[o] = options[o];
    });
  }

  var body = {
    query: this._query,
    bindVars: this._bindVars,
    options: opts
  };

  var requestResult = this._database._connection.POST(
    '/_api/explain',
    JSON.stringify(body));

  avocadosh.checkRequestResult(requestResult);

  if (opts && opts.allPlans) {
    return {
      plans: requestResult.plans,
      warnings: requestResult.warnings,
      stats: requestResult.stats
    };
  } else {
    return {
      plan: requestResult.plan,
      warnings: requestResult.warnings,
      stats: requestResult.stats,
      cacheable: requestResult.cacheable
    };
  }
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief execute the query
// /
// / Invoking execute() will transfer the query and all bind parameters to the
// / server. It will return a cursor with the query results in case of success.
// / In case of an error, the error will be printed
// //////////////////////////////////////////////////////////////////////////////

AvocadoStatement.prototype.execute = function () {
  var body = {
    query: this._query,
    count: this._doCount,
    bindVars: this._bindVars
  };

  if (this._batchSize) {
    body.batchSize = this._batchSize;
  }

  if (this._options) {
    body.options = this._options;
  }

  if (this._cache !== undefined) {
    body.cache = this._cache;
  }

  var requestResult = this._database._connection.POST(
    '/_api/cursor',
    JSON.stringify(body));

  avocadosh.checkRequestResult(requestResult);

  return new AvocadoQueryCursor(this._database, requestResult);
};

exports.AvocadoStatement = AvocadoStatement;
