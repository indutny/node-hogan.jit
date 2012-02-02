#include "test.h"

static const char* arrRef = "neat";

class Object {
 public:
  static const void* GetString(void* obj, const char* key) {
    char* value = new char[10];
    if (strcmp(key, "adjective") == 0) {
      memcpy(value, "neat", 5);
      return value;
    } else if (strcmp(key, "html") == 0) {
      memcpy(value, "&<>'\"", 6);
      return value;
    } else {
      assert(0 && "unexpected");
      return NULL;
    }
  }
  static const void* GetObject(void* obj, const char* key) {
    if (strcmp(key, "prop") == 0) {
      return obj;
    } else if (strcmp(key, "arrprop") == 0) {
      return reinterpret_cast<const void*>(arrRef);
    } else {
      return NULL;
    }
  }

  static const void* At(void* obj, const int index) {
    return index < 3 ? obj : NULL;
  }

  static int IsArray(void* obj) {
    return obj == reinterpret_cast<const void*>(arrRef);
  }
};

TEST_START("API test")
  Options options(Object::GetString,
                  Object::GetObject,
                  Object::At,
                  Object::IsArray,
                  NULL);
  Hogan hogan(&options);

  Object data;
  Template* t;
  char* out;

  t = hogan.Compile("some {{adjective}} template. "
                    "{{#prop}}yeah{{^prop}}oh noes{{/prop}}");

  out = t->Render(&data);
  assert(out != NULL);
  assert(strcmp("some neat template. yeah", out) == 0);

  delete t;
  delete out;

  t = hogan.Compile("some {{adjective}} template."
                    "{{#arrprop}} o{{^arrprop}}oh noes{{/arrprop}}");

  out = t->Render(&data);
  assert(out != NULL);
  assert(strcmp("some neat template. o o o", out) == 0);

  delete t;
  delete out;

  t = hogan.Compile("some {{  adjective   }} template. "
                     "{{#nprop}}yeah{{^nprop}}oh noes{{/nprop}}");

  out = t->Render(&data);
  assert(out != NULL);
  assert(strcmp("some neat template. oh noes", out) == 0);

  delete out;
  delete t;

  t = hogan.Compile("some template with{{!  comments   }}.");

  out = t->Render(&data);
  assert(out != NULL);
  assert(strcmp("some template with.", out) == 0);

  delete out;
  delete t;

  t = hogan.Compile("escaped value {{html}}.");

  out = t->Render(&data);
  assert(out != NULL);
  assert(strcmp("escaped value &amp;&lt;&gt;&apos;&quot;.", out) == 0);

  delete out;
  delete t;

  t = hogan.Compile("escaped value {{{html}}}.");

  out = t->Render(&data);
  assert(out != NULL);
  assert(strcmp("escaped value &<>'\".", out) == 0);

  delete out;
  delete t;

  t = hogan.Compile("escaped value {{& html }}.");

  out = t->Render(&data);
  assert(out != NULL);
  assert(strcmp("escaped value &<>'\".", out) == 0);

  delete out;
  delete t;


TEST_END("API test")
