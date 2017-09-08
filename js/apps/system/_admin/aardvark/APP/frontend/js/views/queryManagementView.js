/* jshint browser: true */
/* jshint unused: false */
/* global Backbone, $, setTimeout, window, avocadoHelper, templateEngine */

(function () {
  'use strict';
  window.QueryManagementView = Backbone.View.extend({
    el: '#content',

    id: '#queryManagementContent',

    templateActive: templateEngine.createTemplate('queryManagementViewActive.ejs'),
    templateSlow: templateEngine.createTemplate('queryManagementViewSlow.ejs'),
    table: templateEngine.createTemplate('avocadoTable.ejs'),
    active: true,
    shouldRender: true,
    timer: 0,
    refreshRate: 2000,

    initialize: function () {
      var self = this;
      this.activeCollection = new window.QueryManagementActive();
      this.slowCollection = new window.QueryManagementSlow();
      this.convertModelToJSON(true);

      window.setInterval(function () {
        if (window.location.hash === '#queries' && window.VISIBLE && self.shouldRender &&
          avocadoHelper.getCurrentSub().route === 'queryManagement') {
          if (self.active) {
            if ($('#avocadoQueryManagementTable').is(':visible')) {
              self.convertModelToJSON(true);
            }
          } else {
            if ($('#avocadoQueryManagementTable').is(':visible')) {
              self.convertModelToJSON(false);
            }
          }
        }
      }, self.refreshRate);
    },

    events: {
      'click #deleteSlowQueryHistory': 'deleteSlowQueryHistoryModal',
      'click #avocadoQueryManagementTable .fa-minus-circle': 'deleteRunningQueryModal'
    },

    tableDescription: {
      id: 'avocadoQueryManagementTable',
      titles: ['ID', 'Query String', 'Bind parameter', 'Runtime', 'Started', ''],
      rows: [],
      unescaped: [false, false, false, false, true]
    },

    deleteRunningQueryModal: function (e) {
      this.killQueryId = $(e.currentTarget).attr('data-id');
      var buttons = [];
      var tableContent = [];

      tableContent.push(
        window.modalView.createReadOnlyEntry(
          undefined,
          'Running Query',
          '你真的要杀死正在查询的进程么？?',
          undefined,
          undefined,
          false,
          undefined
        )
      );

      buttons.push(
        window.modalView.createDeleteButton('Kill', this.killRunningQuery.bind(this))
      );

      window.modalView.show(
        'modalTable.ejs',
        'Kill Running Query',
        buttons,
        tableContent
      );

      $('.modal-delete-confirmation strong').html('Really kill?');
    },

    killRunningQuery: function () {
      this.collection.killRunningQuery(this.killQueryId, this.killRunningQueryCallback.bind(this));
      window.modalView.hide();
    },

    killRunningQueryCallback: function () {
      this.convertModelToJSON(true);
      this.renderActive();
    },

    deleteSlowQueryHistoryModal: function () {
      var buttons = [];
      var tableContent = [];

      tableContent.push(
        window.modalView.createReadOnlyEntry(
          undefined,
          'Slow Query Log',
          '要删除慢速查询日志条目吗？?',
          undefined,
          undefined,
          false,
          undefined
        )
      );

      buttons.push(
        window.modalView.createDeleteButton('Delete', this.deleteSlowQueryHistory.bind(this))
      );

      window.modalView.show(
        'modalTable.ejs',
        '删除慢速查询日志',
        buttons,
        tableContent
      );
    },

    deleteSlowQueryHistory: function () {
      this.collection.deleteSlowQueryHistory(this.slowQueryCallback.bind(this));
      window.modalView.hide();
    },

    slowQueryCallback: function () {
      this.convertModelToJSON(false);
      this.renderSlow();
    },

    render: function () {
      var options = avocadoHelper.getCurrentSub();
      if (options.params.active) {
        this.active = true;
        this.convertModelToJSON(true);
      } else {
        this.active = false;
        this.convertModelToJSON(false);
      }
    },

    addEvents: function () {
      var self = this;
      $('#queryManagementContent tbody').on('mousedown', function () {
        clearTimeout(self.timer);
        self.shouldRender = false;
      });
      $('#queryManagementContent tbody').on('mouseup', function () {
        self.timer = window.setTimeout(function () {
          self.shouldRender = true;
        }, 3000);
      });
    },

    renderActive: function () {
      this.$el.html(this.templateActive.render({}));
      $(this.id).append(this.table.render({
        content: this.tableDescription,
        type: {
          1: 'pre',
          2: 'pre'
        }
      }));
      $('#activequeries').addClass('avocado-active-tab');
      this.addEvents();
    },

    renderSlow: function () {
      this.$el.html(this.templateSlow.render({}));
      $(this.id).append(this.table.render({
        content: this.tableDescription,
        type: {
          1: 'pre',
          2: 'pre'
        }
      }));
      $('#slowqueries').addClass('avocado-active-tab');
      this.addEvents();
    },

    convertModelToJSON: function (active) {
      var self = this;
      var rowsArray = [];

      if (active === true) {
        this.collection = this.activeCollection;
      } else {
        this.collection = this.slowCollection;
      }

      this.collection.fetch({
        success: function () {
          self.collection.each(function (model) {
            var button = '';
            if (active) {
              button = '<i data-id="' + model.get('id') + '" class="fa fa-minus-circle"></i>';
            }
            rowsArray.push([
              model.get('id'),
              model.get('query'),
              JSON.stringify(model.get('bindVars'), null, 2),
              model.get('runTime').toFixed(2) + ' s',
              model.get('started'),
              button
            ]);
          });

          var message = '没有运行查询.';
          if (!active) {
            message = '没有慢查询.';
          }

          if (rowsArray.length === 0) {
            rowsArray.push([
              message,
              '',
              '',
              '',
              ''
            ]);
          }

          self.tableDescription.rows = rowsArray;

          if (active) {
            self.renderActive();
          } else {
            self.renderSlow();
          }
        }
      });
    }

  });
}());
