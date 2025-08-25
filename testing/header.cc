#include "header.h"
#include <iostream>

struct Node {
  int data;
  Node *next;
};

bool List::Insert(int val) {
  Node *node = new Node;
  node->data = val;
  node->next = nullptr;
  if (head == nullptr) {
    head = node;
    tail = node;
    return true;
  }
  tail->next = node;
  tail = node;
  return true;
}

void List::Print() {
  Node *node = head;
  while (node != nullptr) {
    std::cout << node->data << std::endl;
    node = node->next;
  }
}
