#define _GNU_SOURCE

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str-tools/str-tools.h"
#include "io-lat-write.h"

int main(int argc, char* argv[]) {
  long rep;
  char filename[1024] = "file";

  switch (argc) {
    case 3:
      strcpy(filename, argv[2]);
    case 2:
      rep = parse_long_from_string(argv[1]);
      if (rep == -1) {
        fprintf(stderr, "Argument rep must be in range (1, %lu))\n", LONG_MAX);
        exit(IEXIT_CODE_WRONG_ARG_FORMAT);
      }
      break;
    default:
      fprintf(stderr, "Expented two argumetns: <num_of_repeations> (<filename>)\n");
      exit(IEXIT_CODE_WRONG_ARGC);
  }

  return do_io_lat_write(rep, filename);
}