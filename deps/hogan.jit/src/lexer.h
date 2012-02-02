#ifndef _SRC_LEXER_H_
#define _SRC_LEXER_H_

#include <stdint.h> // uint32_t
#include <stdlib.h> // NULL

namespace hogan {

class Lexer {
 public:
  class Token {
   public:
    enum TokenType {
      kString,
      kProp,
      kRawProp,
      kRawTaggedProp,
      kIf,
      kElse,
      kEndIf,
      kPartial,
      kComment,
      kEnd
    };

    Token(TokenType type_) : type(type_), value(NULL), length(0) {
    }

    Token(TokenType type_, const char* value_, uint32_t length_) : type(type_) {
      value = value_;
      length = length_;

      // Trim non-string tokens
      if (type != kString) {
        // Remove spaces from start
        while (value[0] == ' ' && length > 0) {
          value++;
          length--;
        }
        // And from end
        while (value[length - 1] == ' ' && length > 0) {
          length--;
        }
      }
    }
    ~Token() {}

    TokenType type;
    const char* value;
    uint32_t length;
  };

  Lexer(const char* source_, uint32_t length_) : source(source_),
                                                 offset(0),
                                                 length(length_) {
  }
  ~Lexer() {}

  Token* Consume() {
    if (offset == length) return new Token(Token::kEnd);

    // Consume string
    if (source[offset] != '{' ||
        offset + 1 >= length ||
        source[offset + 1] != '{') {

      uint32_t start = offset++;
      while (offset < length) {
        if (offset + 1 < length &&
            source[offset] == '{' &&
            source[offset + 1] == '{') {
          break;
        }
        offset++;
      }
      return new Token(Token::kString, source + start, offset - start);
    }

    // Skip '{{'
    offset += 2;
    if (offset == length) return new Token(Token::kEnd);

    // Handle modificators: '#', '^', '/'
    Token::TokenType type;
    do {
      if (source[offset] == '#') {
        type = Token::kIf;
      } else if (source[offset] == '^') {
        type = Token::kElse;
      } else if (source[offset] == '/') {
        type = Token::kEndIf;
      } else if (source[offset] == '>') {
        type = Token::kPartial;
      } else if (source[offset] == '!') {
        type = Token::kComment;
      } else if (source[offset] == '{') {
        type = Token::kRawProp;
      } else if (source[offset] == '&') {
        type = Token::kRawTaggedProp;
      } else {
        type = Token::kProp;
        break;
      }
      offset++;
    } while(0);

    // Parse until '}}'
    uint32_t start = offset;
    while (offset + 2 < length &&
           (source[offset + 1] != '}' || source[offset + 2] != '}')) {
      offset++;
    }

    // '{{...' without '}}' or '{{}}'
    if (offset + 2 >= length) return new Token(Token::kEnd);
    offset++;

    // '}}}' for kRawProp
    if (type == Token::kRawProp &&
        (offset + 2 >= length || source[offset + 2] != '}')) {
      return new Token(Token::kEnd);
    }

    // kRawTaggedProp is essentially the same as kRawProp
    Token* prop;
    if (type != Token::kRawTaggedProp) {
      prop = new Token(type, source + start, offset - start);
    } else {
      prop = new Token(Token::kRawProp, source + start, offset - start);
    }

    if (type != Token::kRawProp) {
      // Skip '}}'
      offset += 2;
    } else {
      // Skip '}}}' for kRawProp
      offset += 3;
    }

    return prop;
  }

 private:
  const char* source;
  uint32_t offset;
  uint32_t length;
};

} // namespace hogan

#endif // _SRC_LEXER_H_
