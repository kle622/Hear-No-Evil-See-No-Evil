#include "Path.h"

/*std::string getProjectDirectory()
{
  char buffer[MAX_PATH_LENGTH];
  char *result = getcwd(buffer, MAX_PATH_LENGTH);

  if (NULL != result) {
    std::string res = new std::string(buffer, MAX_PATH_LENGTH);
#ifdef _WIN32
    res = res + "\\";
#else // unix
    res = res + "/";
#endif
    res = res + "..";
  }
  else {
    return NULL;
  }
}*/

std::string getResourcesDirectory()
{
#ifdef _WIN32
  return "..\\resources";
#else // unix
  return "../resources";
#endif
}

std::string getResourcePath(std::string resource)
{
  std::string res = getResourcesDirectory();
#ifdef _WIN32
  res = res + "\\";
#else // unix
  res = res + "/";
#endif
  res = res + resource;
  return res;
}
