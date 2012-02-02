#include "test.h"

class Object {
 public:
  static const void* GetObject(void* obj, const char* key) {
    return obj;
  }

  static const void* At(void* obj, const int index) {
    return index < 10000 ? obj : NULL;
  }

  static int IsArray(void* obj) {
    return true;
  }
};

TEST_START("Template output reallocation")
  Options options(NULL,
                  Object::GetObject,
                  Object::At,
                  Object::IsArray,
                  NULL);

  Hogan hogan(&options);

  Object data;
  Template* t;
  char* out;

  t = hogan.Compile("{{#iterate}}string{{/iterate}}");

  out = t->Render(&data);
  assert(out != NULL);
  assert(strlen(out) == 6 * 10000);

  delete t;
  delete out;

TEST_END("Template output reallocation")
