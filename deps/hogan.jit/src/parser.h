#ifndef _SRC_PARSER_H_
#define _SRC_PARSER_H_

#include "lexer.h"
#include "queue.h"
#include <assert.h> // assert
#include <stdlib.h> // NULL

namespace hogan {

class AstNode {
 public:
  enum AstNodeType {
    kBlock,
    kString,
    kProp,
    kRawProp,
    kIf,
    kPartial
  };

  AstNode(AstNodeType type_) : type(type_),
                               value(NULL),
                               length(0),
                               ascendant(NULL) {
  }
  ~AstNode() {
    ascendant = NULL;
  }

  inline void Push(AstNode* node) {
    descendants.Push(node);
  }

  inline AstNode* Shift() {
    return descendants.Shift();
  }

  void SetAscendant(AstNode* node) {
    assert(ascendant == NULL);
    ascendant = node;
  }

  AstNode* GetAscendant() {
    return ascendant;
  }

  AstNodeType type;
  const void* value;
  uint32_t length;

 private:
  Queue<AstNode*> descendants;
  AstNode* ascendant;
};


class Parser : Lexer {
 public:
  Parser(const char* source, uint32_t length) : Lexer(source, length) {
    ast = new AstNode(AstNode::kBlock);
    current = ast;
  }
  ~Parser() {}

  void Parse();

  AstNode* Result() {
    return ast;
  }

  void Enter(AstNode::AstNodeType type) {
    AstNode* node = new AstNode(type);
    node->SetAscendant(current);
    current->Push(node);
    current = node;
  }

  void SetValue(const Lexer::Token* token) {
    current->value = token->value;
    current->length = token->length;
  }

  void Leave(AstNode::AstNodeType type) {
    assert(current->type == type);
    current = current->GetAscendant();
  }

  void Insert(AstNode::AstNodeType type, const Lexer::Token* token) {
    Enter(type);
    SetValue(token);
    Leave(type);
  }

 private:
  AstNode* ast;
  AstNode* current;
};

} // namespace hogan

#endif // _SRC_PARSER_H_
