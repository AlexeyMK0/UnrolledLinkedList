#pragma once

#include <iomanip>
#include <string>
#include <iostream>

template<typename NODE>
void PrintNode(NODE& node) {
    #ifdef ADD_DEBUG_PRINT
    std::cout << "size " << node.size() << std::endl;
    std::cout << "address " << &node << std::endl;
    std::cout << "next: " << node.next() << std::endl;
    std::cout << "prev: " << node.prev() << std::endl;
    // for (int i = 0; i < node.size(); ++i) {
    //     std::cout << node[i] << " ";
    // }
    // std::cout << std::endl;
    #endif
}

template<typename T>
void Print(const T& val) {
    #ifdef ADD_DEBUG_PRINT
    std::cout << val;
    #endif
}

template<typename T>
void Print(const T& val, const std::string& sign) {
    Print(sign);
    Print(": ");
    Print(val);
}

template<typename T>
void Println(const T& val) {
    Print(val);
    #ifdef ADD_DEBUG_PRINT
    std::cout << std::endl;
    #endif
}

template<typename T>
void Println(const T & val, const std::string& sign) {
    Print(val, sign);
    #ifdef ADD_DEBUG_PRINT
    std::cout << std::endl;
    #endif
}
