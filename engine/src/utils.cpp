#include "../include/utils.hpp"
#include <cstring>

bool endswith(string s, const char *end) {
  if (strlen(end) > s.size()) {
    return false;
  }
  string thisend = s.substr(s.size() - strlen(end), s.size() + 1);
  string str = end;
  return strcmp(thisend.c_str(), str.c_str()) == 0;
}
