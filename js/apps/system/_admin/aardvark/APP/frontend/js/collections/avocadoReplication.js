/* jshint browser: true */
/* jshint strict: false, unused: false */
/* global window, Backbone, $, window, avocadoHelper */

window.AvocadoReplication = Backbone.Collection.extend({
  model: window.Replication,

  url: '../api/user',

  getLogState: function (callback) {
    $.ajax({
      type: 'GET',
      cache: false,
      url: avocadoHelper.databaseUrl('/_api/replication/logger-state'),
      contentType: 'application/json',
      processData: false,
      success: function (data) {
        callback(false, data);
      },
      error: function (data) {
        callback(true, data);
      }
    });
  },
  getApplyState: function (callback) {
    $.ajax({
      type: 'GET',
      cache: false,
      url: avocadoHelper.databaseUrl('/_api/replication/applier-state'),
      contentType: 'application/json',
      processData: false,
      success: function (data) {
        callback(false, data);
      },
      error: function (data) {
        callback(true, data);
      }
    });
  }

});
