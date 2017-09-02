/* global window, Backbone, avocadoHelper */

window.DatabaseModel = Backbone.Model.extend({
  idAttribute: 'name',

  initialize: function () {
    'use strict';
  },

  isNew: function () {
    'use strict';
    return false;
  },
  sync: function (method, model, options) {
    'use strict';
    if (method === 'update') {
      method = 'create';
    }
    return Backbone.sync(method, model, options);
  },

  url: avocadoHelper.databaseUrl('/_api/database'),

  defaults: {
  }

});
