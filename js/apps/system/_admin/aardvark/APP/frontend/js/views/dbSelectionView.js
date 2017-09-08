/* jshint browser: true */
/* jshint unused: false */
/* global templateEngine, window, Backbone, $, avocadoHelper */
(function () {
  'use strict';
  window.DBSelectionView = Backbone.View.extend({
    template: templateEngine.createTemplate('dbSelectionView.ejs'),

    events: {
      'click .dbSelectionLink': 'changeDatabase'
    },

    initialize: function (opts) {
      this.current = opts.current;
    },

    changeDatabase: function (e) {
      var changeTo = $(e.currentTarget).closest('.dbSelectionLink.tab').attr('id');
      var url = this.collection.createDatabaseURL(changeTo);
      window.location.replace(url);
    },

    render: function (el) {
      var callback = function (error, list) {
        if (error) {
          avocadoHelper.avocadoError('DB', '无法链接数据库');
        } else {
          this.$el = el;
          this.$el.html(this.template.render({
            list: list,
            current: this.current.get('name')
          }));
          this.delegateEvents();
        }
      }.bind(this);

      this.collection.getDatabasesForUser(callback);

      return this.el;
    }
  });
}());
