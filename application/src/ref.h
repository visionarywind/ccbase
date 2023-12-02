//
// Created by shanfeng on 12/10/2023.
//

#ifndef CCBASE_REF_H
#define CCBASE_REF_H

#include <memory>
#include <vector>

struct Ref;

class AddRef {
public:
    void Add();
    std::vector<std::shared_ptr<Ref>> list_;
};

struct Refer {
    std::vector<int> &list() { return list_; }
    std::vector<int> list_;
};

void RefTest();

void ReferTest();

#endif //CCBASE_REF_H
