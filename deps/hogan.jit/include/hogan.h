#ifndef _HOGAN_H_
#define _HOGAN_H_

namespace hogan {

class Template;
class TemplateCode;
class Compiler;
class Codegen;
class Options;

class Hogan {
 public:
  Hogan(Options* options);
  ~Hogan();

  Template* Compile(const char* source);
 private:
  Options* options_;
};

typedef const void* (*PropertyCallback)(void* obj, const char* key);
typedef const void* (*NumericPropertyCallback)(void* obj, const int index);
typedef int (*IsArrayCallback)(void* obj);
typedef Template* (*PartialCallback)(Template* tpl, const char* name);

class Options {
 public:
  Options(PropertyCallback getString_,
          PropertyCallback getObject_,
          NumericPropertyCallback at_,
          IsArrayCallback isArray_,
          PartialCallback getPartial_);
  Options();

  PropertyCallback getString;
  PropertyCallback getObject;
  NumericPropertyCallback at;
  IsArrayCallback isArray;
  PartialCallback getPartial;
};


class Template {
 public:
  Template();
  ~Template();

  char* Render(void* obj);

 private:
  TemplateCode* code;
  friend class Compiler;
  friend class Codegen;
};

} // namespace hogan

#endif // _HOGAN_H_
