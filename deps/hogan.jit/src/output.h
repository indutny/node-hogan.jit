#ifndef _SRC_OUTPUT_H_
#define _SRC_OUTPUT_H_

#include <assert.h> // assert
#include <sys/types.h> // size_t
#include <string.h> // strlen, memcpy
#include <stdio.h> // strlen, memcpy

namespace hogan {

const int initialCapacity = 128;

class TemplateOutput {
 public:
  enum ValueFlags {
    kNone = 0,
    kEscape = 1,
    kAllocated = 2
  };

  TemplateOutput() {
    chunks = new size_t[initialCapacity << 2];
    capacity = initialCapacity;
    total = 0;
    items = 0;
  }

  ~TemplateOutput() {
    delete chunks;
  }

  inline const char* Escape(const char* value,
                            size_t length,
                            size_t* escaped_length) {
    size_t i;

    // Fast case: no symbols to escape, just return NULL
    for (i = 0; i < length; i++) {
      if (value[i] == '&' || value[i] == '<' || value[i] == '>' ||
          value[i] == '"' || value[i] == '\'') {
        break;
      }
    }
    if (i == length) return NULL;


    // Slow case: create new string and insert escaped symbols in it
    size_t newlen = length << 1;
    char* escaped = new char[newlen];

    size_t offset = 0;
    for (i = 0; i < length; i++) {
      // &quot; - is 6 bytes in length
      // be sure that we have enough space to encode it
      if (offset + 6 >= newlen) {
        // If not - double size of buffer
        char* tmp = new char[newlen << 1];
        memcpy(tmp, escaped, offset);
        newlen = newlen << 1;
        delete escaped;
        escaped = tmp;
      }

      switch(value[i]) {
       case '&':
        escaped[offset] = '&';
        escaped[offset + 1] = 'a';
        escaped[offset + 2] = 'm';
        escaped[offset + 3] = 'p';
        escaped[offset + 4] = ';';
        offset += 5;
        break;
       case '<':
        escaped[offset] = '&';
        escaped[offset + 1] = 'l';
        escaped[offset + 2] = 't';
        escaped[offset + 3] = ';';
        offset += 4;
        break;
       case '>':
        escaped[offset] = '&';
        escaped[offset + 1] = 'g';
        escaped[offset + 2] = 't';
        escaped[offset + 3] = ';';
        offset += 4;
        break;
       case '"':
        escaped[offset] = '&';
        escaped[offset + 1] = 'q';
        escaped[offset + 2] = 'u';
        escaped[offset + 3] = 'o';
        escaped[offset + 4] = 't';
        escaped[offset + 5] = ';';
        offset += 6;
        break;
       case '\'':
        escaped[offset] = '&';
        escaped[offset + 1] = 'a';
        escaped[offset + 2] = 'p';
        escaped[offset + 3] = 'o';
        escaped[offset + 4] = 's';
        escaped[offset + 5] = ';';
        offset += 6;
        break;
       default:
        escaped[offset] = value[i];
        offset++;
      }
    }

    *escaped_length = offset;
    return escaped;
  }

  inline void Realloc() {
    if (capacity != 0) return;

    // Reallocate chunks
    size_t* new_chunks = new size_t[items << 1];
    memcpy(new_chunks, chunks, items * sizeof(*chunks));
    delete chunks;
    chunks = new_chunks;

    // Increase capactiy and size
    capacity += items >> 2;
  }

  void Push(const char* chunk, const size_t size, const size_t flags) {
    const char* value = chunk;
    size_t size_ = size == 0 ? strlen(chunk) : size;
    size_t flags_ = flags;

    if ((flags_ & kEscape) == kEscape) {
      const char* tmp;
      tmp = Escape(value, size_, &size_);
      if (tmp != NULL) {
        value = tmp;
        flags_ |= kAllocated;
      }
    }

    chunks[items] = reinterpret_cast<const size_t>(value);
    chunks[items + 1] = size_;
    chunks[items + 2] = flags_;

    items += 4;
    capacity--;
    total += size_;
    Realloc();
  }

  inline char* Join() {
    char* result = new char[total + 1];
    result[total] = 0;

    off_t offset = 0;
    for (size_t i = 0; i < items; i += 4) {
      char* value = reinterpret_cast<char*>(chunks[i]);
      size_t size = chunks[i + 1];
      size_t flags = chunks[i + 2];

      memcpy(result + offset, value, size);
      offset += size;

      if ((flags & kAllocated) == kAllocated) {
        delete value;
      }
    }
    assert(static_cast<size_t>(offset) == total);

    return result;
  }

 private:
  size_t* chunks;
  size_t capacity; // how many item we can insert
  size_t items; // count of items
  size_t total; // total byte (sum of chunks' lengths)
};

} // namespace hogan

#endif // _SRC_OUTPUT_H_
