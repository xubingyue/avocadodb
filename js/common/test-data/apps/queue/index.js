'use strict';
const queues = require('@avocadodb/foxx/queues');
const router = require('@avocadodb/foxx/router')();
module.context.use(router);

router.post((req, res) => {
  const queue = queues.create('test_queue');
  queue.push({
    name: 'job',
    mount: '/queue_test_mount'
  }, {});
});

router.delete((req, res) => {
  const queue = queues.get('test_queue');
  for (const job of queue.all()) {
    queue.delete(job);
  }
  queues.delete('test_queue');
});
