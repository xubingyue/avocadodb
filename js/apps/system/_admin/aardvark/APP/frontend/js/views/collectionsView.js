/* jshint browser: true */
/* jshint unused: false */
/* global frontendConfig, _, Backbone, templateEngine, window, setTimeout, clearTimeout, avocadoHelper, Joi, $ */

(function() {
  'use strict';
  window.CollectionsView = Backbone.View.extend({
    el: '#content',
    el2: '#collectionsThumbnailsIn',
    readOnly: false,

    searchTimeout: null,
    refreshRate: 10000,

    template: templateEngine.createTemplate('collectionsView.ejs'),

    remove: function() {
      this.$el.empty().off(); /* off to unbind the events */
      this.stopListening();
      this.unbind();
      delete this.el;
      return this;
    },

    refetchCollections: function() {
      var self = this;
      this.collection.fetch({
        cache: false,
        success: function() {
          self.checkLockedCollections();
        }
      });
    },

    checkLockedCollections: function() {
      var callback = function(error, lockedCollections) {
        var self = this;
        if (error) {
          console.log('Could not check locked collections');
        } else {
          this.collection.each(function(model) {
            model.set('locked', false);
          });

          _.each(lockedCollections, function(locked) {
            var model = self.collection.findWhere({
              id: locked.collection
            });
            model.set('locked', true);
            model.set('lockType', locked.type);
            model.set('desc', locked.desc);
          });

          this.collection.each(function(model) {
            if (!model.get('locked')) {
              $('#collection_' + model.get('name')).find(
                '.corneredBadge').removeClass('loaded unloaded');
              $('#collection_' + model.get('name') +
                ' .corneredBadge').text(model.get('status'));
              $('#collection_' + model.get('name') +
                ' .corneredBadge').addClass(model.get('status'));
            }

            if (model.get('locked') || model.get('status') ===
              'loading') {
              $('#collection_' + model.get('name')).addClass(
                'locked');
              if (model.get('locked')) {
                $('#collection_' + model.get('name')).find(
                  '.corneredBadge').removeClass(
                  'loaded unloaded');
                $('#collection_' + model.get('name')).find(
                  '.corneredBadge').addClass('inProgress');
                $('#collection_' + model.get('name') +
                  ' .corneredBadge').text(model.get('desc'));
              } else {
                $('#collection_' + model.get('name') +
                  ' .corneredBadge').text(model.get('status'));
              }
            } else {
              $('#collection_' + model.get('name')).removeClass(
                'locked');
              $('#collection_' + model.get('name') +
                ' .corneredBadge').text(model.get('status'));
              if ($('#collection_' + model.get('name') +
                  ' .corneredBadge').hasClass('inProgress')) {
                $('#collection_' + model.get('name') +
                  ' .corneredBadge').text(model.get('status'));
                $('#collection_' + model.get('name') +
                  ' .corneredBadge').removeClass('inProgress');
                $('#collection_' + model.get('name') +
                  ' .corneredBadge').addClass('loaded');
              }
              if (model.get('status') === 'unloaded') {
                $('#collection_' + model.get('name') +
                  ' .icon_avocadodb_info').addClass('disabled');
              }
            }
          });
        }
      }.bind(this);

      window.avocadoHelper.syncAndReturnUninishedAardvarkJobs('index',
        callback);
    },

    initialize: function() {
      var self = this;

      window.setInterval(function() {
        if (window.location.hash === '#collections' && window.VISIBLE) {
          self.refetchCollections();
        }
      }, self.refreshRate);
    },

    render: function() {
      this.checkLockedCollections();
      var dropdownVisible = false;

      if ($('#collectionsDropdown').is(':visible')) {
        dropdownVisible = true;
      }

      $(this.el).html(this.template.render({}));
      this.setFilterValues();

      if (dropdownVisible === true) {
        $('#collectionsDropdown2').show();
      }

      var searchOptions = this.collection.searchOptions;

      this.collection.getFiltered(searchOptions).forEach(function(
        avocadoCollection) {
        $('#collectionsThumbnailsIn', this.el).append(new window.CollectionListItemView({
          model: avocadoCollection,
          collectionsView: this
        }).render().el);
      }, this);

      // if type in collectionsDropdown2 is changed,
      // the page will be rerendered, so check the toggel button
      if ($('#collectionsDropdown2').css('display') === 'none') {
        $('#collectionsToggle').removeClass('activated');
      } else {
        $('#collectionsToggle').addClass('activated');
      }

      var length;
      avocadoHelper.setCheckboxStatus('#collectionsDropdown');

      try {
        length = searchOptions.searchPhrase.length;
      } catch (ignore) {}
      $('#searchInput').val(searchOptions.searchPhrase);
      $('#searchInput').focus();
      $('#searchInput')[0].setSelectionRange(length, length);

      avocadoHelper.fixTooltips('.icon_avocadodb, .avocadoicon', 'left');
      avocadoHelper.checkDatabasePermissions(this.setReadOnly.bind(this));

      return this;
    },

    setReadOnly: function() {
      this.readOnly = true;
      $('#createCollection').parent().parent().addClass('disabled');
    },

    events: {
      'click #createCollection': 'createCollection',
      'keydown #searchInput': 'restrictToSearchPhraseKey',
      'change #searchInput': 'restrictToSearchPhrase',
      'click #searchSubmit': 'restrictToSearchPhrase',
      'click .checkSystemCollections': 'checkSystem',
      'click #checkLoaded': 'checkLoaded',
      'click #checkUnloaded': 'checkUnloaded',
      'click #checkDocument': 'checkDocument',
      'click #checkEdge': 'checkEdge',
      'click #sortName': 'sortName',
      'click #sortType': 'sortType',
      'click #sortOrder': 'sortOrder',
      'click #collectionsToggle': 'toggleView',
      'click .css-label': 'checkBoxes'
    },

    updateCollectionsView: function() {
      var self = this;
      this.collection.fetch({
        cache: false,
        success: function() {
          self.render();
        }
      });
    },

    toggleView: function() {
      $('#collectionsToggle').toggleClass('activated');
      $('#collectionsDropdown2').slideToggle(200);
    },

    checkBoxes: function(e) {
      // chrome bugfix
      var clicked = e.currentTarget.id;
      $('#' + clicked).click();
    },

    checkSystem: function() {
      var searchOptions = this.collection.searchOptions;
      var oldValue = searchOptions.includeSystem;

      searchOptions.includeSystem = ($('.checkSystemCollections').is(
        ':checked') === true);

      if (oldValue !== searchOptions.includeSystem) {
        this.render();
      }
    },
    checkEdge: function() {
      var searchOptions = this.collection.searchOptions;
      var oldValue = searchOptions.includeEdge;

      searchOptions.includeEdge = ($('#checkEdge').is(':checked') ===
        true);

      if (oldValue !== searchOptions.includeEdge) {
        this.render();
      }
    },
    checkDocument: function() {
      var searchOptions = this.collection.searchOptions;
      var oldValue = searchOptions.includeDocument;

      searchOptions.includeDocument = ($('#checkDocument').is(
        ':checked') === true);

      if (oldValue !== searchOptions.includeDocument) {
        this.render();
      }
    },
    checkLoaded: function() {
      var searchOptions = this.collection.searchOptions;
      var oldValue = searchOptions.includeLoaded;

      searchOptions.includeLoaded = ($('#checkLoaded').is(':checked') ===
        true);

      if (oldValue !== searchOptions.includeLoaded) {
        this.render();
      }
    },
    checkUnloaded: function() {
      var searchOptions = this.collection.searchOptions;
      var oldValue = searchOptions.includeUnloaded;

      searchOptions.includeUnloaded = ($('#checkUnloaded').is(
        ':checked') === true);

      if (oldValue !== searchOptions.includeUnloaded) {
        this.render();
      }
    },
    sortName: function() {
      var searchOptions = this.collection.searchOptions;
      var oldValue = searchOptions.sortBy;

      searchOptions.sortBy = (($('#sortName').is(':checked') === true) ?
        'name' : 'type');
      if (oldValue !== searchOptions.sortBy) {
        this.render();
      }
    },
    sortType: function() {
      var searchOptions = this.collection.searchOptions;
      var oldValue = searchOptions.sortBy;

      searchOptions.sortBy = (($('#sortType').is(':checked') === true) ?
        'type' : 'name');
      if (oldValue !== searchOptions.sortBy) {
        this.render();
      }
    },
    sortOrder: function() {
      var searchOptions = this.collection.searchOptions;
      var oldValue = searchOptions.sortOrder;

      searchOptions.sortOrder = (($('#sortOrder').is(':checked') ===
        true) ? -1 : 1);
      if (oldValue !== searchOptions.sortOrder) {
        this.render();
      }
    },

    setFilterValues: function() {
      var searchOptions = this.collection.searchOptions;
      $('#checkLoaded').attr('checked', searchOptions.includeLoaded);
      $('#checkUnloaded').attr('checked', searchOptions.includeUnloaded);
      $('.checkSystemCollections').attr('checked', searchOptions.includeSystem);
      $('#checkEdge').attr('checked', searchOptions.includeEdge);
      $('#checkDocument').attr('checked', searchOptions.includeDocument);
      $('#sortName').attr('checked', searchOptions.sortBy !== 'type');
      $('#sortType').attr('checked', searchOptions.sortBy === 'type');
      $('#sortOrder').attr('checked', searchOptions.sortOrder !== 1);
    },

    search: function() {
      var searchOptions = this.collection.searchOptions;
      var searchPhrase = $('#searchInput').val();
      if (searchPhrase === searchOptions.searchPhrase) {
        return;
      }
      searchOptions.searchPhrase = searchPhrase;

      this.render();
    },

    resetSearch: function() {
      if (this.searchTimeout) {
        clearTimeout(this.searchTimeout);
        this.searchTimeout = null;
      }

      var searchOptions = this.collection.searchOptions;
      searchOptions.searchPhrase = null;
    },

    restrictToSearchPhraseKey: function() {
      // key pressed in search box
      var self = this;

      // force a new a search
      this.resetSearch();

      self.searchTimeout = setTimeout(function() {
        self.search();
      }, 200);
    },

    restrictToSearchPhrase: function() {
      // force a new a search
      this.resetSearch();

      // search executed
      this.search();
    },

    createCollection: function(e) {
      if (!this.readOnly) {
        e.preventDefault();
        var self = this;

        $.ajax({
          type: 'GET',
          cache: false,
          url: avocadoHelper.databaseUrl('/_api/engine'),
          contentType: 'application/json',
          processData: false,
          success: function(data) {
            self.engine = data;
            self.createNewCollectionModal(data);
          },
          error: function() {
            avocadoHelper.avocadoError('Engine',
              '不能提取avocadodb引擎日志细节.');
          }
        });
      }
    },

    submitCreateCollection: function() {
      var self = this;
      var callbackCoord = function(error, isCoordinator) {
        if (error) {
          avocadoHelper.avocadoError('DB',
            'Could not check coordinator state');
        } else {
          var collName = $('#new-collection-name').val();
          var collSize = $('#new-collection-size').val();
          var replicationFactor = $('#new-replication-factor').val();
          var collType = $('#new-collection-type').val();
          var collSync = $('#new-collection-sync').val();
          var shards = 1;
          var shardBy = [];

          if (replicationFactor === '') {
            replicationFactor = 1;
          }
          if ($('#is-satellite-collection').val() === 'true') {
            replicationFactor = 'satellite';
          }

          if (isCoordinator) {
            shards = $('#new-collection-shards').val();

            if (shards === '') {
              shards = 1;
            }
            shards = parseInt(shards, 10);
            if (shards < 1) {
              avocadoHelper.avocadoError(
                '碎片数必须是大于或等于1的整数值。'
              );
              return 0;
            }
            shardBy = _.pluck($('#new-collection-shardBy').select2(
              'data'), 'text');
            if (shardBy.length === 0) {
              shardBy.push('_key');
            }
          }
          if (collName.substr(0, 1) === '_') {
            avocadoHelper.avocadoError(
              '不允许第一个字符是“_”！');
            return 0;
          }
          var isSystem = false;
          var wfs = (collSync === 'true');
          if (collSize > 0) {
            try {
              collSize = JSON.parse(collSize) * 1024 * 1024;
            } catch (e) {
              avocadoHelper.avocadoError(
                '请输入有效数字');
              return 0;
            }
          }
          if (collName === '') {
            avocadoHelper.avocadoError('没有输入数据集名字!');
            return 0;
          }
          // no new system collections via webinterface
          // var isSystem = (collName.substr(0, 1) === '_')
          var callback = function(error, data) {
            if (error) {
              try {
                data = JSON.parse(data.responseText);
                avocadoHelper.avocadoError('Error', data.errorMessage);
              } catch (ignore) {}
            } else {
              this.updateCollectionsView();
            }
            window.modalView.hide();
          }.bind(this);

          var tmpObj = {
            collName: collName,
            wfs: wfs,
            isSystem: isSystem,
            replicationFactor: replicationFactor,
            collType: collType,
            shards: shards,
            shardBy: shardBy
          };
          if (self.engine.name !== 'rocksdb') {
            tmpObj.journalSize = collSize;
          }
          this.collection.newCollection(tmpObj, callback);
        }
      }.bind(this);

      window.isCoordinator(callbackCoord);
    },

    createNewCollectionModal: function() {
      var self = this;
      var callbackCoord2 = function(error, isCoordinator) {
        if (error) {
          avocadoHelper.avocadoError('DB',
            'Could not check coordinator state');
        } else {
          var buttons = [];
          var tableContent = [];
          var advanced = {};
          var advancedTableContent = [];

          tableContent.push(
            window.modalView.createTextEntry(
              'new-collection-name',
              'Name',
              '',
              false,
              '',
              true, [{
                rule: Joi.string().regex(/^[a-zA-Z]/),
                msg: '集合名必须始终以字母开头。'
              }, {
                rule: Joi.string().regex(/^[a-zA-Z0-9\-_]*$/),
                msg: '只有符号，“_”和“-”是允许的。'
              }, {
                rule: Joi.string().required(),
                msg: '没有指定的数据集名字.'
              }]
            )
          );
          tableContent.push(
            window.modalView.createSelectEntry(
              'new-collection-type',
              'Type',
              '',
              '要创建的数据集的类型', [{
                value: 2,
                label: 'Document'
              }, {
                value: 3,
                label: 'Edge'
              }]
            )
          );

          if (isCoordinator) {
            tableContent.push(
              window.modalView.createTextEntry(
                'new-collection-shards',
                'Shards',
                '',
                '要创建的分片数。你不能事后改变这个。' +
                '推荐设置dbservers数量的平方数',
                '',
                true
              )
            );
            tableContent.push(
              window.modalView.createSelect2Entry(
                'new-collection-shardBy',
                'shardBy',
                '',
                '用于在分片上找到文档的键。 ' +
                '键入键并按返回键添加.',
                '_key',
                false
              )
            );
          }

          buttons.push(
            window.modalView.createSuccessButton(
              'Save',
              this.submitCreateCollection.bind(this)
            )
          );
          if (window.App.isCluster) {
            if (frontendConfig.isEnterprise) {
              advancedTableContent.push(
                window.modalView.createSelectEntry(
                  'is-satellite-collection',
                  'Satellite collection',
                  '',
                  '创建卫星收集？这将禁用复制因子。.', [{
                    value: false,
                    label: 'No'
                  }, {
                    value: true,
                    label: 'Yes'
                  }]
                )
              );
            }
            advancedTableContent.push(
              window.modalView.createTextEntry(
                'new-replication-factor',
                'Replication factor',
                '',
                '数字值。必须至少1。集群中数据的总拷贝数',
                '',
                false, [{
                  rule: Joi.string().allow('').optional().regex(
                    /^[0-9]*$/),
                  msg: '必须是数字啊.'
                }]
              )
            );
          }
          if (self.engine.name !== 'rocksdb') {
            advancedTableContent.push(
              window.modalView.createTextEntry(
                'new-collection-size',
                'Journal size',
                '',
                '日志和数据文件最小为1mb或者更多.',
                '',
                false, [{
                  rule: Joi.string().allow('').optional().regex(
                    /^[0-9]*$/),
                  msg: 'Must be a number.'
                }]
              )
            );
          }
          advancedTableContent.push(
            window.modalView.createSelectEntry(
              'new-collection-sync',
              'Wait for sync',
              '',
              '在从文档的创建或更新返回之前同步到磁盘.', [{
                value: false,
                label: 'No'
              }, {
                value: true,
                label: 'Yes'
              }]
            )
          );
          advanced.header = 'Advanced';
          advanced.content = advancedTableContent;
          window.modalView.show(
            'modalTable.ejs',
            'New Collection',
            buttons,
            tableContent,
            advanced
          );

          // select2 workaround
          $(
            '#s2id_new-collection-shardBy .select2-search-field input'
          ).on('focusout', function(e) {
            if ($('.select2-drop').is(':visible')) {
              if (!$('#select2-search-field input').is(':focus')) {
                window.setTimeout(function() {
                  $(e.currentTarget).parent().parent().parent()
                    .select2('close');
                }, 200);
              }
            }
          });

          if (window.App.isCluster && frontendConfig.isEnterprise) {
            $('#is-satellite-collection').on('change', function(
              element) {
              if ($('#is-satellite-collection').val() === 'true') {
                $('#new-replication-factor').prop('disabled',
                  true);
              } else {
                $('#new-replication-factor').prop('disabled',
                  false);
              }
              $('#new-replication-factor').val('').focus().focusout();
            });
          }
        }
      }.bind(this);

      window.isCoordinator(callbackCoord2);
    }
  });
}());
