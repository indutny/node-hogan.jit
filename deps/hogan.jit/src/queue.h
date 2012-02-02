#ifndef _SRC_QUEUE_H_
#define _SRC_QUEUE_H_

#include <stdlib.h> // NULL

namespace hogan {

template <class T>
class Queue {
 public:
  class Entity {
   public:
    T value;
    Entity* next;

    Entity(T value_) : value(value_), next(NULL) {
    }

    ~Entity() {}
  };

  Queue() : head(NULL), current(NULL) {
  }
  ~Queue() {
    T value;
    while ((value = Shift()) != NULL) {
    }
  }

  void Push(T value) {
    Entity* next = new Entity(value);
    if (head == NULL) {
      head = next;
      current = head;
    } else {
      current->next = next;
      current = next;
    }
  }

  T Shift() {
    if (head == NULL) return NULL;

    Entity* old = head;
    T result = old->value;
    head = old->next;

    delete old;

    return result;
  }

 private:
  Entity* head;
  Entity* current;
};


} // namespace hogan

#endif // _SRC_QUEUE_H_
