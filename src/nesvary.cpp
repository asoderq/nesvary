#include <iostream>

#include "nesvary.hpp"

#define QUOTE(str) #str
#define EXPAND_AND_QUOTE(str) QUOTE(str)

const char* nesvary_version()
{
    return EXPAND_AND_QUOTE(PROJECT_VERSION);
}

int main(int argc, char *argv[]) {
    std::cout << nesvary_version() << std::endl;
}
