#include <errno.h>
#include <stdlib.h>

long parse_long_from_string(char* str) {
  long res = strtol(str, NULL, 10);
  return (errno == ERANGE || errno == EINVAL) ? -1 : res;
}
