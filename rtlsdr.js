'use strict';

var rtlsdrBindings = require("bindings")("nodeRtlsdr.node");

exports.getDevices = function (callback) {
  callback = callback || function () {};
  rtlsdrBindings.getDevices(callback);
};
