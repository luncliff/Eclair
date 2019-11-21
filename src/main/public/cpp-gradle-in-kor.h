//
// interface header file
//
#ifndef CPP_GRADKE_IN_KOR_H
#define CPP_GRADKE_IN_KOR_H

#ifdef _WIN32
#define _INTERFACE_ __declspec(dllexport)
#else
#define _INTERFACE_
#endif

#include <string>

namespace cpp_gradle_in_kor
{
class Greeter
{
public:
    _INTERFACE_ std::string greeting();
};
} // namespace cpp_gradle_in_kor

#endif
