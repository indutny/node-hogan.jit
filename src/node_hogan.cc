#include <node.h>
#include <v8.h>
#include <string.h> // memcpy
#include <hogan.h>

#include "node_hogan.h"


using namespace node;
using namespace v8;


#define UNWRAP\
    Object* o = reinterpret_cast<Object*>(obj);


void HoganTemplate::Initialize(Handle<Object> target) {
  hogan::Options options;

  options.getString = HoganTemplate::GetString;
  options.getObject = HoganTemplate::GetObject;
  options.at = HoganTemplate::ObjectAt;
  options.isArray = HoganTemplate::IsArray;

  api = new hogan::Hogan(&options);

  Local<FunctionTemplate> t = FunctionTemplate::New(HoganTemplate::New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(String::NewSymbol("Template"));

  NODE_SET_PROTOTYPE_METHOD(t, "render", HoganTemplate::Render);

  target->Set(String::NewSymbol("Template"), t->GetFunction());
}


const void* HoganTemplate::GetString(void* obj, const char* key) {
  UNWRAP
  Handle<Value> value = o->Get(String::NewSymbol(key));
  if (value->IsUndefined()) return NULL;

  String::Utf8Value val(value->ToString());
  char* str = new char[val.length()];
  memcpy(str, *val, val.length());

  return str;
}


const void* HoganTemplate::GetObject(void* obj, const char* key) {
  UNWRAP
  Handle<Value> value = o->Get(String::NewSymbol(key));
  if (value->IsUndefined()) return NULL;
  if (value->IsFalse()) return NULL;

  return reinterpret_cast<void*>(*value->ToObject());
}


const void* HoganTemplate::ObjectAt(void* obj, const int index) {
  UNWRAP
  Local<Array> arr = Array::Cast(o);
  if (static_cast<uint32_t>(index) >= arr->Length()) return NULL;

  Handle<Value> value = arr->Get(index);
  if (value->IsUndefined()) return NULL;

  return reinterpret_cast<void*>(*value->ToObject());
}


int HoganTemplate::IsArray(void* obj) {
  UNWRAP

  return o->IsArray();
}


Handle<Value> HoganTemplate::New(const Arguments& args) {
  HandleScope scope;

  String::Utf8Value v(args[0].As<String>());
  HoganTemplate* t = new HoganTemplate(*v);

  t->Wrap(args.Holder());

  return Undefined();
}


Handle<Value> HoganTemplate::Render(const Arguments& args) {
  HandleScope scope;

  HoganTemplate* t = ObjectWrap::Unwrap<HoganTemplate>(args.This());

  Local<Object> obj = args[0].As<Object>();
  char* out = t->tpl->Render(reinterpret_cast<void*>(*obj));

  return String::New(out);
}


NODE_MODULE(hogan, HoganTemplate::Initialize);
