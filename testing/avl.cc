#include <algorithm>
#include <chrono>
#include <iostream>
#include <set>
#include <thread>

template <typename T>
class AVLTree {
private:
    struct Node {
        T value;
        Node* left;
        Node* right;
        int height;
        
        Node(T val) : value(val), left(nullptr), right(nullptr), height(1) {}
    };

    Node* root;

    int height(Node* node) {
        return node ? node->height : 0;
    }

    void updateHeight(Node* node) {
        node->height = 1 + std::max(height(node->left), height(node->right));
    }

    int balanceFactor(Node* node) {
        return height(node->left) - height(node->right);
    }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        updateHeight(y);
        updateHeight(x);
        
        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);
        
        return y;
    }

    Node* balance(Node* node) {
        if (!node) return nullptr;
        
        updateHeight(node);
        int bf = balanceFactor(node);
        
        if (bf > 1 && balanceFactor(node->left) >= 0) {
            return rotateRight(node);
        }
        
        if (bf < -1 && balanceFactor(node->right) <= 0) {
            return rotateLeft(node);
        }
        
        if (bf > 1 && balanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        
        if (bf < -1 && balanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        
        return node;
    }

    Node* insert(Node* node, T value) {
        if (!node) return new Node(value);
        
        if (value < node->value) {
            node->left = insert(node->left, value);
        } else if (value > node->value) {
            node->right = insert(node->right, value);
        } else {
            return node;
        }
        
        return balance(node);
    }

    Node* minValueNode(Node* node) {
        Node* current = node;
        while (current && current->left) {
            current = current->left;
        }
        return current;
    }

    Node* remove(Node* node, T value) {
        if (!node) return nullptr;
        
        if (value < node->value) {
            node->left = remove(node->left, value);
        } else if (value > node->value) {
            node->right = remove(node->right, value);
        } else {
            if (!node->left || !node->right) {
                Node* temp = node->left ? node->left : node->right;
                if (!temp) {
                    temp = node;
                    node = nullptr;
                } else {
                    *node = *temp;
                }
                delete temp;
            } else {
                Node* temp = minValueNode(node->right);
                node->value = temp->value;
                node->right = remove(node->right, temp->value);
            }
        }
        
        if (!node) return nullptr;
        
        return balance(node);
    }

    bool contains(Node* node, T value) {
        if (!node) return false;
        if (value == node->value) return true;
        return value < node->value ? contains(node->left, value) : contains(node->right, value);
    }

    void inorder(Node* node) {
        if (!node) return;
        inorder(node->left);
        std::cout << node->value << " ";
        inorder(node->right);
    }

public:
    AVLTree() : root(nullptr) {}
    
    void insert(T value) {
        root = insert(root, value);
    }
    
    void remove(T value) {
        root = remove(root, value);
    }
    
    bool contains(T value) {
        return contains(root, value);
    }
    
    void inorder() {
        inorder(root);
        std::cout << std::endl;
    }
};

uint64_t GetClockTimeNs() {
  auto ts = std::chrono::system_clock::now();
  int64_t system_t = std::chrono::duration_cast<std::chrono::nanoseconds>(ts.time_since_epoch()).count();
  return static_cast<uint64_t>(system_t);
}

int main() {
    constexpr size_t count = 10000;
    std::thread avl_thread([&]() {
      int cycle_count = 1000;
      uint64_t total_cost = 0;
      for (int i = 0; i != cycle_count; i++) {
        AVLTree<int> tree;
        uint64_t start = GetClockTimeNs();
        for (size_t j = 0; j != count; j++) {
            tree.insert(i);
        }
        uint64_t cost = GetClockTimeNs() - start;
        total_cost += cost;
        // for (size_t j = 0; j != count; j++) {
        //     tree.remove(j);
        // }
      }
      std::cout << "avl cost : " << total_cost * 1.0 / 1000 << std::flush << std::endl;
    });
    
    std::thread stl_thread([&]() {
        int cycle_count = 1000;
        uint64_t total_cost = 0;
        for (int i = 0; i != cycle_count; i++) {
            std::set<int> std_set;
            uint64_t start = GetClockTimeNs();
            for (size_t j = 0; j != count; j++) {
                std_set.insert(i);
            }
            uint64_t cost = GetClockTimeNs() - start;
            // std::cout << "std cost : " << cost * 1.0 / 1000 << std::flush << std::endl;
            total_cost += cost;
            // for (size_t j = 0; j != count; j++) {
            //     std_set.erase(j);
            // }
        }
        std::cout << "std cost : " << total_cost * 1.0 / 1000 << std::flush << std::endl;
    });
    avl_thread.join();
    stl_thread.join();

    return 0;
}