'use strict';
const router = require('@avocadodb/foxx/router')();
module.context.use(router);
router.get((req, res) => {
  res.send({hello: 'world'});
});

