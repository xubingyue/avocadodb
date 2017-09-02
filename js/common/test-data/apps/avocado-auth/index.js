'use strict';

const createRouter = require('@avocadodb/foxx/router');
const router = createRouter();

router.get(function (req, res) {
  res.json({user: req.avocadoUser});
});

module.context.use(router);
