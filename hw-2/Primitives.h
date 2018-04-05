#pragma once
#include <Exception.h>
#include <iostream>

class Int32 : public Stackable {
public:
    Int32(int val):value(val) {
        std::cout << "Creating Int32(" << value << ")" << std::endl;
    }
    ~Int32() {
        std::cout << "Destoying Int32(" << value << ")" << std::endl;
    }
    int operator()() { return value; }

private:
    int value;
};
