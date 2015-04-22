#ifndef __PATH_INLCUDES_H
#define __PATH_INLCUDES_H
#ifdef _WIN32
    #include <direct.h>
    #define getcwd _getcwd // stupid MSFT "deprecation" warning
#else
    #include <unistd.h>
#endif

#include <string>
#define MAX_PATH_LENGTH 1000

std::string getProjectDirectory();
std::string getResourcesDirectory();
std::string getResourcePath(std::string resource);

#endif  // __PATH_INLCUDES_H
