
# node-rtlsdr

Node wrapper around [rtl-sdr](http://sdr.osmocom.org/trac/wiki/rtl-sdr/).

Note: this is a working fork of the [node-rtlsdr](https://github.com/joeferner/node-rtlsdr) module.

You need to install the rtl-sdr libraries first:

```
$ apt-get install librtlsdr0 librtlsdr-dev
```

## Usage



```
$ mkdir node_modules
$ npm install git+https://github.com/pminervini/node-rtlsdr.git
```

```javascript
var rtlsdr = require('rtlsdr');

rtlsdr.getDevices(function (err, devices) {
  devices[0].open(function (err, device) {
    device.setSampleRate(2048000);
    device.setCenterFrequency(99500000);
    device.on("data", function (data) {
      // process data
      console.log(data);
    });
    device.start();
    setTimeout(function () {
      device.stop();
    }, 1000);
  });
});
```

Output:

```
Found Rafael Micro R820T tuner
<SlowBuffer 7d 83 81 7d 72 86 82 81 79 7f 82 85 7c 7f 82 89 80 84 7c 7e 80 8a 86 7c 7d 82 8b 82 7e 82 86 80 82 7e 7e 7f 88 7d 83 76 81 85 87 77 7e 81 81 7b 7d 79 82 ...>
<SlowBuffer 87 83 7b 7e 89 88 81 7a 7f 7f 88 82 85 7d 81 83 88 7e 7a 83 8b 7e 81 7a 83 83 88 80 84 7d 80 85 85 78 80 84 89 78 7f 7d 87 83 84 7e 7e 79 84 83 7f 76 88 ...>
<SlowBuffer 84 84 80 7d 82 81 84 81 83 7d 87 84 7d 7f 84 85 80 7b 80 84 85 7b 85 81 81 81 88 7f 80 83 84 7e 7d 80 8a 7d 7c 7c 88 81 82 7d 81 7f 84 7f 80 7b 81 7c 81 ...>
<SlowBuffer 7c 7f 7b 81 82 7d 77 88 83 81 78 82 7b 86 7e 80 7e 7f 80 86 7e 7d 81 8b 7e 7e 79 85 83 84 7e 7f 7d 81 87 85 7c 7f 84 87 7c 7f 80 88 84 7b 7e 83 84 82 89 ...>
<SlowBuffer 7a 87 82 82 79 85 7f 81 75 81 83 7f 7a 80 7d 7f 80 84 7a 7f 7e 82 76 7d 7d 81 7b 77 7c 83 7d 7b 7e 7d 79 81 7a 78 7d 81 80 74 7a 7e 86 7b 7a 7e 82 79 7b ...>
<SlowBuffer 78 83 7d 78 7a 7e 7b 80 82 76 77 83 84 80 76 7e 7e 81 79 82 78 7c 81 80 77 7d 81 86 7e 7b 7a 87 7c 84 7c 7d 78 88 7d 7b 7b 84 82 7f 7a 83 81 83 81 85 76 ...>
```
