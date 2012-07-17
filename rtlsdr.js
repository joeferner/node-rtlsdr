'use strict';

var util = require("util");
var events = require("events");
var rtlsdrBindings = require("bindings")("nodeRtlsdr.node");

exports.getDevices = function (callback) {
  callback = callback || function () {};
  var opts = {};
  opts.toEventEmitter = function (clazz) {
    util.inherits(clazz, events.EventEmitter);
  };
  rtlsdrBindings.getDevices(opts, callback);
};
