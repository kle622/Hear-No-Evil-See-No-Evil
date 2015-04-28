#ifndef __PATH_INLCUDES_H
#define __PATH_INLCUDES_H
#ifdef _WIN32
    #include <direct.h>
    #define getcwd _getcwd // stupid MSFT "deprecation" warning
    #define PATH_TOKEN "\\"
#else
    #include <unistd.h>
    #define PATH_TOKEN "/"
#endif

#include <string>

template<typename T>
T sysPath(T v) {
  return v;
}

template<typename T, typename... Args>
std::string sysPath(T str1, Args... strings)
{
  return str1 + std::string(PATH_TOKEN) + sysPath(strings...);
}

std::string resPath(std::string resource)
{
  return sysPath("..", "resources", resource);
}

#endif  // __PATH_INLCUDES_H
