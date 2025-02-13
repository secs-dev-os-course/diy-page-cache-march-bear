#pragma once

#include <sys/types.h>
#include <stdbool.h>

#include "cache_list.h"

#define CACHE_NO_ERROR 0
#define CACHE_ERROR_NO_BLOCK -1
#define CACHE_ERROR_REALLOC_DATA -2
#define CACHE_ERROR_TOO_BIG_COUNT -3
#define CACHE_NO_DATA_BEFORE_IOFF -4

/*
    Работа с кэшем на уровне "положить/получить"
    Интерфейс к cache_list
*/

ssize_t cache_get_size(struct cache_list* cache, page_key key);
int cache_get(struct cache_list* cache, page_key key, off_t ioff, void* buf, size_t count);
struct cache_node* cache_put(
    struct cache_list* cache, page_key key, bool to_wr, off_t ioff, void* data_buf, size_t count
);
