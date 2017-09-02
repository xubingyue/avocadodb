/* jshint strict: false, unused: false */
/* global TRANSACTION, AQL_PARSE */

// //////////////////////////////////////////////////////////////////////////////
// / @brief AvocadoDatabase
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

module.isSystem = true;

var internal = require('internal');

// //////////////////////////////////////////////////////////////////////////////
// / @brief constructor
// //////////////////////////////////////////////////////////////////////////////

exports.AvocadoDatabase = internal.AvocadoDatabase;

var AvocadoDatabase = exports.AvocadoDatabase;

// must be called after export
var AvocadoCollection = require('@avocadodb/avocado-collection').AvocadoCollection;
var AvocadoView = require('@avocadodb/avocado-view').AvocadoView;
var AvocadoError = require('@avocadodb').AvocadoError;
var AvocadoStatement = require('@avocadodb/avocado-statement').AvocadoStatement;

// //////////////////////////////////////////////////////////////////////////////
// / @brief prints a database
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._PRINT = function (context) {
  context.output += this.toString();
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief strng representation of a database
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype.toString = function (seen, path, names, level) {
  return '[AvocadoDatabase "' + this._name() + '"]';
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief factory method to create a new statement
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._createStatement = function (data) {
  return new AvocadoStatement(this, data);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief factory method to create and execute a new statement
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._query = function (query, bindVars, cursorOptions, options) {
  if (typeof query === 'object' && query !== null && arguments.length === 1) {
    return new AvocadoStatement(this, query).execute();
  }
  if (options === undefined && cursorOptions !== undefined) {
    options = cursorOptions;
  }

  var payload = {
    query: query,
    bindVars: bindVars || undefined,
    count: (cursorOptions && cursorOptions.count) || false,
    batchSize: (cursorOptions && cursorOptions.batchSize) || undefined,
    options: options || undefined,
    cache: (options && options.cache) || undefined
  };
  return new AvocadoStatement(this, payload).execute();
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief explains a query
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._explain = function (query, bindVars, options) {
  if (typeof query === 'object' && typeof query.toAQL === 'function') {
    query = { query: query.toAQL() };
  }

  if (arguments.length > 1) {
    query = { query: query, bindVars: bindVars, options: options };
  }

  require('@avocadodb/aql/explainer').explain(query);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief parses a query
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._parse = function (query) {
  if (typeof query === 'object' && typeof query.toAQL === 'function') {
    query = query.toAQL();
  }

  return AQL_PARSE(query);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief was docuBlock executeTransaction
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._executeTransaction = function (data) {
  return TRANSACTION(data);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief was docuBlock collectionDatabaseDrop
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._drop = function (name, options) {
  var collection = name;

  if (!(name instanceof AvocadoCollection)) {
    collection = internal.db._collection(name);
  }

  if (collection === null) {
    return;
  }

  try {
    return collection.drop(options);
  } catch (err) {
    // ignore if the collection does not exist
    if (err instanceof AvocadoError &&
      err.errorNum === internal.errors.ERROR_AVOCADO_COLLECTION_NOT_FOUND.code) {
      return;
    }
    // rethrow exception
    throw err;
  }
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief was docuBlock collectionDatabaseTruncate
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._truncate = function (name) {
  var collection = name;

  if (!(name instanceof AvocadoCollection)) {
    collection = internal.db._collection(name);
  }

  if (collection === null) {
    return;
  }

  collection.truncate();
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief index id regex
// //////////////////////////////////////////////////////////////////////////////

// //////////////////////////////////////////////////////////////////////////////
// / @brief was docuBlock IndexVerify
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.indexRegex = /^([a-zA-Z0-9\-_]+)\/([0-9]+)$/;

// //////////////////////////////////////////////////////////////////////////////
// / @brief was docuBlock IndexHandle
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._index = function (id) {
  if (id.hasOwnProperty('id')) {
    id = id.id;
  }

  var pa = AvocadoDatabase.indexRegex.exec(id);
  var err;

  if (pa === null) {
    err = new AvocadoError();
    err.errorNum = internal.errors.ERROR_AVOCADO_INDEX_HANDLE_BAD.code;
    err.errorMessage = internal.errors.ERROR_AVOCADO_INDEX_HANDLE_BAD.message;
    throw err;
  }

  var col = this._collection(pa[1]);

  if (col === null) {
    err = new AvocadoError();
    err.errorNum = internal.errors.ERROR_AVOCADO_COLLECTION_NOT_FOUND.code;
    err.errorMessage = internal.errors.ERROR_AVOCADO_COLLECTION_NOT_FOUND.message;
    throw err;
  }

  var indexes = col.getIndexes();
  var i;

  for (i = 0;  i < indexes.length;  ++i) {
    var index = indexes[i];

    if (index.id === id) {
      return index;
    }
  }

  return null;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief was docuBlock dropIndex
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._dropIndex = function (id) {
  if (id.hasOwnProperty('id')) {
    id = id.id;
  }

  var pa = AvocadoDatabase.indexRegex.exec(id);
  var err;

  if (pa === null) {
    err = new AvocadoError();
    err.errorNum = internal.errors.ERROR_AVOCADO_INDEX_HANDLE_BAD.code;
    err.errorMessage = internal.errors.ERROR_AVOCADO_INDEX_HANDLE_BAD.message;
    throw err;
  }

  var col = this._collection(pa[1]);

  if (col === null) {
    err = new AvocadoError();
    err.errorNum = internal.errors.ERROR_AVOCADO_COLLECTION_NOT_FOUND.code;
    err.errorMessage = internal.errors.ERROR_AVOCADO_COLLECTION_NOT_FOUND.message;
    throw err;
  }

  return col.dropIndex(id);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief was docuBlock endpoints
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._endpoints = function () {
  return internal._endpoints();
};
