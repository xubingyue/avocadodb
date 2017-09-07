/* jshint browser: true */
/* global Backbone, $, window, ace, avocadoHelper, templateEngine, Joi, _ */
(function() {
  'use strict';

  window.ApplicationDetailView = Backbone.View.extend({
    el: '#content',

    divs: ['#readme', '#swagger', '#app-info', '#sideinformation',
      '#information', '#settings'
    ],
    navs: ['#service-info', '#service-api', '#service-readme',
      '#service-settings'
    ],

    template: templateEngine.createTemplate('applicationDetailView.ejs'),

    remove: function() {
      this.$el.empty().off(); /* off to unbind the events */
      this.stopListening();
      this.unbind();
      delete this.el;
      return this;
    },

    events: {
      'click .open': 'openApp',
      'click .delete': 'deleteApp',
      'click #app-deps': 'showDepsDialog',
      'click #app-switch-mode': 'toggleDevelopment',
      'click #app-scripts [data-script]': 'runScript',
      'click #app-tests': 'runTests',
      'click #app-replace': 'replaceApp',
      'click #download-app': 'downloadApp',
      'click .subMenuEntries li': 'changeSubview',
      'click #jsonLink': 'toggleSwagger',
      'mouseenter #app-scripts': 'showDropdown',
      'mouseleave #app-scripts': 'hideDropdown'
    },

    resize: function(auto) {
      if (auto) {
        $('.innerContent').css('height', 'auto');
      } else {
        $('.innerContent').height($('.centralRow').height() - 150);
        $('#swagger iframe').height($('.centralRow').height() - 150);
        $('#swagger #swaggerJsonContent').height($('.centralRow').height() -
          150);
      }
    },

    toggleSwagger: function() {
      var callbackFunction = function(json) {
        $('#jsonLink').html('JSON');
        this.jsonEditor.setValue(JSON.stringify(json, null, '\t'), 1);
        $('#swaggerJsonContent').show();
        $('#swagger iframe').hide();
      }.bind(this);

      if ($('#jsonLink').html() === 'Swagger') {
        var url = avocadoHelper.databaseUrl(
          '/_admin/aardvark/foxxes/docs/swagger.json?mount=' +
          encodeURIComponent(this.model.get('mount')));
        avocadoHelper.download(url, callbackFunction);
      } else {
        $('#swaggerJsonContent').hide();
        $('#swagger iframe').show();
        $('#jsonLink').html('Swagger');
      }
    },

    changeSubview: function(e) {
      _.each(this.navs, function(nav) {
        $(nav).removeClass('active');
      });

      $(e.currentTarget).addClass('active');

      _.each(this.divs, function(div) {
        $('.headerButtonBar').hide();
        $(div).hide();
      });

      if (e.currentTarget.id === 'service-readme') {
        this.resize(true);
        $('#readme').show();
      } else if (e.currentTarget.id === 'service-api') {
        this.resize();
        $('#swagger').show();
      } else if (e.currentTarget.id === 'service-info') {
        this.resize(true);
        this.render();
        $('#information').show();
        $('#sideinformation').show();
      } else if (e.currentTarget.id === 'service-settings') {
        this.resize(true);
        this.showConfigDialog();
        $('.headerButtonBar').show();
        $('#settings').show();
      }
    },

    downloadApp: function() {
      if (!this.model.isSystem()) {
        this.model.download();
      }
    },

    replaceApp: function() {
      var mount = this.model.get('mount');
      window.foxxInstallView.upgrade(mount, function() {
        window.App.applicationDetail(encodeURIComponent(mount));
      });
      $('.createModalDialog .avocadoHeader').html('Replace Service');
      $('#infoTab').click();
    },

    updateConfig: function() {
      this.model.getConfiguration(function() {
        $('#app-warning')[this.model.needsAttention() ? 'show' :
          'hide']();
        $('#app-warning-config')[this.model.needsConfiguration() ?
          'show' : 'hide']();

        if (this.model.needsConfiguration()) {
          $('#app-config').addClass('error');
        } else {
          $('#app-config').removeClass('error');
        }
      }.bind(this));
    },

    updateDeps: function() {
      this.model.getDependencies(function() {
        $('#app-warning')[this.model.needsAttention() ? 'show' :
          'hide']();
        $('#app-warning-deps')[this.model.hasUnconfiguredDependencies() ?
          'show' : 'hide']();
        if (this.model.hasUnconfiguredDependencies()) {
          $('#app-deps').addClass('error');
        } else {
          $('#app-deps').removeClass('error');
        }
      }.bind(this));
    },

    toggleDevelopment: function() {
      this.model.toggleDevelopment(!this.model.isDevelopment(),
        function() {
          if (this.model.isDevelopment()) {
            $('.app-switch-mode').text('Set Production');
            $('#app-development-indicator').css('display', 'inline');
            $('#app-development-path').css('display', 'inline');
          } else {
            $('.app-switch-mode').text('Set Development');
            $('#app-development-indicator').css('display', 'none');
            $('#app-development-path').css('display', 'none');
          }
        }.bind(this));
    },

    runScript: function(event) {
      event.preventDefault();
      var script = $(event.currentTarget).attr('data-script');
      var tableContent = [
        window.modalView.createBlobEntry(
          'app_script_arguments',
          'Script arguments',
          '', null, 'optional', false, [{
            rule: function(v) {
              return v && JSON.parse(v);
            },
            msg: '必须是格式良好的JSON或空的'
          }]
        )
      ];
      var buttons = [
        window.modalView.createSuccessButton('Run script', function() {
          var opts = $('#app_script_arguments').val();
          opts = opts && JSON.parse(opts);
          window.modalView.hide();
          this.model.runScript(script, opts, function(err, result) {
            var info;
            if (err) {
              info = (
                '<p>脚本出错' +
                (err.statusCode ? (' (HTTP ' + err.statusCode +
                  ')') : '') +
                ':</p>' +
                '<pre>' + err.message + '</pre>'
              );
            } else if (result) {
              info = (
                '<p>脚本的结果：</p>' +
                '<pre>' + JSON.stringify(result, null, 2) +
                '</pre>'
              );
            } else {
              info = '<p>脚本成功运行。</p>';
            }
            window.modalView.show(
              'modalTable.ejs',
              'Result of script "' + script + '"',
              undefined,
              undefined,
              undefined,
              info
            );
          });
        }.bind(this))
      ];
      window.modalView.show(
        'modalTable.ejs',
        'Run script "' + script + '" on "' + this.model.get('mount') +
        '"',
        buttons,
        tableContent
      );
    },

    showSwagger: function(event) {
      event.preventDefault();
      this.render('swagger');
    },

    showReadme: function(event) {
      event.preventDefault();
      this.render('readme');
    },

    runTests: function(event) {
      event.preventDefault();
      var warning = (
        '<p><strong>警告:</strong> 运行测试可能会造成破坏性的副作用，包括数据丢失。' +
        ' 请确保不要在生产数据库上运行测试。 < /p>'
      );
      if (this.model.isDevelopment()) {
        warning += (
          '<p><strong>警告:</strong> 这个app运行在 <strong>开发者模式</strong>.' +
          '如果任何测试访问应用程序的HTTP API，它们可能变得不确定性。</p>'
        );
      }
      var buttons = [
        window.modalView.createSuccessButton('Run tests', function() {
          window.modalView.hide();
          this.model.runTests({
            reporter: 'suite'
          }, function(err, result) {
            window.modalView.show(
              'modalTestResults.ejs',
              'Test results',
              undefined,
              undefined,
              undefined,
              err || result
            );
          });
        }.bind(this))
      ];
      window.modalView.show(
        'modalTable.ejs',
        'Run tests for app "' + this.model.get('mount') + '"',
        buttons,
        undefined,
        undefined,
        warning
      );
    },

    render: function(mode) {
      this.resize();
      this.model.fetchThumbnail(function() {
        var callback = function(error, db) {
          var self = this;
          if (error) {
            avocadoHelper.avocadoError('DB',
              '无法获取当前数据库');
          } else {
            $(this.el).html(this.template.render({
              app: this.model,
              baseUrl: avocadoHelper.databaseUrl('', db),
              mode: mode
            }));

            // init ace
            self.jsonEditor = ace.edit('swaggerJsonEditor');
            self.jsonEditor.setReadOnly(true);
            self.jsonEditor.getSession().setMode('ace/mode/json');

            $.ajax({
              url: this.appUrl(db),
              headers: {
                accept: 'text/html,*/*;q=0.9'
              }
            }).success(function() {
              $('.open', this.el).prop('disabled', false);
            }.bind(this));

            this.updateConfig();
            this.updateDeps();

            if (mode === 'swagger') {
              $.get('./foxxes/docs/swagger.json?mount=' +
                encodeURIComponent(this.model.get('mount')),
                function(data) {
                  if (Object.keys(data.paths).length < 1) {
                    self.render('readme');
                    $('#app-show-swagger').attr('disabled',
                      'true');
                  }
                });
            }
          }

          this.breadcrumb();
        }.bind(this);

        avocadoHelper.currentDatabase(callback);

        if (_.isEmpty(this.model.get('config'))) {
          $('#service-settings').attr('disabled', true);
        }
      }.bind(this));
      return $(this.el);
    },

    breadcrumb: function() {
      var string = 'Service: ' + this.model.get('name') +
        '<i class="fa fa-ellipsis-v" aria-hidden="true"></i>';

      var contributors = '<p class="mount"><span>Contributors:</span>';
      if (this.model.get('contributors') && this.model.get(
          'contributors').length > 0) {
        _.each(this.model.get('contributors'), function(contributor) {
          if (contributor.email) {
            contributors += '<a href="mailto:' + contributor.email +
              '">' + (contributor.name || contributor.email) +
              '</a>';
          } else if (contributor.name) {
            contributors += '<a>contributor.name</a>';
          }
        });
      } else {
        contributors += 'No contributors';
      }
      contributors += '</p>';
      $('.information').append(
        contributors
      );

      // information box info tab
      if (this.model.get('author')) {
        $('.information').append(
          '<p class="mount"><span>作者:</span>' + this.model.get(
            'author') + '</p>'
        );
      }
      if (this.model.get('mount')) {
        $('.information').append(
          '<p class="mount"><span>安装:</span>' + this.model.get(
            'mount') + '</p>'
        );
      }
      if (this.model.get('development')) {
        if (this.model.get('path')) {
          $('.information').append(
            '<p class="path"><span>路径:</span>' + this.model.get(
              'path') + '</p>'
          );
        }
      }
      $('#subNavigationBar .breadcrumb').html(string);
    },

    openApp: function() {
      var callback = function(error, db) {
        if (error) {
          avocadoHelper.avocadoError('DB',
            '无法获取当前数据库');
        } else {
          window.open(this.appUrl(db), this.model.get('title')).focus();
        }
      }.bind(this);

      avocadoHelper.currentDatabase(callback);
    },

    deleteApp: function() {
      var buttons = [
        window.modalView.createDeleteButton('Delete', function() {
          var opts = {
            teardown: $('#app_delete_run_teardown').is(':checked')
          };
          this.model.destroy(opts, function(err, result) {
            if (!err && result.error === false) {
              window.modalView.hide();
              window.App.navigate('services', {
                trigger: true
              });
            }
          });
        }.bind(this))
      ];
      var tableContent = [
        window.modalView.createCheckboxEntry(
          'app_delete_run_teardown',
          'Run teardown?',
          true,
          "Should this app's teardown script be executed before removing the app?",
          true
        )
      ];
      window.modalView.show(
        'modalTable.ejs',
        'Delete Foxx App mounted at "' + this.model.get('mount') +
        '"',
        buttons,
        tableContent,
        undefined,
        '<p>你肯定吗？没有回头路...</p>',
        true
      );
    },

    appUrl: function(currentDB) {
      return avocadoHelper.databaseUrl(this.model.get('mount'),
        currentDB);
    },

    applyConfig: function() {
      var cfg = {};
      _.each(this.model.get('config'), function(opt, key) {
        var $el = $('#app_config_' + key);
        var val = $el.val();
        if (opt.type === 'boolean' || opt.type === 'bool') {
          cfg[key] = $el.is(':checked');
          return;
        }
        if (val === '' && opt.hasOwnProperty('default')) {
          cfg[key] = opt.default;
          if (opt.type === 'json') {
            cfg[key] = JSON.stringify(opt.default);
          }
          return;
        }
        if (opt.type === 'number') {
          cfg[key] = parseFloat(val);
        } else if (opt.type === 'integer' || opt.type === 'int') {
          cfg[key] = parseInt(val, 10);
        } else if (opt.type === 'json') {
          cfg[key] = val && JSON.stringify(JSON.parse(val));
        } else {
          cfg[key] = val;
          return;
        }
      });
      this.model.setConfiguration(cfg, function() {
        this.updateConfig();
        avocadoHelper.avocadoNotification(this.model.get('name'),
          '设置应用.');
      }.bind(this));
    },

    showConfigDialog: function() {
      if (_.isEmpty(this.model.get('config'))) {
        $('#settings .buttons').html($('#hidden_buttons').html());
        return;
      }
      var tableContent = _.map(this.model.get('config'), function(obj,
        name) {
        var defaultValue = obj.default === undefined ? '' : String(
          obj.default);
        var currentValue = obj.current === undefined ? '' : String(
          obj.current);
        var methodName = 'createTextEntry';
        var mandatory = false;
        var checks = [];
        if (obj.type === 'boolean' || obj.type === 'bool') {
          methodName = 'createCheckboxEntry';
          obj.default = obj.default || false;
          defaultValue = obj.default || false;
          currentValue = obj.current || false;
        } else if (obj.type === 'json') {
          methodName = 'createBlobEntry';
          defaultValue = obj.default === undefined ? '' : JSON.stringify(
            obj.default);
          currentValue = obj.current === undefined ? '' : obj.current;
          checks.push({
            rule: function(v) {
              return v && JSON.parse(v);
            },
            msg: '必须是格式良好的JSON或空的.'
          });
        } else if (obj.type === 'integer' || obj.type === 'int') {
          checks.push({
            rule: Joi.number().integer().optional().allow(''),
            msg: '必须是整数.'
          });
        } else if (obj.type === 'number') {
          checks.push({
            rule: Joi.number().optional().allow(''),
            msg: '必须是一个数字.'
          });
        } else {
          if (obj.type === 'password') {
            methodName = 'createPasswordEntry';
          }
          checks.push({
            rule: Joi.string().optional().allow(''),
            msg: '必须是一个字符串.'
          });
        }
        if (obj.default === undefined && obj.required !== false) {
          mandatory = true;
          checks.unshift({
            rule: Joi.any().required(),
            msg: '此字段必填.'
          });
        }
        return window.modalView[methodName](
          'app_config_' + name,
          name,
          currentValue,
          obj.description,
          defaultValue,
          mandatory,
          checks
        );
      });

      var buttons = [
        window.modalView.createSuccessButton('Apply', this.applyConfig
          .bind(this))
      ];

      window.modalView.show(
        'modalTable.ejs', 'Configuration', buttons, tableContent,
        null, null, null, null, null, 'settings'
      );
      $('.modal-footer').prepend($('#hidden_buttons').html());
    },

    applyDeps: function() {
      var deps = {};
      _.each(this.model.get('deps'), function(title, name) {
        var $el = $('#app_deps_' + name);
        deps[name] = window.avocadoHelper.escapeHtml($el.val());
      });
      this.model.setDependencies(deps, function() {
        window.modalView.hide();
        this.updateDeps();
      }.bind(this));
    },

    showDepsDialog: function() {
      if (_.isEmpty(this.model.get('deps'))) {
        return;
      }
      var tableContent = _.map(this.model.get('deps'), function(obj,
        name) {
        var currentValue = obj.current === undefined ? '' : String(
          obj.current);
        var defaultValue = '';
        var description = obj.definition.name;
        if (obj.definition.version !== '*') {
          description += '@' + obj.definition.version;
        }
        var checks = [{
          rule: Joi.string().optional().allow(''),
          msg: '必须是一个字符串.'
        }];
        if (obj.definition.required) {
          checks.push({
            rule: Joi.string().required(),
            msg: '此值是必需的.'
          });
        }
        return window.modalView.createTextEntry(
          'app_deps_' + name,
          obj.title,
          currentValue,
          description,
          defaultValue,
          obj.definition.required,
          checks
        );
      });
      var buttons = [
        window.modalView.createSuccessButton('Apply', this.applyDeps.bind(
          this))
      ];
      window.modalView.show(
        'modalTable.ejs', 'Dependencies', buttons, tableContent
      );
    },

    showDropdown: function() {
      if (!_.isEmpty(this.model.get('scripts'))) {
        $('#scripts_dropdown').show(200);
      }
    },

    hideDropdown: function() {
      $('#scripts_dropdown').hide();
    }
  });
}());
