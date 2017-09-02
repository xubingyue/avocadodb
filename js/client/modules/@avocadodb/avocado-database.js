/* jshint strict: false */

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

var internal = require('internal');
var avocadosh = require('@avocadodb/avocadosh');

// //////////////////////////////////////////////////////////////////////////////
// / @brief constructor
// //////////////////////////////////////////////////////////////////////////////

var AvocadoCollection;
var AvocadoView;

function AvocadoDatabase (connection) {
  this._connection = connection;
  this._collectionConstructor = AvocadoCollection;
  this._viewConstructor = AvocadoView;
  this._properties = null;

  this._registerCollection = function (name, obj) {
    // store the collection in our own list
    this[name] = obj;
  };

  this._viewList = {};
  this._registerView = function (name, obj) {
    // store the view in our own list
    this._viewList[name] = obj;
  };
  this._unregisterView = function(name) {
    if (this._viewList[name] !== undefined) {
      delete this._viewList[name];
    }
  };
}

exports.AvocadoDatabase = AvocadoDatabase;

// load after exporting AvocadoDatabase
AvocadoCollection = require('@avocadodb/avocado-collection').AvocadoCollection;
AvocadoView = require('@avocadodb/avocado-view').AvocadoView;
var AvocadoError = require('@avocadodb').AvocadoError;
var AvocadoStatement = require('@avocadodb/avocado-statement').AvocadoStatement;

// //////////////////////////////////////////////////////////////////////////////
// / @brief index id regex
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.indexRegex = /^([a-zA-Z0-9\-_]+)\/([0-9]+)$/;

// //////////////////////////////////////////////////////////////////////////////
// / @brief key regex
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.keyRegex = /^([a-zA-Z0-9_:\-@\.\(\)\+,=;\$!\*'%])+$/;

// //////////////////////////////////////////////////////////////////////////////
// / @brief append the waitForSync parameter to a URL
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._appendSyncParameter = function (url, waitForSync) {
  if (waitForSync) {
    if (url.indexOf('?') === -1) {
      url += '?';
    } else {
      url += '&';
    }
    url += 'waitForSync=true';
  }
  return url;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief append some boolean parameter to a URL
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._appendBoolParameter = function (url, name, val) {
  if (url.indexOf('?') === -1) {
    url += '?';
  } else {
    url += '&';
  }
  url += name + (val ? '=true' : '=false');
  return url;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief return the base url for collection usage
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._collectionurl = function (id) {
  if (id === undefined) {
    return '/_api/collection';
  }

  return '/_api/collection/' + encodeURIComponent(id);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief return the base url for view usage
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._viewurl = function (id) {
  if (id === undefined) {
    return '/_api/view';
  }

  return '/_api/view/' + encodeURIComponent(id);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief return the base url for document usage
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._documenturl = function (id, expectedName) {
  var s = id.split('/');

  if (s.length !== 2) {
    throw new AvocadoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_ARANGO_DOCUMENT_HANDLE_BAD.code,
      errorMessage: internal.errors.ERROR_ARANGO_DOCUMENT_HANDLE_BAD.message
    });
  } else if (expectedName !== undefined && expectedName !== '' && s[0] !== expectedName) {
    throw new AvocadoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_ARANGO_CROSS_COLLECTION_REQUEST.code,
      errorMessage: internal.errors.ERROR_ARANGO_CROSS_COLLECTION_REQUEST.message
    });
  }

  if (AvocadoDatabase.keyRegex.exec(s[1]) === null) {
    throw new AvocadoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_ARANGO_DOCUMENT_HANDLE_BAD.code,
      errorMessage: internal.errors.ERROR_ARANGO_DOCUMENT_HANDLE_BAD.message
    });
  }

  return '/_api/document/' + encodeURIComponent(s[0]) + '/' + encodeURIComponent(s[1]);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief return the base url for index usage
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._indexurl = function (id, expectedName) {
  if (typeof id === 'string') {
    var pa = AvocadoDatabase.indexRegex.exec(id);

    if (pa === null && expectedName !== undefined) {
      id = expectedName + '/' + id;
    }
  } else if (typeof id === 'number' && expectedName !== undefined) {
    // stringify a numeric id
    id = expectedName + '/' + id;
  }

  var s = id.split('/');

  if (s.length !== 2) {
    // invalid index handle
    throw new AvocadoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_ARANGO_INDEX_HANDLE_BAD.code,
      errorMessage: internal.errors.ERROR_ARANGO_INDEX_HANDLE_BAD.message
    });
  } else if (expectedName !== undefined && expectedName !== '' && s[0] !== expectedName) {
    // index handle does not match collection name
    throw new AvocadoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_ARANGO_CROSS_COLLECTION_REQUEST.code,
      errorMessage: internal.errors.ERROR_ARANGO_CROSS_COLLECTION_REQUEST.message
    });
  }

  return '/_api/index/' + encodeURIComponent(s[0]) + '/' + encodeURIComponent(s[1]);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief prints the help for AvocadoDatabase
// //////////////////////////////////////////////////////////////////////////////

var helpAvocadoDatabase = avocadosh.createHelpHeadline('AvocadoDatabase (db) help') +
  'Administration Functions:                                                 ' + '\n' +
  '  _help()                               this help                         ' + '\n' +
  '  _flushCache()                         flush and refill collection cache ' + '\n' +
  '                                                                          ' + '\n' +
  'Collection Functions:                                                     ' + '\n' +
  '  _collections()                        list all collections              ' + '\n' +
  '  _collection(<name>)                   get collection by identifier/name ' + '\n' +
  '  _create(<name>, <properties>)         creates a new collection          ' + '\n' +
  '  _createEdgeCollection(<name>)         creates a new edge collection     ' + '\n' +
  '  _drop(<name>)                         delete a collection               ' + '\n' +
  '                                                                          ' + '\n' +
  'Document Functions:                                                       ' + '\n' +
  '  _document(<id>)                       get document by handle (_id)      ' + '\n' +
  '  _replace(<id>, <data>, <overwrite>)   overwrite document                ' + '\n' +
  '  _update(<id>, <data>, <overwrite>,    partially update document         ' + '\n' +
  '          <keepNull>)                                                     ' + '\n' +
  '  _remove(<id>)                         delete document                   ' + '\n' +
  '  _exists(<id>)                         checks whether a document exists  ' + '\n' +
  '  _truncate()                           delete all documents              ' + '\n' +
  '                                                                          ' + '\n' +
  'Database Management Functions:                                            ' + '\n' +
  '  _createDatabase(<name>)               creates a new database            ' + '\n' +
  '  _dropDatabase(<name>)                 drops an existing database        ' + '\n' +
  '  _useDatabase(<name>)                  switches into an existing database' + '\n' +
  '  _drop(<name>)                         delete a collection               ' + '\n' +
  '  _name()                               name of the current database      ' + '\n' +
  '                                                                          ' + '\n' +
  'Query / Transaction Functions:                                            ' + '\n' +
  '  _executeTransaction(<transaction>)    execute transaction               ' + '\n' +
  '  _query(<query>)                       execute AQL query                 ' + '\n' +
  '  _createStatement(<data>)              create and return AQL query       ' + '\n' +
  '                                                                          ' + '\n' +
  'View Functions:                                                           ' + '\n' +
  '  _views()                                  list all views                ' + '\n' +
  '  _view(<name>)                             get view by name              ' + '\n' +
  '  _createView(<name>, <type>, <properties>) creates a new view            ';

AvocadoDatabase.prototype._help = function () {
  internal.print(helpAvocadoDatabase);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief return a string representation of the database object
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype.toString = function () {
  return '[object AvocadoDatabase "' + this._name() + '"]';
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief return all collections from the database
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._collections = function () {
  var requestResult = this._connection.GET(this._collectionurl());

  avocadosh.checkRequestResult(requestResult);

  if (requestResult.result !== undefined) {
    var collections = requestResult.result;
    var result = [];
    var i;

    // add all collentions to object
    for (i = 0;  i < collections.length;  ++i) {
      var collection = new this._collectionConstructor(this, collections[i]);
      this._registerCollection(collection._name, collection);
      result.push(collection);
    }

    return result.sort(function (l, r) {
      // we assume no two collections have the same name
      if (l.name().toLowerCase() < r.name().toLowerCase()) {
        return -1;
      }
      return 1;
    });
  }

  return undefined;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief return a single collection, identified by its id or name
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._collection = function (id) {
  if (typeof id !== 'number' &&
      this[id] && this[id] instanceof this._collectionConstructor) {
    return this[id];
  }
  var url;

  if (typeof id === 'number') {
    url = this._collectionurl(id) + '?useId=true';
  } else {
    url = this._collectionurl(id);
  }

  var requestResult = this._connection.GET(url);

  // return null in case of not found
  if (requestResult !== null
    && requestResult.error === true
    && requestResult.errorNum === internal.errors.ERROR_ARANGO_COLLECTION_NOT_FOUND.code) {
    return null;
  }

  // check all other errors and throw them
  avocadosh.checkRequestResult(requestResult);

  var name = requestResult.name;

  if (name !== undefined) {
    this._registerCollection(name, new this._collectionConstructor(this, requestResult));
    return this[name];
  }

  return null;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief creates a new collection
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._create = function (name, properties, type, options) {
  var body = {
    'name': name,
    'type': AvocadoCollection.TYPE_DOCUMENT
  };

  if (properties !== undefined) {
    [ 'waitForSync', 'journalSize', 'isSystem', 'isVolatile',
      'doCompact', 'keyOptions', 'shardKeys', 'numberOfShards',
      'distributeShardsLike', 'indexBuckets', 'id', 'isSmart',
      'replicationFactor', 'smartGraphAttribute', 'avoidServers'].forEach(function (p) {
      if (properties.hasOwnProperty(p)) {
        body[p] = properties[p];
      }
    });
  }
  
  let urlAddon = '';
  if (typeof options === "object" && options !== null) {
    if (options.hasOwnProperty('waitForSyncReplication')) {
      if (options.waitForSyncReplication) {
        urlAddon = '?waitForSyncReplication=1';
      } else {
        urlAddon = '?waitForSyncReplication=0';
      }
    }
  }

  if (type !== undefined) {
    body.type = type;
  }

  var requestResult = this._connection.POST(this._collectionurl() + urlAddon,
    JSON.stringify(body));

  avocadosh.checkRequestResult(requestResult);

  var nname = requestResult.name;

  if (nname !== undefined) {
    this._registerCollection(nname, new this._collectionConstructor(this, requestResult));
    return this[nname];
  }

  return undefined;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief creates a new document collection
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._createDocumentCollection = function (name, properties) {
  return this._create(name, properties, AvocadoCollection.TYPE_DOCUMENT);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief creates a new edges collection
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._createEdgeCollection = function (name, properties) {
  return this._create(name, properties, AvocadoCollection.TYPE_EDGE);
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief truncates a collection
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._truncate = function (id) {
  var name;

  if (typeof id !== 'string') {
    id = id._id;
  }

  for (name in this) {
    if (this.hasOwnProperty(name)) {
      var collection = this[name];

      if (collection instanceof this._collectionConstructor) {
        if (collection._id === id || collection._name === id) {
          return collection.truncate();
        }
      }
    }
  }

  return undefined;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief drops a collection
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._drop = function (id, options) {
  var name;

  for (name in this) {
    if (this.hasOwnProperty(name)) {
      var collection = this[name];

      if (collection instanceof this._collectionConstructor) {
        if (collection._id === id || collection._name === id) {
          return collection.drop(options);
        }
      }
    }
  }

  var c = this._collection(id);
  if (c) {
    return c.drop(options);
  }
  return undefined;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief flush the local cache
// / this is called by connection.reconnect()
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._flushCache = function () {
  var name;

  for (name in this) {
    if (this.hasOwnProperty(name)) {
      var collection = this[name];

      if (collection instanceof this._collectionConstructor) {
        // reset the collection status
        collection._status = null;
        this[name] = undefined;
      }
    }
  }

  try {
    // repopulate cache
    this._collections();
  } catch (err) {}

  this._properties = null;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief query the database properties
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._queryProperties = function (force) {
  if (force || this._properties === null) {
    var url = '/_api/database/current';
    var requestResult = this._connection.GET(url);

    avocadosh.checkRequestResult(requestResult);
    this._properties = requestResult.result;
  }

  return this._properties;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief return the database id
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._id = function () {
  return this._queryProperties().id;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief return whether or not the current database is the system database
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._isSystem = function () {
  return this._queryProperties().isSystem;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief get the name of the current database
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._name = function () {
  return this._queryProperties().name;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief get the path of the current database
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._path = function () {
  return this._queryProperties().path;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief returns one index
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._index = function (id) {
  if (id.hasOwnProperty('id')) {
    id = id.id;
  }

  var requestResult = this._connection.GET(this._indexurl(id));

  avocadosh.checkRequestResult(requestResult);

  return requestResult;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief deletes one index
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._dropIndex = function (id) {
  if (id.hasOwnProperty('id')) {
    id = id.id;
  }

  var requestResult = this._connection.DELETE(this._indexurl(id));

  if (requestResult !== null
    && requestResult.error === true
    && requestResult.errorNum === internal.errors.ERROR_ARANGO_INDEX_NOT_FOUND.code) {
    return false;
  }

  avocadosh.checkRequestResult(requestResult);

  return true;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief returns the engine name
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._engine = function () {
  var requestResult = this._connection.GET('/_api/engine');

  avocadosh.checkRequestResult(requestResult);

  return requestResult;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief returns the engine statistics
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._engineStats = function () {
  var requestResult = this._connection.GET('/_api/engine/stats');

  avocadosh.checkRequestResult(requestResult);

  return requestResult;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief returns the database version
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._version = function (details) {
  var requestResult = this._connection.GET('/_api/version' +
                        (details ? '?details=true' : ''));

  avocadosh.checkRequestResult(requestResult);

  return details ? requestResult : requestResult.version;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief return a single document from the collection, identified by its id
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._document = function (id) {
  var rev = null;
  var requestResult;

  if (typeof id === 'object') {
    if (id.hasOwnProperty('_rev')) {
      rev = id._rev;
    }
    if (id.hasOwnProperty('_id')) {
      id = id._id;
    }
  }

  if (rev === null) {
    requestResult = this._connection.GET(this._documenturl(id));
  } else {
    requestResult = this._connection.GET(this._documenturl(id),
      {'if-match': JSON.stringify(rev) });
  }

  if (requestResult !== null && requestResult.error === true) {
    if (requestResult.errorNum === internal.errors.ERROR_HTTP_PRECONDITION_FAILED.code) {
      requestResult.errorNum = internal.errors.ERROR_ARANGO_CONFLICT.code;
    }
  }

  avocadosh.checkRequestResult(requestResult);

  return requestResult;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief checks whether a document exists, identified by its id
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._exists = function (id) {
  var rev = null;
  var requestResult;

  if (typeof id === 'object') {
    if (id.hasOwnProperty('_rev')) {
      rev = id._rev;
    }
    if (id.hasOwnProperty('_id')) {
      id = id._id;
    }
  }

  if (rev === null) {
    requestResult = this._connection.HEAD(this._documenturl(id));
  } else {
    requestResult = this._connection.HEAD(this._documenturl(id),
      {'if-match': JSON.stringify(rev) });
  }

  if (requestResult !== null && requestResult.error === true) {
    if (requestResult.errorNum === internal.errors.ERROR_HTTP_NOT_FOUND.code) {
      return false;
    }
    if (requestResult.errorNum === internal.errors.ERROR_HTTP_PRECONDITION_FAILED.code) {
      requestResult.errorNum = internal.errors.ERROR_ARANGO_CONFLICT.code;
    }
  }

  avocadosh.checkRequestResult(requestResult);

  return true;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief delete a document in the collection, identified by its id
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._remove = function (id, overwrite, waitForSync) {
  var rev = null;
  var requestResult;

  if (typeof id === 'object') {
    if (Array.isArray(id)) {
      throw new AvocadoError({
        error: true,
        code: internal.errors.ERROR_ARANGO_DOCUMENT_HANDLE_BAD.code,
        errorNum: internal.errors.ERROR_ARANGO_DOCUMENT_HANDLE_BAD.code,
        errorMessage: internal.errors.ERROR_ARANGO_DOCUMENT_HANDLE_BAD.message
      });
    }
    if (id.hasOwnProperty('_rev')) {
      rev = id._rev;
    }
    if (id.hasOwnProperty('_id')) {
      id = id._id;
    }
  }

  var params = '';
  var ignoreRevs = false;
  var options;

  if (typeof overwrite === 'object') {
    if (typeof waitForSync !== 'undefined') {
      throw 'too many arguments';
    }
    // we assume the caller uses new signature (id, data, options)
    options = overwrite;
    if (options.hasOwnProperty('overwrite') && options.overwrite) {
      ignoreRevs = true;
    }
    if (options.hasOwnProperty('waitForSync')) {
      waitForSync = options.waitForSync;
    }
  } else {
    if (overwrite) {
      ignoreRevs = true;
    }
    options = {};
  }

  var url = this._documenturl(id) + params;
  url = this._appendSyncParameter(url, waitForSync);
  url = this._appendBoolParameter(url, 'ignoreRevs', ignoreRevs);
  url = this._appendBoolParameter(url, 'returnOld', options.returnOld);

  if (rev === null || ignoreRevs) {
    requestResult = this._connection.DELETE(url);
  } else {
    requestResult = this._connection.DELETE(url,
      {'if-match': JSON.stringify(rev) });
  }

  if (requestResult !== null && requestResult.error === true) {
    if (requestResult.errorNum === internal.errors.ERROR_HTTP_PRECONDITION_FAILED.code) {
      requestResult.errorNum = internal.errors.ERROR_ARANGO_CONFLICT.code;
    }
  }

  avocadosh.checkRequestResult(requestResult);

  return options.silent ? true : requestResult;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief replace a document in the collection, identified by its id
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._replace = function (id, data, overwrite, waitForSync) {
  var rev = null;
  var requestResult;

  if (typeof id === 'object') {
    if (Array.isArray(id)) {
      throw new AvocadoError({
        error: true,
        code: internal.errors.ERROR_ARANGO_DOCUMENT_TYPE_INVALID.code,
        errorNum: internal.errors.ERROR_ARANGO_DOCUMENT_TYPE_INVALID.code,
        errorMessage: internal.errors.ERROR_ARANGO_DOCUMENT_TYPE_INVALID.message
      });
    }
    if (id.hasOwnProperty('_rev')) {
      rev = id._rev;
    }
    if (id.hasOwnProperty('_id')) {
      id = id._id;
    }
  }

  var params = '';
  var ignoreRevs = false;
  var options;

  if (typeof overwrite === 'object') {
    if (typeof waitForSync !== 'undefined') {
      throw 'too many arguments';
    }
    // we assume the caller uses new signature (id, data, options)
    options = overwrite;
    if (options.hasOwnProperty('overwrite') && options.overwrite) {
      ignoreRevs = true;
    }
    if (options.hasOwnProperty('waitForSync')) {
      waitForSync = options.waitForSync;
    }
  } else {
    if (overwrite) {
      ignoreRevs = true;
    }
    options = {};
  }
  var url = this._documenturl(id) + params;
  url = this._appendSyncParameter(url, waitForSync);
  url = this._appendBoolParameter(url, 'ignoreRevs', true);
  url = this._appendBoolParameter(url, 'returnOld', options.returnOld);
  url = this._appendBoolParameter(url, 'returnNew', options.returnNew);

  if (rev === null || ignoreRevs) {
    requestResult = this._connection.PUT(url, JSON.stringify(data));
  } else {
    requestResult = this._connection.PUT(url, JSON.stringify(data),
      {'if-match': JSON.stringify(rev) });
  }

  if (requestResult !== null && requestResult.error === true) {
    if (requestResult.errorNum === internal.errors.ERROR_HTTP_PRECONDITION_FAILED.code) {
      requestResult.errorNum = internal.errors.ERROR_ARANGO_CONFLICT.code;
    }
  }

  avocadosh.checkRequestResult(requestResult);

  return options.silent ? true : requestResult;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief update a document in the collection, identified by its id
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._update = function (id, data, overwrite, keepNull, waitForSync) {
  var rev = null;
  var requestResult;

  if (typeof id === 'object') {
    if (Array.isArray(id)) {
      throw new AvocadoError({
        error: true,
        code: internal.errors.ERROR_ARANGO_DOCUMENT_TYPE_INVALID.code,
        errorNum: internal.errors.ERROR_ARANGO_DOCUMENT_TYPE_INVALID.code,
        errorMessage: internal.errors.ERROR_ARANGO_DOCUMENT_TYPE_INVALID.message
      });
    }
    if (id.hasOwnProperty('_rev')) {
      rev = id._rev;
    }
    if (id.hasOwnProperty('_id')) {
      id = id._id;
    }
  }

  var params = '';
  var ignoreRevs = false;
  var options;
  if (typeof overwrite === 'object') {
    if (typeof keepNull !== 'undefined') {
      throw 'too many arguments';
    }
    // we assume the caller uses new signature (id, data, options)
    options = overwrite;
    if (!options.hasOwnProperty('keepNull')) {
      options.keepNull = true;
    }
    params = '?keepNull=' + options.keepNull;
    if (!options.hasOwnProperty('mergeObjects')) {
      options.mergeObjects = true;
    }
    params += '&mergeObjects=' + options.mergeObjects;

    if (options.hasOwnProperty('overwrite') && options.overwrite) {
      ignoreRevs = true;
    }
  } else {
    // set default value for keepNull
    var keepNullValue = ((typeof keepNull === 'undefined') ? true : keepNull);
    params = '?keepNull=' + (keepNullValue ? 'true' : 'false');

    if (overwrite) {
      ignoreRevs = true;
    }
    options = {};
  }
  var url = this._documenturl(id) + params;
  url = this._appendSyncParameter(url, waitForSync);
  url = this._appendBoolParameter(url, 'ignoreRevs', true);
  url = this._appendBoolParameter(url, 'returnOld', options.returnOld);
  url = this._appendBoolParameter(url, 'returnNew', options.returnNew);

  if (rev === null || ignoreRevs) {
    requestResult = this._connection.PATCH(url, JSON.stringify(data));
  } else {
    requestResult = this._connection.PATCH(url, JSON.stringify(data),
      {'if-match': JSON.stringify(rev) });
  }

  if (requestResult !== null && requestResult.error === true) {
    if (requestResult.errorNum === internal.errors.ERROR_HTTP_PRECONDITION_FAILED.code) {
      requestResult.errorNum = internal.errors.ERROR_ARANGO_CONFLICT.code;
    }
  }

  avocadosh.checkRequestResult(requestResult);

  return options.silent ? true : requestResult;
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

  var data = {
    query: query,
    bindVars: bindVars || undefined,
    count: (cursorOptions && cursorOptions.count) || false,
    batchSize: (cursorOptions && cursorOptions.batchSize) || undefined,
    options: options || undefined,
    cache: (options && options.cache) || undefined
  };

  return new AvocadoStatement(this, data).execute();
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
    query = { query: query.toAQL() };
  } else {
    query = { query: query };
  }

  const requestResult = this._connection.POST('/_api/query', JSON.stringify(query));

  if (requestResult && requestResult.error === true) {
    throw new AvocadoError(requestResult);
  }

  avocadosh.checkRequestResult(requestResult);

  return requestResult;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief create a new database
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._createDatabase = function (name, options, users) {
  var data = {
    name: name,
    options: options || { },
    users: users || []
  };

  var requestResult = this._connection.POST('/_api/database', JSON.stringify(data));

  if (requestResult !== null && requestResult.error === true) {
    throw new AvocadoError(requestResult);
  }

  avocadosh.checkRequestResult(requestResult);

  return requestResult.result;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief drop an existing database
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._dropDatabase = function (name) {
  var requestResult = this._connection.DELETE('/_api/database/' + encodeURIComponent(name));

  if (requestResult !== null && requestResult.error === true) {
    throw new AvocadoError(requestResult);
  }

  avocadosh.checkRequestResult(requestResult);

  return requestResult.result;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief list all existing databases
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._databases = function () {
  var requestResult = this._connection.GET('/_api/database');

  if (requestResult !== null && requestResult.error === true) {
    throw new AvocadoError(requestResult);
  }

  avocadosh.checkRequestResult(requestResult);

  return requestResult.result;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief uses a database
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._useDatabase = function (name) {
  if (internal.printBrowser) {
    throw new AvocadoError({
      error: true,
      code: internal.errors.ERROR_NOT_IMPLEMENTED.code,
      errorNum: internal.errors.ERROR_NOT_IMPLEMENTED.code,
      errorMessage: '_useDatabase() is not supported in the web interface'
    });
  }

  var old = this._connection.getDatabaseName();

  // no change
  if (name === old) {
    return true;
  }

  this._connection.setDatabaseName(name);

  try {
    // re-query properties
    this._queryProperties(true);
    this._flushCache();
  } catch (err) {
    this._connection.setDatabaseName(old);

    if (err.hasOwnProperty('errorNum')) {
      throw err;
    }

    throw new AvocadoError({
      error: true,
      code: internal.errors.ERROR_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_BAD_PARAMETER.code,
      errorMessage: "cannot use database '" + name + "'"
    });
  }

  return true;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief lists all endpoints
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._endpoints = function () {
  var requestResult = this._connection.GET('/_api/endpoint');

  if (requestResult !== null && requestResult.error === true) {
    throw new AvocadoError(requestResult);
  }

  avocadosh.checkRequestResult(requestResult);

  return requestResult;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief execute a transaction
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._executeTransaction = function (data) {
  if (!data || typeof (data) !== 'object') {
    throw new AvocadoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_BAD_PARAMETER.code,
      errorMessage: 'usage: _executeTransaction(<object>)'
    });
  }

  if (!data.collections || typeof (data.collections) !== 'object') {
    throw new AvocadoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_BAD_PARAMETER.code,
      errorMessage: 'missing/invalid collections definition for transaction'
    });
  }

  if (!data.action ||
    (typeof (data.action) !== 'string' && typeof (data.action) !== 'function')) {
    throw new AvocadoError({
      error: true,
      code: internal.errors.ERROR_HTTP_BAD_PARAMETER.code,
      errorNum: internal.errors.ERROR_BAD_PARAMETER.code,
      errorMessage: 'missing/invalid action definition for transaction'
    });
  }

  if (typeof (data.action) === 'function') {
    data.action = String(data.action);
  }

  var requestResult = this._connection.POST('/_api/transaction', JSON.stringify(data));

  if (requestResult !== null && requestResult.error === true) {
    throw new AvocadoError(requestResult);
  }

  avocadosh.checkRequestResult(requestResult);

  return requestResult.result;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief creates a new view
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._createView = function (name, type, properties) {
  var body = {
    'name': name,
    'type': type,
    'properties': properties
  };

  if (properties === undefined) {
    body['properties'] = {};
  }

  var requestResult = this._connection.POST(this._viewurl(),
    JSON.stringify(body));

  avocadosh.checkRequestResult(requestResult);

  var nname = requestResult.name;

  if (nname !== undefined) {
    this._registerView(nname, new this._viewConstructor(this, requestResult));
    return this._viewList[nname];
  }

  return undefined;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief return all views from the database
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._views = function () {
  var requestResult = this._connection.GET(this._viewurl());

  avocadosh.checkRequestResult(requestResult);

  var result = [];
  if (requestResult !== undefined) {
    var views = requestResult;
    var i;

    // add all views to object
    for (i = 0;  i < views.length;  ++i) {
      var view = new this._viewConstructor(this, views[i]);
      this._registerView(view._name, view);
      result.push(view);
    }

    result = result.sort(function (l, r) {
      // we assume no two views have the same name
      if (l.name().toLowerCase() < r.name().toLowerCase()) {
        return -1;
      }
      return 1;
    });
  }

  return result;
};

// //////////////////////////////////////////////////////////////////////////////
// / @brief return a single view, identified by its name
// //////////////////////////////////////////////////////////////////////////////

AvocadoDatabase.prototype._view = function (id) {
  if (this._viewList[id] && this._viewList[id] instanceof
      this._viewConstructor) {
    return this._viewList[id];
  }
  var url = this._viewurl(id);
  var requestResult = this._connection.GET(url);

  // return null in case of not found
  if (requestResult !== null
    && requestResult.error === true
    && requestResult.errorNum === internal.errors.ERROR_ARANGO_VIEW_NOT_FOUND.code) {
    return null;
  }

  // check all other errors and throw them
  avocadosh.checkRequestResult(requestResult);

  var name = requestResult.name;

  if (name !== undefined) {
    this._registerView(name, new this._viewConstructor(this, requestResult));
    return this._viewList[name];
  }

  return null;
};
