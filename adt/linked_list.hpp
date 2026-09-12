#ifndef COMMON_ADT_LINKED_LIST_HPP
#define COMMON_ADT_LINKED_LIST_HPP

template <typename T>
struct IterableLinkedList {
  struct Iterator {
    T *operator*() const { return current_node; }

    Iterator &operator++() {
      current_node = current_node->next;
      return *this;
    }

    friend bool operator!=(const Iterator &iterator1, const Iterator &iterator2) {
      return iterator1.current_node != iterator2.current_node;
    }

    T *current_node;
  };

  Iterator begin() { return {head}; }

  Iterator end() { return {nullptr}; }

  T *head;
};

template <typename T>
IterableLinkedList<T> iterable_ll(T *head) {
  return {head};
}

template <typename T>
T *reverse_ll(T *head) {
  T *prev = nullptr;
  T *curr = head;
  while (curr) {
    T *next    = curr->next;
    curr->next = prev;
    prev       = curr;
    curr       = next;
  }
  return prev;
}

#endif
