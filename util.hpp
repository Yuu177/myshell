#ifndef UTIL
#define UTIL

#include <unistd.h>

#include <string>

inline std::string GetPwd() {
  char current_path[1024];
  if (getcwd(current_path, sizeof(current_path)) == nullptr) {
    perror("get curr path failed");
    exit(-1);
  }
  return std::string(current_path);
}

#endif  // UTIL
