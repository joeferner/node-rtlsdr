
#include "nodeRtlSdr.h"
#include <rtl-sdr.h>

v8::Handle<v8::Value> GetDevices(const v8::Arguments& args) {
  v8::HandleScope scope;
  v8::Handle<v8::Value> callbackArgs[2];
  int deviceCount, i;
  char vendor[256], product[256], serial[256], str[1000];
  const char* deviceName;
  int err;
  v8::Local<v8::Array> deviceArray;
  v8::Local<v8::Object> deviceInfo;
  callbackArgs[0] = v8::Undefined();
  callbackArgs[1] = v8::Undefined();

  // callback
  if(!args[0]->IsFunction()) {
    return scope.Close(v8::ThrowException(v8::Exception::TypeError(v8::String::New("First argument must be a function"))));
  }
  v8::Local<v8::Value> callback = args[0];

  deviceArray = v8::Array::New();
  deviceCount = rtlsdr_get_device_count();
  for(i = 0; i < deviceCount; i++) {
    err = rtlsdr_get_device_usb_strings(i, vendor, product, serial);
    if(err) {
      sprintf(str, "Could not get device strings");
      callbackArgs[0] = v8::Exception::TypeError(v8::String::New(str));
      goto getDevicesDone;
    }
    deviceName = rtlsdr_get_device_name(i);
    deviceInfo = v8::Object::New();
    deviceArray->Set(i, deviceInfo);
  }

  callbackArgs[1] = deviceArray;

getDevicesDone:
  v8::Function::Cast(*callback)->Call(v8::Context::GetCurrent()->Global(), 2, callbackArgs);
  return scope.Close(v8::Undefined());
}
