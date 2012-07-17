'use strict';

var rtlsdr = require('../');

module.exports = {
  "device list": function (test) {
    rtlsdr.getDevices(function (err, devices) {
      if (err) {
        return test.fail(err);
      }
      test.equal(devices.length, 1);
      test.done();
    });
  }
};
