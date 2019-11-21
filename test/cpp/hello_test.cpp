#include <cassert>
#include <iostream>

#include "cpp-gradle-in-kor.h"

#include "test_suite.h"

cpp_gradle_in_kor::Greeter greeter{};

int main()
{
    std::cout << greeter.greeting() << std::endl;
    return 0;
}
