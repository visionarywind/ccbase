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

void RefTest();

#endif //CCBASE_REF_H
