#pragma once

struct Node;

struct List {
  Node *head;
  Node *tail;

  bool Insert(int val);
  void Print();
};