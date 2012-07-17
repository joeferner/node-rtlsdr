'use strict';

var rtlsdr = require('../');

module.exports = {
  "device list, open": function (test) {
    rtlsdr.getDevices(function (err, devices) {
        if (err) {
          test.fail(err);
          return test.done();
        }
        test.equal(devices.length, 1);
        console.log(devices[0]);
        devices[0].open(function (err, device) {
          if (err) {
            test.fail(err);
            return test.done();
          }
          device.setSampleRate(2048000);
          device.setCenterFrequency(99500000);
          device.on("data", function (data) {
            console.log(data);
          });
          device.start();
          setTimeout(function () {
            device.stop();
            test.done();
          }, 10000);
        });
      }
    );
  }
};
