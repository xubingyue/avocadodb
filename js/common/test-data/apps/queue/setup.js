'use strict';

const db = require('@avocadodb').db;
const name = 'foxx_queue_test';

if (!db._collection(name)) {
  db._createDocumentCollection(name);
}
