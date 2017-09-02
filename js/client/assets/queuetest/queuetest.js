const db = require('internal').db;

db.foxxqueuetest.replace('test', {'date': Date.now(), 'server': AvocadoServerState.id()});
module.exports = true;
