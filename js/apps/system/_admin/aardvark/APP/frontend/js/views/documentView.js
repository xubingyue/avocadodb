/* jshint browser: true */
/* jshint unused: false */
/* global Backbone, $, localStorage, window, avocadoHelper, templateEngine, JSONEditor */
/* global document, _ */

(function() {
  'use strict';

  var createDocumentLink = function(id) {
    var split = id.split('/');
    return 'collection/' +
      encodeURIComponent(split[0]) + '/' +
      encodeURIComponent(split[1]);
  };

  window.DocumentView = Backbone.View.extend({
    el: '#content',
    readOnly: false,
    colid: 0,
    docid: 0,

    customView: false,
    defaultMode: 'tree',

    template: templateEngine.createTemplate('documentView.ejs'),

    events: {
      'click #saveDocumentButton': 'saveDocument',
      'click #deleteDocumentButton': 'deleteDocumentModal',
      'click #confirmDeleteDocument': 'deleteDocument',
      'click #document-from': 'navigateToDocument',
      'click #document-to': 'navigateToDocument',
      'keydown #documentEditor .ace_editor': 'keyPress',
      'keyup .jsoneditor .search input': 'checkSearchBox',
      'click .jsoneditor .modes': 'storeMode',
      'click #addDocument': 'addDocument'
    },

    remove: function() {
      this.$el.empty().off(); /* off to unbind the events */
      this.stopListening();
      this.unbind();
      delete this.el;
      return this;
    },

    checkSearchBox: function(e) {
      if ($(e.currentTarget).val() === '') {
        this.editor.expandAll();
      }
    },

    initialize: function() {
      var mode = localStorage.getItem('JSONEditorMode');
      if (mode) {
        this.defaultMode = mode;
      }
    },

    addDocument: function(e) {
      if (!this.readOnly) {
        window.App.documentsView.addDocumentModal(e);
      }
    },

    storeMode: function() {
      var self = this;

      $('.type-modes').on('click', function(elem) {
        var mode = $(elem.currentTarget).text().toLowerCase();
        localStorage.setItem('JSONEditorMode', mode);
        self.defaultMode = mode;
      });
    },

    keyPress: function(e) {
      if (e.ctrlKey && e.keyCode === 13) {
        e.preventDefault();
        this.saveDocument();
      } else if (e.metaKey && e.keyCode === 13) {
        e.preventDefault();
        this.saveDocument();
      }
    },

    editor: 0,

    setType: function() {
      var callback = function(error, data, type) {
        if (error) {
          avocadoHelper.avocadoError('Error', 'Could not fetch data.');
        } else {
          this.type = type;
          this.breadcrumb();
          this.fillInfo();
          this.fillEditor();
          avocadoHelper.checkCollectionPermissions(this.colid, this.changeViewToReadOnly
            .bind(this));
        }
      }.bind(this);

      this.collection.getDocument(this.colid, this.docid, callback);
    },

    deleteDocumentModal: function() {
      if (!this.readOnly) {
        var buttons = [];
        var tableContent = [];
        tableContent.push(
          window.modalView.createReadOnlyEntry(
            'doc-delete-button',
            '确认删除，文档ID是',
            this.type._id,
            undefined,
            undefined,
            false,
            /[<>&'"]/
          )
        );
        buttons.push(
          window.modalView.createDeleteButton('删除', this.deleteDocument
            .bind(this))
        );
        window.modalView.show('modalTable.ejs', '删除数据',
          buttons, tableContent);
      }
    },

    deleteDocument: function() {
      var successFunction = function() {
        if (this.customView) {
          this.customDeleteFunction();
        } else {
          var navigateTo = 'collection/' + encodeURIComponent(this.colid) +
            '/documents/1';
          window.modalView.hide();
          window.App.navigate(navigateTo, {
            trigger: true
          });
        }
      }.bind(this);

      if (this.type._from && this.type._to) {
        var callbackEdge = function(error) {
          if (error) {
            avocadoHelper.avocadoError('Edge error',
              '无法删除 edge');
          } else {
            successFunction();
          }
        };
        this.collection.deleteEdge(this.colid, this.docid, callbackEdge);
      } else {
        var callbackDoc = function(error) {
          if (error) {
            avocadoHelper.avocadoError('Error',
              '无法删除数据');
          } else {
            successFunction();
          }
        };
        this.collection.deleteDocument(this.colid, this.docid,
          callbackDoc);
      }
    },

    navigateToDocument: function(e) {
      var navigateTo = $(e.target).attr('documentLink');
      var test = (navigateTo.split('%').length - 1) % 3;

      if (decodeURIComponent(navigateTo) !== navigateTo && test !== 0) {
        navigateTo = decodeURIComponent(navigateTo);
      }

      if (navigateTo) {
        window.App.navigate(navigateTo, {
          trigger: true
        });
      }
    },

    fillInfo: function() {
      var mod = this.collection.first();
      var _id = mod.get('_id');
      var _key = mod.get('_key');
      var _rev = mod.get('_rev');
      var _from = mod.get('_from');
      var _to = mod.get('_to');

      $('#document-type').css('margin-left', '10px');
      $('#document-type').text('_id:');
      $('#document-id').css('margin-left', '0');
      $('#document-id').text(_id);
      $('#document-key').text(_key);
      $('#document-rev').text(_rev);

      if (_from && _to) {
        var hrefFrom = createDocumentLink(_from);
        var hrefTo = createDocumentLink(_to);
        $('#document-from').text(_from);
        $('#document-from').attr('documentLink', hrefFrom);
        $('#document-to').text(_to);
        $('#document-to').attr('documentLink', hrefTo);
      } else {
        $('.edge-info-container').hide();
      }
    },

    fillEditor: function() {
      var toFill = this.removeReadonlyKeys(this.collection.first().attributes);
      $('.disabledBread').last().text(this.collection.first().get(
        '_key'));
      this.editor.set(toFill);
      $('.ace_content').attr('font-size', '11pt');
    },

    jsonContentChanged: function() {
      this.enableSaveButton();
    },

    resize: function() {
      $('#documentEditor').height($('.centralRow').height() - 300);
    },

    render: function() {
      $(this.el).html(this.template.render({}));

      $('#documentEditor').height($('.centralRow').height() - 300);
      this.disableSaveButton();

      var self = this;

      var container = document.getElementById('documentEditor');
      var options = {
        onChange: function() {
          self.jsonContentChanged();
        },
        search: true,
        mode: 'tree',
        modes: ['tree', 'code'],
        ace: window.ace
          // iconlib: 'fontawesome4'
      };

      this.editor = new JSONEditor(container, options);
      if (this.defaultMode) {
        this.editor.setMode(this.defaultMode);
      }

      return this;
    },

    changeViewToReadOnly: function() {
      this.readOnly = true;
      // breadcrumb
      $('.breadcrumb').find('a').html($('.breadcrumb').find('a').html() +
        ' (read-only)');
      // editor read only mode
      this.editor.setMode('view');
      $('.jsoneditor-modes').hide();
      // bottom buttons
      $('.bottomButtonBar button').addClass('disabled');
      $('.bottomButtonBar button').unbind('click');
      // top add document button
      $('#addDocument').addClass('disabled');
    },

    cleanupEditor: function() {
      if (this.editor) {
        this.editor.destroy();
      }
    },

    removeReadonlyKeys: function(object) {
      return _.omit(object, ['_key', '_id', '_from', '_to', '_rev']);
    },

    saveDocument: function() {
      if ($('#saveDocumentButton').attr('disabled') === undefined) {
        if (this.collection.first().attributes._id.substr(0, 1) === '_') {
          var buttons = [];
          var tableContent = [];
          tableContent.push(
            window.modalView.createReadOnlyEntry(
              'doc-save-system-button',
              'Caution',
              '您正在修改系统集合。真的继续吗？',
              undefined,
              undefined,
              false,
              /[<>&'"]/
            )
          );
          buttons.push(
            window.modalView.createSuccessButton('Save', this.confirmSaveDocument
              .bind(this))
          );
          window.modalView.show('modalTable.ejs',
            '系统数据集', buttons, tableContent);
        } else {
          this.confirmSaveDocument();
        }
      }
    },

    confirmSaveDocument: function() {
      window.modalView.hide();

      var model;

      try {
        model = this.editor.get();
      } catch (e) {
        this.errorConfirmation(e);
        this.disableSaveButton();
        return;
      }

      model = JSON.stringify(model);

      if (this.type === 'edge' || this.type._from) {
        var callbackE = function(error, data) {
          if (error) {
            avocadoHelper.avocadoError('Error', data.responseJSON.errorMessage);
          } else {
            this.successConfirmation();
            this.disableSaveButton();
          }
        }.bind(this);

        this.collection.saveEdge(this.colid, this.docid, $(
            '#document-from').html(), $('#document-to').html(), model,
          callbackE);
      } else {
        var callback = function(error, data) {
          if (error) {
            avocadoHelper.avocadoError('Error', data.responseJSON.errorMessage);
          } else {
            this.successConfirmation();
            this.disableSaveButton();
          }
        }.bind(this);

        this.collection.saveDocument(this.colid, this.docid, model,
          callback);
      }
    },

    successConfirmation: function() {
      avocadoHelper.avocadoNotification('数据已保存.');
    },

    errorConfirmation: function(e) {
      avocadoHelper.avocadoError('修改数据: ', e);
    },

    enableSaveButton: function() {
      $('#saveDocumentButton').prop('disabled', false);
      $('#saveDocumentButton').addClass('button-success');
      $('#saveDocumentButton').removeClass('button-close');
    },

    disableSaveButton: function() {
      $('#saveDocumentButton').prop('disabled', true);
      $('#saveDocumentButton').addClass('button-close');
      $('#saveDocumentButton').removeClass('button-success');
    },

    breadcrumb: function() {
      var name = window.location.hash.split('/');
      $('#subNavigationBar .breadcrumb').html(
        '<a href="#collection/' + name[1] +
        '/documents/1">Collection: ' + name[1] + '</a>' +
        '<i class="fa fa-chevron-right"></i>' +
        this.type.charAt(0).toUpperCase() + this.type.slice(1) + ': ' +
        name[2]
      );
    },

    escaped: function(value) {
      return value.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(
          />/g, '&gt;')
        .replace(/"/g, '&quot;').replace(/'/g, '&#39;');
    }
  });
}());
