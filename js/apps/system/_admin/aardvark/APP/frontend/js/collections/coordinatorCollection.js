/* jshint browser: true */
/* jshint unused: false */
/* global window, Backbone, avocadoHelper */
(function () {
  'use strict';
  window.CoordinatorCollection = Backbone.Collection.extend({
    model: window.Coordinator,

    url: avocadoHelper.databaseUrl('/_admin/aardvark/cluster/Coordinators')

  });
}());
