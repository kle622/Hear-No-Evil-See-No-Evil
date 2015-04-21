#ifndef __PATH_INLCUDES_H
#define __PATH_INLCUDES_H
#ifdef _WIN32
    #include <direct.h>
    #define getcwd _getcwd // stupid MSFT "deprecation" warning
#elif
    #include <unistd.h>
#endif
#endif  // __PATH_INLCUDES_H
