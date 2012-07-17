
#include "nodeRtlSdr.h"

extern "C" {
  void init (v8::Handle<v8::Object> target)
  {
    v8::HandleScope scope;
    NODE_SET_METHOD(target, "getDevices", GetDevices);
  }
}

NODE_MODULE(rtlsdr, init);
