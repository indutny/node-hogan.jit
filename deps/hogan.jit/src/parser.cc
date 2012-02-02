#include "parser.h"

#include <assert.h> // assert

namespace hogan {

void Parser::Parse() {
  Lexer::Token* tok = Consume();

  while (tok->type != Lexer::Token::kEnd) {
    switch (tok->type) {
     case Lexer::Token::kString:
      Insert(AstNode::kString, tok);
      break;
     case Lexer::Token::kProp:
      Insert(AstNode::kProp, tok);
      break;
     case Lexer::Token::kRawProp:
      Insert(AstNode::kRawProp, tok);
      break;
     case Lexer::Token::kPartial:
      Insert(AstNode::kPartial, tok);
      break;
     case Lexer::Token::kIf:
      Enter(AstNode::kIf);
      SetValue(tok);
      Enter(AstNode::kBlock);
      break;
     case Lexer::Token::kElse:
      Leave(AstNode::kBlock);
      Enter(AstNode::kBlock);
      SetValue(tok);
      break;
     case Lexer::Token::kEndIf:
      Leave(AstNode::kBlock);
      Leave(AstNode::kIf);
      break;
     case Lexer::Token::kComment:
      break;
     default:
      assert(false && "Unexpected lexer token");
    }

    delete tok;
    tok = Consume();
  }

  // Delete kEnd token too
  delete tok;
}

} // namespace hogan
