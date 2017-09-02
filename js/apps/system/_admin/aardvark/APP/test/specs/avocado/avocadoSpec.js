/* jshint browser: true */
/* jshint unused: false */
/* global describe, isCoordinator, versionHelper, window, it, expect, spyOn, beforeEach, $, avocadoHelper */

describe('Avocado Helper', function () {
  'use strict';

  describe('Checking collection types converter', function () {
    var ajaxCB, jQueryDummy;

    beforeEach(function () {
      ajaxCB = function (url, opts) {
        if (url === 'cluster/amICoordinator') {
          expect(opts.async).toEqual(false);
          opts.success(true);
        } else if (url instanceof Object) {
          opts = url;
          if (opts.url === '/_api/database/current') {
            expect(opts.type).toEqual('GET');
            expect(opts.cache).toEqual(false);
            expect(opts.async).toEqual(false);
            expect(opts.contentType).toEqual('application/json');
            expect(opts.processData).toEqual(false);
            opts.success({result: {name: 'blub'}});
          }
        }
      };
      spyOn($, 'ajax').andCallFake(function (url, opts) {
        ajaxCB(url, opts);
      });
    });

    it('check available hotkeys function', function () {
      var dummy = {
        scrollDown: function () {
          window.scrollBy(0, 180);
        },
        scrollUp: function () {
          window.scrollBy(0, -180);
        }
      };

      expect(JSON.stringify(dummy)).toEqual(JSON.stringify(avocadoHelper.hotkeysFunctions));
    });

    it('check enabling keyboard hotkeys', function () {
      jQueryDummy = {
        on: function () {},
        keydown: function () {}
      };
      spyOn(jQueryDummy, 'on');
      spyOn(window, '$').andReturn(
        jQueryDummy
      );
      avocadoHelper.enableKeyboardHotkeys(true);
      expect(jQueryDummy.on).toHaveBeenCalled();
    });

    it('check enabling keyboard hotkeys scrollUp function', function () {
      spyOn(window, 'scrollBy');
      avocadoHelper.hotkeysFunctions.scrollDown();
      expect(window.scrollBy).toHaveBeenCalledWith(0, 180);
    });

    it('check showHotkeysModal function', function () {
      window.modalView = new window.ModalView();
      spyOn(window.modalView, 'show');
      window.avocadoHelper.allHotkeys = [1];
      avocadoHelper.hotkeysFunctions.showHotkeysModal();
      expect(window.modalView.show).toHaveBeenCalledWith(
        'modalHotkeys.ejs', 'Keyboard Shortcuts', [], [1]
      );
      delete window.modalView;
    });

    it('check enabling keyboard hotkeys scrollUp function', function () {
      spyOn(window, 'scrollBy');
      avocadoHelper.hotkeysFunctions.scrollUp();
      expect(window.scrollBy).toHaveBeenCalledWith(0, -180);
    });

    it('if blank collection name', function () {
      var myObject = {}, dummy;
      myObject.name = '';
      dummy = avocadoHelper.collectionType(myObject);
      expect(dummy).toBe('-');
    });

    it('if type document', function () {
      var myObject = {},
        dummy;
      myObject.type = 2;
      myObject.name = 'testCollection';
      dummy = avocadoHelper.collectionType(myObject);
      expect(dummy).toBe('document');
    });

    it('if type edge', function () {
      var myObject = {},
        dummy;
      myObject.type = 3;
      myObject.name = 'testCollection';
      dummy = avocadoHelper.collectionType(myObject);
      expect(dummy).toBe('edge');
    });

    it('if type unknown', function () {
      var myObject = {},
        dummy;
      myObject.type = Math.random();
      myObject.name = 'testCollection';
      dummy = avocadoHelper.collectionType(myObject);
      expect(dummy).toBe('unknown');
    });

    it('is coordinatoor', function () {
      var res;
      res = isCoordinator();
      expect(res).toBe(true);
    });

    it('versionHelper_toString', function () {
      var res;
      res = versionHelper.toString({
        major: 2,
        minor: 0,
        patch: 1
      });
      expect(res).toEqual('2.0.1');
    });

    it('currentDatabase with success', function () {
      var res;
      res = avocadoHelper.currentDatabase();
      expect(res).toEqual('blub');
    });
    it('currentDatabase with error', function () {
      var res;
      ajaxCB = function (opts) {
        if (opts.url === '/_api/database/current') {
          expect(opts.type).toEqual('GET');
          expect(opts.cache).toEqual(false);
          expect(opts.async).toEqual(false);
          expect(opts.contentType).toEqual('application/json');
          expect(opts.processData).toEqual(false);
          opts.error({result: {name: 'blub'}});
        }
      };
      res = avocadoHelper.currentDatabase();
      expect(res).toEqual(false);
    });

    it('databaseAllowed with success', function () {
      var res;
      spyOn(avocadoHelper, 'currentDatabase').andReturn('blub');
      ajaxCB = function (opts) {
        expect(opts.url).toEqual('/_db/blub/_api/database/');
        expect(opts.type).toEqual('GET');
        expect(opts.cache).toEqual(false);
        expect(opts.async).toEqual(false);
        expect(opts.contentType).toEqual('application/json');
        expect(opts.processData).toEqual(false);
        opts.success();
      };
      res = avocadoHelper.databaseAllowed();
      expect(res).toEqual(true);
    });
    it('databaseAllowed with error', function () {
      var res;
      spyOn(avocadoHelper, 'currentDatabase').andReturn('blub');
      ajaxCB = function (opts) {
        expect(opts.url).toEqual('/_db/blub/_api/database/');
        expect(opts.type).toEqual('GET');
        expect(opts.cache).toEqual(false);
        expect(opts.async).toEqual(false);
        expect(opts.contentType).toEqual('application/json');
        expect(opts.processData).toEqual(false);
        opts.error();
      };
      res = avocadoHelper.databaseAllowed();
      expect(res).toEqual(false);
    });

    it('avocadoNotification', function () {
      var notificationList = {
        add: function () {
          return undefined;
        }
      };
      window.App = {
        notificationList: notificationList
      };
      spyOn(notificationList, 'add');
      avocadoHelper.avocadoNotification('bla', 'blub');
      expect(notificationList.add).toHaveBeenCalledWith({title: 'bla', content: 'blub'});
      avocadoHelper.avocadoError('bla', 'blub');
      expect(notificationList.add).toHaveBeenCalledWith({title: 'bla', content: 'blub'});
      delete window.App;
    });

    it('randomToken', function () {
      expect(avocadoHelper.getRandomToken()).not.toEqual(undefined);
    });

    it('collectionApiType', function () {
      var adsDummy = {
          getCollectionInfo: function () {
            return {type: 3};
          }
        }, adsDummy2 = {
          getCollectionInfo: function () {
            return {type: 2};
          }
      };
      avocadoHelper.avocadoDocumentStore = adsDummy;
      expect(avocadoHelper.collectionApiType('blub', true)).toEqual('edge');
      avocadoHelper.avocadoDocumentStore = adsDummy2;
      expect(avocadoHelper.collectionApiType('blub', true)).toEqual('document');
    });
  });

  describe('checking html escaping', function () {
    var input = '<&>"\'',
      expected = '&lt;&amp;&gt;&quot;&#39;';
    expect(avocadoHelper.escapeHtml(input)).toEqual(expected);
  });
});
