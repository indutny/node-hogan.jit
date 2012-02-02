#ifndef _SRC_ASM_H_
#define _SRC_ASM_H_

#include "hogan.h"
#include "parser.h"
#include "queue.h" // Queue
#include "output.h" // TemplateOutput

#include <stdint.h> // uint32_t
#include <sys/types.h> // size_t

namespace hogan {

class Code;
class Template;
class TemplateCode;

typedef size_t (*TemplateFunction)(void* obj,
                                   TemplateOutput* out,
                                   Template* tpl);

class Compiler {
 public:
  static Template* Compile(AstNode* ast, Options* options, const char* source);
};

class Code {
 public:
  Code();
  ~Code();

  void Grow();
  void Commit();

  uint32_t size;
  uint32_t page_size;

  char* code;
  char* guard;
  bool committed;

  Queue<char*>* data;
};

class TemplateCode : public Code {
 public:
  TemplateCode() {
  }

  ~TemplateCode() {
    delete source;
  }

  inline TemplateFunction AsFunction() {
    return reinterpret_cast<TemplateFunction>(this->code);
  }

  // Just for reference
  const char* source;
};


} // namespace hogan

#endif // _SRC_ASM_H_
