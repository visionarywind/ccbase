//
// Created by jiangshanfeng on 2023/7/25.
//

#include <set>
#include <iostream>
#include "set.h"

void Set::test() {
    std::set<NodePtr> s;
    s.emplace(std::make_shared<Node>(1));
    // set can insert nullptr, treat nullptr as a normal element
    s.emplace(nullptr);
    for (const auto &e : s) {
        if (e != nullptr) {
            std::cout << e->val_ << std::endl;
        }
    }
    // set can find nullptr key
    auto ret = s.find(nullptr);
    std::cout << (ret == s.end()) << std::endl;
};