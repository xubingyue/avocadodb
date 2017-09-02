'use strict';

const db = require('@avocadodb').db;
db['foxx_queue_test'].save({job: true});
