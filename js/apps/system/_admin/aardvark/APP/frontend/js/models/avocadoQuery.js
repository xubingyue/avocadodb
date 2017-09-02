/* global window, Backbone, avocadoHelper */
(function () {
  'use strict';
  window.AvocadoQuery = Backbone.Model.extend({
    urlRoot: avocadoHelper.databaseUrl('/_api/user'),

    defaults: {
      name: '',
      type: 'custom',
      value: ''
    }

  });
}());
