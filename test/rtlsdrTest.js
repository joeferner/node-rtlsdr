'use strict';

var rtlsdr = require('../');

module.exports = {
  "device list, open": function (test) {
    rtlsdr.getDevices(function (err, devices) {
      if (err) {
        return test.fail(err);
      }
      test.equal(devices.length, 1);
      devices[0].open(function (err, device) {
        if (err) {
          return test.fail(err);
        }
        device.setSampleRate(2048000);
        device.setCenterFrequency(99500000);
        device.start();
        setTimeout(function () {
          device.stop();
          test.done();
        }, 1000);
      });
    });
  }
};
