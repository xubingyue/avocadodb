/* jshint browser: true */
/* jshint unused: false */
/* global avocadoHelper, Backbone, templateEngine, $, window, _ */
(function () {
  'use strict';

  window.NodesView = Backbone.View.extend({
    el: '#content',
    template: templateEngine.createTemplate('nodesView.ejs'),
    interval: 10000,
    knownServers: [],

    events: {
      'click #nodesContent .coords-nodes .pure-table-row': 'navigateToNode',
      'click #nodesContent .dbs-nodes .pure-table-row': 'navigateToNode',
      'click #nodesContent .coords-nodes .pure-table-row .fa-trash-o': 'deleteNode',
      'click #nodesContent .dbs-nodes .pure-table-row .fa-trash-o': 'deleteNode',
      'click #addCoord': 'addCoord',
      'click #removeCoord': 'removeCoord',
      'click #addDBs': 'addDBs',
      'click #removeDBs': 'removeDBs',
      'click .abortClusterPlan': 'abortClusterPlanModal',
      'keyup #plannedCoords': 'checkKey',
      'keyup #plannedDBs': 'checkKey'
    },

    remove: function () {
      this.$el.empty().off(); /* off to unbind the events */
      this.stopListening();
      this.unbind();
      delete this.el;
      return this;
    },

    checkKey: function (e) {
      if (e.keyCode === 13) {
        var self = this;

        var callbackFunction = function (e) {
          var number;
          if (e.target.id === 'plannedCoords') {
            try {
              number = JSON.parse($('#plannedCoords').val());
              if (typeof number === 'number') {
                window.modalView.hide();
                self.setCoordSize(number);
              } else {
                avocadoHelper.avocadoError('Error', '无效的值。必须是个数字.');
              }
            } catch (e) {
              avocadoHelper.avocadoError('Error', '无效的值。必须是个数字.');
            }
          } else if (e.target.id === 'plannedDBs') {
            try {
              number = JSON.parse($('#plannedCoords').val());
              if (typeof number === 'number') {
                window.modalView.hide();
                self.setDBsSize(number);
              } else {
                avocadoHelper.avocadoError('Error', '无效的值。一定是个数字.');
              }
            } catch (e) {
              avocadoHelper.avocadoError('Error', '无效的值。一定是个数字ss.');
            }
          }
        };

        this.changePlanModal(callbackFunction.bind(null, e));
      }
    },

    changePlanModal: function (func, element) {
      var buttons = []; var tableContent = [];
      tableContent.push(
        window.modalView.createReadOnlyEntry(
          'plan-confirm-button',
          'Caution',
          '您正在更改群集计划。继续吗？',
          undefined,
          undefined,
          false,
          /[<>&'"]/
        )
      );
      buttons.push(
        window.modalView.createSuccessButton('Yes', func.bind(this, element))
      );
      window.modalView.show('modalTable.ejs', 'Modify Cluster Size', buttons, tableContent);
    },

    initialize: function () {
      var self = this;
      clearInterval(this.intervalFunction);

      if (window.App.isCluster) {
        this.updateServerTime();

        // start polling with interval
        this.intervalFunction = window.setInterval(function () {
          if (window.location.hash === '#nodes') {
            self.render(false);
          }
        }, this.interval);
      }
    },

    deleteNode: function (elem) {
      if ($(elem.currentTarget).hasClass('noHover')) {
        return;
      }
      var self = this;
      var name = $(elem.currentTarget.parentNode.parentNode).attr('node').slice(0, -5);
      if (window.confirm('要删除此节点吗？')) {
        $.ajax({
          type: 'POST',
          url: avocadoHelper.databaseUrl('/_admin/cluster/removeServer'),
          contentType: 'application/json',
          async: true,
          data: JSON.stringify(name),
          success: function (data) {
            self.render(false);
          },
          error: function () {
            if (window.location.hash === '#nodes') {
              avocadoHelper.avocadoError('Cluster', '无法获取群集信息');
            }
          }
        });
      }
      return false;
    },

    navigateToNode: function (elem) {
      var name = $(elem.currentTarget).attr('node').slice(0, -5);

      if ($(elem.currentTarget).hasClass('noHover')) {
        return;
      }

      window.App.navigate('#node/' + encodeURIComponent(name), {trigger: true});
    },

    render: function (navi) {
      if (window.location.hash === '#nodes') {
        var self = this;

        if ($('#content').is(':empty')) {
          avocadoHelper.renderEmpty('请等待。请求集群信息...', 'fa fa-spin fa-circle-o-notch');
        }

        if (navi !== false) {
          avocadoHelper.buildNodesSubNav('Overview');
        }

        var scalingFunc = function (nodes) {
          $.ajax({
            type: 'GET',
            url: avocadoHelper.databaseUrl('/_admin/cluster/numberOfServers'),
            contentType: 'application/json',
            success: function (data) {
              if (window.location.hash === '#nodes') {
                self.continueRender(nodes, data);
              }
            }
          });
        };

        $.ajax({
          type: 'GET',
          cache: false,
          url: avocadoHelper.databaseUrl('/_admin/cluster/health'),
          contentType: 'application/json',
          processData: false,
          async: true,
          success: function (data) {
            if (window.location.hash === '#nodes') {
              scalingFunc(data.Health);
            }
          },
          error: function () {
            if (window.location.hash === '#nodes') {
              avocadoHelper.avocadoError('Cluster', '无法获取群集信息');
            }
          }
        });
      }
    },

    continueRender: function (nodes, scaling) {
      var coords = {};
      var dbs = {};
      var scale = false;

      _.each(nodes, function (node, name) {
        if (node.Role === 'Coordinator') {
          coords[name] = node;
        } else if (node.Role === 'DBServer') {
          dbs[name] = node;
        }
      });

      if (scaling.numberOfDBServers !== null && scaling.numberOfCoordinators !== null) {
        scale = true;
      }

      var callback = function (scaleProperties) {
        this.$el.html(this.template.render({
          coords: coords,
          dbs: dbs,
          scaling: scale,
          scaleProperties: scaleProperties,
          plannedDBs: scaling.numberOfDBServers,
          plannedCoords: scaling.numberOfCoordinators
        }));

        if (!scale) {
          $('.title').css('position', 'relative');
          $('.title').css('top', '-4px');
          $('.sectionHeader .information').css('margin-top', '-3px');
        }
      }.bind(this);

      this.renderCounts(scale, callback);
    },

    updatePlanned: function (data) {
      if (data.numberOfCoordinators) {
        $('#plannedCoords').val(data.numberOfCoordinators);
        this.renderCounts(true);
      }
      if (data.numberOfDBServers) {
        $('#plannedDBs').val(data.numberOfDBServers);
        this.renderCounts(true);
      }
    },

    setCoordSize: function (number) {
      var self = this;
      var data = {
        numberOfCoordinators: number
      };

      $.ajax({
        type: 'PUT',
        url: avocadoHelper.databaseUrl('/_admin/cluster/numberOfServers'),
        contentType: 'application/json',
        data: JSON.stringify(data),
        success: function () {
          self.updatePlanned(data);
        },
        error: function () {
          avocadoHelper.avocadoError('Scale', '无法设置控制器大小.');
        }
      });
    },

    setDBsSize: function (number) {
      var self = this;
      var data = {
        numberOfDBServers: number
      };

      $.ajax({
        type: 'PUT',
        url: avocadoHelper.databaseUrl('/_admin/cluster/numberOfServers'),
        contentType: 'application/json',
        data: JSON.stringify(data),
        success: function () {
          self.updatePlanned(data);
        },
        error: function () {
          avocadoHelper.avocadoError('Scale', '无法设置控制器大小.');
        }
      });
    },

    abortClusterPlanModal: function () {
      var buttons = []; var tableContent = [];
      tableContent.push(
        window.modalView.createReadOnlyEntry(
          'plan-abort-button',
          'Caution',
          '你是中止计划集群方案。所有挂起的服务器都将被删除。继续?',
          undefined,
          undefined,
          false,
          /[<>&'"]/
        )
      );
      buttons.push(
        window.modalView.createSuccessButton('Yes', this.abortClusterPlan.bind(this))
      );
      window.modalView.show('modalTable.ejs', 'Modify Cluster Size', buttons, tableContent);
    },

    abortClusterPlan: function () {
      window.modalView.hide();
      try {
        var coords = JSON.parse($('#infoCoords > .positive > span').text());
        var dbs = JSON.parse($('#infoDBs > .positive > span').text());
        this.setCoordSize(coords);
        this.setDBsSize(dbs);
      } catch (ignore) {
        avocadoHelper.avocadoError('Plan', 'Could not abort Cluster Plan');
      }
    },

    renderCounts: function (scale, callback) {
      var self = this;
      var renderFunc = function (id, ok, pending, error) {
        var string = '<span class="positive"><span>' + ok + '</span><i class="fa fa-check-circle"></i></span>';
        if (pending && scale === true) {
          string = string + '<span class="warning"><span>' + pending +
            '</span><i class="fa fa-circle-o-notch fa-spin"></i></span><button class="abortClusterPlan button-navbar button-default">Abort</button>';
        }
        if (error) {
          string = string + '<span class="negative"><span>' + error + '</span><i class="fa fa-exclamation-circle"></i></span>';
        }
        $(id).html(string);

        if (!scale) {
          $('.title').css('position', 'relative');
          $('.title').css('top', '-4px');
        }
      };

      var callbackFunction = function (nodes) {
        var coordsErrors = 0;
        var coords = 0;
        var coordsPending = 0;
        var dbs = 0;
        var dbsErrors = 0;
        var dbsPending = 0;

        _.each(nodes, function (node) {
          if (node.Role === 'Coordinator') {
            if (node.Status === 'GOOD') {
              coords++;
            } else {
              coordsErrors++;
            }
          } else if (node.Role === 'DBServer') {
            if (node.Status === 'GOOD') {
              dbs++;
            } else {
              dbsErrors++;
            }
          }
        });

        $.ajax({
          type: 'GET',
          cache: false,
          url: avocadoHelper.databaseUrl('/_admin/cluster/numberOfServers'),
          contentType: 'application/json',
          processData: false,
          success: function (data) {
            coordsPending = Math.abs((coords + coordsErrors) - data.numberOfCoordinators);
            dbsPending = Math.abs((dbs + dbsErrors) - data.numberOfDBServers);

            if (callback) {
              callback({
                coordsPending: coordsPending,
                coordsOk: coords,
                coordsErrors: coordsErrors,
                dbsPending: dbsPending,
                dbsOk: dbs,
                dbsErrors: dbsErrors
              });
            } else {
              renderFunc('#infoDBs', dbs, dbsPending, dbsErrors);
              renderFunc('#infoCoords', coords, coordsPending, coordsErrors);
            }

            if (!self.isPlanFinished()) {
              $('.scaleGroup').addClass('no-hover');
              $('#plannedCoords').attr('disabled', 'disabled');
              $('#plannedDBs').attr('disabled', 'disabled');
            }
          }
        });
      };

      $.ajax({
        type: 'GET',
        cache: false,
        url: avocadoHelper.databaseUrl('/_admin/cluster/health'),
        contentType: 'application/json',
        processData: false,
        success: function (data) {
          callbackFunction(data.Health);
        }
      });
    },

    isPlanFinished: function () {
      var boolean;

      if ($('#infoDBs').find('.warning').length > 0) {
        boolean = false;
      } else if ($('#infoCoords').find('.warning').length > 0) {
        boolean = false;
      } else {
        boolean = true;
      }

      return boolean;
    },

    addCoord: function () {
      var func = function () {
        window.modalView.hide();
        this.setCoordSize(this.readNumberFromID('#plannedCoords', true));
      };

      if (this.isPlanFinished()) {
        this.changePlanModal(func.bind(this));
      } else {
        avocadoHelper.avocadoNotification('Cluster Plan', '计划状态尚未完成.');
        $('.noty_buttons .button-danger').remove();
      }
    },

    removeCoord: function () {
      var func = function () {
        window.modalView.hide();
        this.setCoordSize(this.readNumberFromID('#plannedCoords', false, true));
      };

      if (this.isPlanFinished()) {
        this.changePlanModal(func.bind(this));
      } else {
        avocadoHelper.avocadoNotification('Cluster Plan', '计划状态尚未完成.');
        $('.noty_buttons .button-danger').remove();
      }
    },

    addDBs: function () {
      var func = function () {
        window.modalView.hide();
        this.setDBsSize(this.readNumberFromID('#plannedDBs', true));
      };

      if (this.isPlanFinished()) {
        this.changePlanModal(func.bind(this));
      } else {
        avocadoHelper.avocadoNotification('Cluster Plan', '计划状态尚未完成.');
        $('.noty_buttons .button-danger').remove();
      }
    },

    removeDBs: function () {
      var func = function () {
        window.modalView.hide();
        this.setDBsSize(this.readNumberFromID('#plannedDBs', false, true));
      };

      if (this.isPlanFinished()) {
        this.changePlanModal(func.bind(this));
      } else {
        avocadoHelper.avocadoNotification('Cluster Plan', '计划状态尚未完成.');
        $('.noty_buttons .button-danger').remove();
      }
    },

    readNumberFromID: function (id, increment, decrement) {
      var value = $(id).val();
      var parsed = false;

      try {
        parsed = JSON.parse(value);
      } catch (ignore) {}

      if (increment) {
        parsed++;
      }
      if (decrement) {
        if (parsed !== 1) {
          parsed--;
        }
      }

      return parsed;
    },

    updateServerTime: function () {
      this.serverTime = new Date().getTime();
    }

  });
}());
