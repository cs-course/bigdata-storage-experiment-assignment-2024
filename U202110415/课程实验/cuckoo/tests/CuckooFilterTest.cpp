//
// Created by Lu Shuyu on 2024/4/23.
//

#include <cassert>
#include <iostream>

#include <CuckooMap/CuckooFilter.h>

struct Key {
    int k;
    Key() : k(0) {}
    Key(int i) : k(i) {}
    bool empty() { return k == 0; }
};

namespace std {

template <> struct equal_to<Key> {
    bool operator()(Key const &a, Key const &b) const { return a.k == b.k; }
};
} // namespace std

int main(int /*argc*/, char * /*argv*/[]) {
    CuckooFilter<Key> m(false, 100);
    auto insert = [&]() -> void {
        for (int i = 0; i < 300; ++i) {
            Key k(i);
            m.insert(k);
            std::cout << "Inserted key " << i << std::endl;
        }
    };
    auto show = [&]() {
        for (int i = 299; i >= 0; --i) {
            Key k(i);
            if (m.lookup(k)) {
                std::cout << "Found key " << i << std::endl;
            } else {
                std::cout << "Did not find key " << i << std::endl;
                assert(false);
            }
        }
    };
    auto remove = [&]() -> void {
        for (int i = 0; i < 100; ++i) {
            Key k(i);
            if (m.remove(k)) {
                std::cout << "Removed key " << i << std::endl;
            } else {
                std::cout << "Did not find key " << i << std::endl;
                assert(false);
            }
        }
    };
    auto notShow = [&]() {
        for (int i = 0; i < 100; ++i) {
            Key k(i);
            if (m.lookup(k)) {
                std::cout << "Found removed key " << i << std::endl;
                assert(false);
            } else {
                std::cout << "Did not find key " << i << std::endl;
            }
        }
    };
    std::cout << "map was made" << std::endl;
    insert();
    show();
    remove();
    notShow();
}