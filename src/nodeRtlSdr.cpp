
#include "nodeRtlSdr.h"
#include <rtl-sdr.h>

#define DEFAULT_ASYNC_BUF_NUMBER 32
#define DEFAULT_BUF_LENGTH       (16 * 16384)

v8::Persistent<v8::Function> g_deviceConstructor;
int g_initialized = false;

v8::Handle<v8::Value> device_open(const v8::Arguments& args);
v8::Handle<v8::Value> device_setSampleRate(const v8::Arguments& args);
v8::Handle<v8::Value> device_setCenterFrequency(const v8::Arguments& args);
v8::Handle<v8::Value> device_start(const v8::Arguments& args);
v8::Handle<v8::Value> device_stop(const v8::Arguments& args);
void device_cleanUp(v8::Persistent<v8::Value> obj, void *parameter);
static void device_dataCallback(unsigned char *buf, uint32_t len, void *ctx);

struct DeviceData {
  rtlsdr_dev_t *dev;
  v8::Persistent<v8::Object> v8dev;
};

struct DataEventData {
  DeviceData* devData;
  unsigned char* buf;
  int len;
};

struct StartData {
  DeviceData* devData;
  v8::Persistent<v8::Value> callback;
};

v8::Handle<v8::Value> GetDevices(const v8::Arguments& args) {
  v8::HandleScope scope;
  int deviceCount, i;
  char vendor[256], product[256], serial[256], str[1000];
  const char* deviceName;
  int err;
  v8::Local<v8::Array> deviceArray;
  v8::Local<v8::Object> device;
  v8::Handle<v8::Value> callbackArgs[2];

  callbackArgs[0] = v8::Undefined();
  callbackArgs[1] = v8::Undefined();

  // options
  if(!args[0]->IsObject()) {
    return scope.Close(v8::ThrowException(v8::Exception::TypeError(v8::String::New("First argument must be an object"))));
  }
  v8::Local<v8::Object> options = args[0]->ToObject();

  // callback
  if(!args[1]->IsFunction()) {
    return scope.Close(v8::ThrowException(v8::Exception::TypeError(v8::String::New("Second argument must be a function"))));
  }
  v8::Local<v8::Value> callback = args[1];

  if(!g_initialized) {
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New();
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(v8::String::NewSymbol("RtlsdrDevice"));
    g_deviceConstructor = v8::Persistent<v8::Function>::New(t->GetFunction());

    v8::Handle<v8::Value> toEventEmitterArgs[1];
    toEventEmitterArgs[0] = g_deviceConstructor;
    v8::Local<v8::Value> toEventEmitterFn = options->Get(v8::String::New("toEventEmitter"));
    v8::Function::Cast(*toEventEmitterFn)->Call(options, 1, toEventEmitterArgs);

    g_initialized = true;
  }

  deviceArray = v8::Array::New();
  deviceCount = rtlsdr_get_device_count();
  for(i = 0; i < deviceCount; i++) {
    vendor[0] = '\0';
    product[0] = '\0';
    serial[0] = '\0';
    err = rtlsdr_get_device_usb_strings(i, vendor, product, serial);
    if(err) {
      sprintf(str, "Could not get device strings");
      callbackArgs[0] = v8::Exception::Error(v8::String::New(str));
      goto getDevicesDone;
    }
    deviceName = rtlsdr_get_device_name(i);
    device = g_deviceConstructor->NewInstance();
    device->Set(v8::String::New("vendor"), v8::String::New(vendor));
    device->Set(v8::String::New("product"), v8::String::New(product));
    device->Set(v8::String::New("serial"), v8::String::New(serial));
    device->Set(v8::String::New("name"), v8::String::New(deviceName));
    device->Set(v8::String::New("index"), v8::Int32::New(i));
    device->Set(v8::String::New("open"), v8::FunctionTemplate::New(device_open)->GetFunction());
    device->Set(v8::String::New("setSampleRate"), v8::FunctionTemplate::New(device_setSampleRate)->GetFunction());
    device->Set(v8::String::New("setCenterFrequency"), v8::FunctionTemplate::New(device_setCenterFrequency)->GetFunction());
    device->Set(v8::String::New("start"), v8::FunctionTemplate::New(device_start)->GetFunction());
    device->Set(v8::String::New("stop"), v8::FunctionTemplate::New(device_stop)->GetFunction());
    deviceArray->Set(i, device);
  }

  callbackArgs[1] = deviceArray;

getDevicesDone:
  v8::Function::Cast(*callback)->Call(v8::Context::GetCurrent()->Global(), 2, callbackArgs);
  return scope.Close(v8::Undefined());
}

v8::Handle<v8::Value> device_open(const v8::Arguments& args) {
  v8::HandleScope scope;
  int deviceIdx;
  int err;
  DeviceData* deviceData;
  rtlsdr_dev_t *dev;
  v8::Local<v8::Object> device = args.This();
  char str[1000];
  v8::Handle<v8::Value> callbackArgs[2];

  callbackArgs[0] = v8::Undefined();
  callbackArgs[1] = v8::Undefined();

  // callback
  if(!args[0]->IsFunction()) {
    return scope.Close(v8::ThrowException(v8::Exception::Error(v8::String::New("First argument must be a function"))));
  }
  v8::Local<v8::Value> callback = args[0];

  deviceIdx = device->Get(v8::String::New("index"))->ToInt32()->Value();
  err = rtlsdr_open(&dev, deviceIdx);
  if(err < 0) {
    sprintf(str, "Could not open device err:%d.", err);
    callbackArgs[0] = v8::Exception::Error(v8::String::New(str));
    goto deviceOpenDone;
  }

  deviceData = new DeviceData();
  deviceData->dev = dev;
  device->SetPointerInInternalField(0, deviceData);
  deviceData->v8dev = v8::Persistent<v8::Object>::New(device);
  deviceData->v8dev.MakeWeak(deviceData, device_cleanUp);
  deviceData->v8dev.MarkIndependent();

  callbackArgs[1] = deviceData->v8dev;

deviceOpenDone:
  v8::Function::Cast(*callback)->Call(v8::Context::GetCurrent()->Global(), 2, callbackArgs);
  return scope.Close(v8::Undefined());
}

v8::Handle<v8::Value> device_setSampleRate(const v8::Arguments& args) {
  v8::HandleScope scope;
  char str[1000];
  v8::Local<v8::Object> device = args.This();
  DeviceData* data = (DeviceData*)device->GetPointerFromInternalField(0);

  int sampleRate = args[0]->ToInt32()->Value();

  int err = rtlsdr_set_sample_rate(data->dev, sampleRate);
  if(err < 0) {
    sprintf(str, "failed to set sample rate (err: %d)", err);
    return scope.Close(v8::ThrowException(v8::Exception::Error(v8::String::New(str))));
  }

  return scope.Close(v8::Undefined());
}

v8::Handle<v8::Value> device_setCenterFrequency(const v8::Arguments& args) {
  v8::HandleScope scope;
  char str[1000];
  v8::Local<v8::Object> device = args.This();
  DeviceData* data = (DeviceData*)device->GetPointerFromInternalField(0);

  int frequency = args[0]->ToInt32()->Value();

  int err = rtlsdr_set_center_freq(data->dev, frequency);
  if(err < 0) {
    sprintf(str, "failed to set frequency (err: %d)", err);
    return scope.Close(v8::ThrowException(v8::Exception::Error(v8::String::New(str))));
  }

  return scope.Close(v8::Undefined());
}

void EIO_Start(uv_work_t* req) {
  StartData* startData = (StartData*)req->data;

printf("starting\n");
  int err = rtlsdr_read_async(startData->devData->dev, device_dataCallback, (void*)startData->devData, DEFAULT_ASYNC_BUF_NUMBER, DEFAULT_BUF_LENGTH);
  if(err) {
    char str[1000];
    sprintf(str, "failed to start read async (err: %d)", err);
    printf("error starting: %s", str);
    // TODO callback with error
    return;
  }

  printf("started");
}

void EIO_StartAfter(uv_work_t* req) {
  v8::HandleScope scope;
  StartData* startData = (StartData*)req->data;

  // TODO startData->callback.Dispose();
  delete startData;
  printf("started2");
}

v8::Handle<v8::Value> device_start(const v8::Arguments& args) {
  v8::HandleScope scope;
  char str[1000];
  v8::Local<v8::Object> device = args.This();
  DeviceData* data = (DeviceData*)device->GetPointerFromInternalField(0);
  StartData* startData;

  // callback
  /* TODO
  if(!args[0]->IsFunction()) {
    return scope.Close(v8::ThrowException(v8::Exception::TypeError(v8::String::New("First argument must be a function"))));
  }
  v8::Local<v8::Value> callback = args[0];
  */

  int err = rtlsdr_reset_buffer(data->dev);
  if(err) {
    sprintf(str, "failed to reset buffer (err: %d)", err);
    return scope.Close(v8::ThrowException(v8::Exception::Error(v8::String::New(str))));
  }

  startData = new StartData();
  startData->devData = data;
  // TODO startData->callback = v8::Persistent<v8::Value>::New(callback);

  uv_work_t* req = new uv_work_t();
  req->data = startData;
  uv_queue_work(uv_default_loop(), req, EIO_Start, EIO_StartAfter);

  return scope.Close(v8::Undefined());
}

v8::Handle<v8::Value> device_stop(const v8::Arguments& args) {
  v8::HandleScope scope;
  v8::Local<v8::Object> device = args.This();
  DeviceData* data = (DeviceData*)device->GetPointerFromInternalField(0);

  rtlsdr_cancel_async(data->dev);

  return scope.Close(v8::Undefined());
}

void EIO_EmitData(uv_work_t* req) {

}

void EIO_EmitDataAfter(uv_work_t* req) {
  v8::HandleScope scope;
  DataEventData* eventData = (DataEventData*)req->data;

  v8::Handle<v8::Value> emitArgs[2];
  emitArgs[0] = v8::String::New("data");
  emitArgs[1] = v8::Local<v8::Object>::New(node::Buffer::New((char*)eventData->buf, eventData->len)->handle_);
  v8::Function::Cast(*eventData->devData->v8dev->Get(v8::String::New("emit")))->Call(eventData->devData->v8dev, 2, emitArgs);

  delete eventData;
}

static void device_dataCallback(unsigned char* buf, uint32_t len, void *ctx) {
printf("data %d\n", len);
  DeviceData* devData = (DeviceData*)ctx;
  DataEventData* eventData = new DataEventData();
  eventData->devData = devData;
  eventData->buf = buf;
  eventData->len = len;

  uv_work_t* req = new uv_work_t();
  req->data = eventData;
  uv_queue_work(uv_default_loop(), req, EIO_EmitData, EIO_EmitDataAfter);
}

void device_cleanUp(v8::Persistent<v8::Value> obj, void *parameter) {
  DeviceData* data = (DeviceData*)parameter;

  rtlsdr_close(data->dev);
  delete data;
}
