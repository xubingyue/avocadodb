/* global Backbone, window, avocadoHelper, frontendConfig */

(function () {
  'use strict';

  window.CurrentDatabase = Backbone.Model.extend({
    url: avocadoHelper.databaseUrl('/_api/database/current', frontendConfig.db),

    parse: function (data) {
      return data.result;
    }
  });
}());
