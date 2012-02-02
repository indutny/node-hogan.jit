#include "test.h"

static Template* partial = NULL;

Template* GetPartial(Template* t, const char* name) {
  assert(strcmp(name, "partial") == 0);
  return partial;
};

TEST_START("Partials")
  Options options;
  options.getPartial = GetPartial;

  Hogan hogan(&options);

  void* data = NULL;

  Template* t;
  char* out;

  partial = hogan.Compile(" partial ");
  t = hogan.Compile("string{{>partial}}string");

  out = t->Render(data);
  assert(out != NULL);
  assert(strcmp("string partial string", out) == 0);

  delete partial;
  delete t;
  delete out;

TEST_END("Partials")
