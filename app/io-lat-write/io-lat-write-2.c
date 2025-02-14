#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "file-api.h"
#include "io-lat-write.h"

static char* alloc_buf(size_t buf_size) {
  char* buf = malloc(buf_size * sizeof(char));
  if (buf == NULL) {
    return NULL;
  }

  for (size_t i = 0; i < buf_size; ++i) {
    buf[i] = FILL_CH;
  }

  return buf;
}

static void print_runtime_in_format(struct timeval const* stop, struct timeval const* start) {
  unsigned long usec = (stop->tv_sec - start->tv_sec) * 1000000 + stop->tv_usec - start->tv_usec;
  printf(
      "Writing time: %lu s %lu ms %lu us\n", usec / 1000000, (usec % 1000000) / 1000, (usec % 1000)
  );
}

int do_io_lat_write(long rep, char* filename) {
  int fd = lab2_open(filename);

  if (fd == -1) {
    fprintf(stderr, "Cannor open file %s", filename);
    return IEXIT_CODE_CANNOT_OPEN_FILE;
  }

  char* buf = alloc_buf(BLOCK_SIZE);

  for (long i = 0; i < rep; ++i) {
    struct timeval stop, start;

    gettimeofday(&start, NULL);
    size_t writed = lab2_write(fd, buf, BLOCK_SIZE);
    gettimeofday(&stop, NULL);

    if (writed != BLOCK_SIZE) {
      fprintf(stderr, "Error during writing to the file");
      lab2_close(fd);
      return IEXIT_CODE_WRITING_ERROR;
    }

    print_runtime_in_format(&stop, &start);
  }

  lab2_close(fd);
  free(buf);

  return 0;
}
