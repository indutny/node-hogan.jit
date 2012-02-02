#include "test.h"

static bool html = false;
static char* adjective;

class Object {
 public:
  static const void* GetString(void* obj, const char* key) {
    adjective = new char[100];
    if (!html) {
      memcpy(adjective, "neat", 5);
    } else {
      memcpy(adjective, "<b>adjective</b>", 17);
    }
    return reinterpret_cast<const void*>(adjective);
  }
};

TEST_START("bench basic")
  Options options(Object::GetString,
                  NULL,
                  NULL,
                  NULL,
                  NULL);
  Hogan hogan(&options);

  Object data;
  Template* t;
  char* out;

  const int cnum = 200000;

  BENCH_START(compile, cnum)
  for (int i = 0; i < cnum; i++) {
    t = hogan.Compile("some {{adjective}} template.");
    delete t;
  }
  BENCH_END(compile, cnum)

  const int num = 12000000;

  t = hogan.Compile("some {{{adjective}}} template.");
  BENCH_START(unescaped, num)
  for (int i = 0; i < num; i++) {
    out = t->Render(&data);
    delete out;
  }
  BENCH_END(unescaped, num)
  delete t;

  t = hogan.Compile("some {{adjective}} template.");
  BENCH_START(escaped, num)
  for (int i = 0; i < num; i++) {
    out = t->Render(&data);
    delete out;
  }
  BENCH_END(escaped, num)
  delete t;

  html = true;

  t = hogan.Compile("some {{adjective}} template.");
  BENCH_START(escaped_html, num)
  for (int i = 0; i < num; i++) {
    out = t->Render(&data);
    delete out;
  }
  BENCH_END(escaped_html, num)
  delete t;

TEST_END("bench basic")
