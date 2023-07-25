//
// Created by jiangshanfeng on 2023/7/25.
//

#ifndef CCBASE_SET_H
#define CCBASE_SET_H

#include <memory>

struct Node {
    Node(int val) : val_(val) {}

    Node *next_{nullptr};
    int val_;
};

using NodePtr = std::shared_ptr<Node>;

class Set {
public:
    void test();
};


#endif //CCBASE_SET_H
