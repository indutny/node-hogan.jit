#include "hogan.h"
#include "compiler.h"
#include "codegen.h"
#include "output.h" // TemplateOutput

#include <assert.h> // assert
#include <stdlib.h> // NULL, abort
#include <string.h> // memset
#include <sys/mman.h> // mmap, munmap
#include <sys/types.h> // size_t
#include <stdint.h> // uint32_t

#include <unistd.h> // sysconf or getpagesize

namespace hogan {

Template* Compiler::Compile(AstNode* ast,
                            Options* options,
                            const char* source) {
  Template* t = new Template();

  t->code->source = source;
  Codegen codegen(t->code, options);

  codegen.GeneratePrologue();
  codegen.GenerateBlock(ast);
  codegen.GenerateEpilogue();

  t->code->data = codegen.data;

  // Copy buffered code into executable memory
  // and guard it
  t->code->Commit();

  return t;
}


char* Template::Render(void* obj) {
  TemplateOutput out;
  code->AsFunction()(obj, &out, this);

  return out.Join();
};


Template::Template() {
  code = new TemplateCode();
}


Template::~Template() {
  delete code;
}


Code::Code() {
#ifdef __DARWIN
  page_size = getpagesize();
#else
  page_size = sysconf(_SC_PAGE_SIZE);
#endif

  size = page_size;
  guard = NULL;

  code = new char[size];
  committed = false;
  memset(code, 0x90, size);
}


Code::~Code() {
  if (committed) {
    if (munmap(code, static_cast<size_t>(size)) != 0) abort();
    if (munmap(guard, static_cast<size_t>(page_size)) != 0) abort();
  } else {
    delete code;
  }

  char* chunk;
  while ((chunk = data->Shift()) != NULL) delete chunk;
  delete data;
}


void Code::Grow() {
  uint32_t new_size = size << 1;
  char* new_code = new char[new_size];

  memcpy(new_code, code, size);
  memset(new_code + size, 0x90, size);

  delete code;
  code = new_code;
}


void Code::Commit() {
  void* data = mmap(0,
                    size,
                    PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_ANON | MAP_PRIVATE,
                    -1,
                    0);

  if (data == MAP_FAILED) abort();

  memcpy(data, code, size);
  delete code;
  code = reinterpret_cast<char*>(data);

  // Allocate guard
  data = mmap(code + size,
              page_size,
              PROT_NONE,
              MAP_ANON | MAP_PRIVATE,
              -1,
              0);
  if (data == MAP_FAILED) abort();

  guard = reinterpret_cast<char*>(data);

  committed = true;
}


} // namespace hogan
