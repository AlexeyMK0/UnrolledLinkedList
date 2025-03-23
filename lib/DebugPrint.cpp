#include "DebugPrint.h"

template<>
void Print(const bool& val) {
    #ifdef ADD_DEBUG_PRINT
    std::cout << std::boolalpha << val;
    #endif
}
