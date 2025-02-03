#pragma once

#include <sys/types.h>
#include <stdbool.h>

#include "cache_list.h"

#define CACHE_NO_ERROR 0
#define CACHE_ERROR_NO_BLOCK -1
#define CACHE_ERROR_REALLOC_DATA -2
#define CACHE_ERROR_TOO_SMALL_BUF -3

ssize_t cache_get_size(struct cache_list* cache, int fd, off_t offset);
int cache_get(struct cache_list* cache, int fd, off_t offset, void* buf, size_t count);
struct cache_node* cache_put(struct cache_list* cache, int fd, off_t offset, void* data_buf, size_t count);
