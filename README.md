
# rtlsdr

Node wrapper around [rtl-sdr](http://sdr.osmocom.org/trac/wiki/rtl-sdr/)

**Note: This module has not been tested on windows. If you would like to submit a pull request that would be great.**

**Note: You must install rtl-sdr libs first.**

## Example

```javascript
var rtlsdr = require('rtlsdr');

rtlsdr.getDevices(function (err, devices) {
});
```
