#include <iostream>
using namespace std;

class Base {
public:
    virtual void display(int key, int value = 110) {
        cout << "Base::display called with key = " << key << ", value = " << value << endl;
    }
};

class Derived : public Base {
public:
    // 注意：这里我们重写了方法但没有重新指定默认参数
    void display(int key,int value) override {
        cout << "Derived::display called with key = " << key << ", value = " << value << endl;
    }
};

int main() {
    Base* basePtr = new Base();
    Base* derivedPtr = new Derived();
    cout << "Calling through Base pointer:" << endl;
    basePtr->display(1);      // 使用Base的默认值10
    
    cout << "\nCalling through Base pointer pointing to Derived:" << endl;
    derivedPtr->display(1);   // 使用Base的默认值10，不是Derived的20
  
    delete basePtr;
    delete derivedPtr;
    
    return 0;
}