#ifndef _SRC_NODE_HOGAN_H_
#define _SRC_NODE_HOGAN_H_

#include <node.h>
#include <v8.h>
#include <hogan.h>

using namespace node;
using namespace v8;


static hogan::Hogan* api;

class HoganTemplate : public ObjectWrap {
 public:
  static void Initialize(Handle<Object> target);

  static const void* GetString(void* obj, const char* key);
  static const void* GetObject(void* obj, const char* key);
  static const void* ObjectAt(void* obj, const int index);
  static int IsArray(void* obj);

  static Handle<Value> New(const Arguments& args);
  static Handle<Value> Render(const Arguments& args);

  HoganTemplate(char* source) {
    tpl = api->Compile(source);
  }

  ~HoganTemplate() {
    delete tpl;
  }

 protected:
  hogan::Template* tpl;
};

#endif // _SRC_NODE_HOGAN_H_
