#include <stdlib.h>
#include <string.h>

#include "cache.h"

ssize_t cache_get_size(struct cache_list* cache, int fd, off_t offset) {
    struct cache_node* curr = cache->first;
    for (size_t _i = 0; _i < cache->length; ++_i) {
        if (cache_node_is(curr, fd, offset)) {
            return curr->size;
        }
    }

    return -1;
}

int cache_get(struct cache_list* cache, int fd, off_t offset, void* buf, size_t count) {
    struct cache_node* curr = cache->first;
    for (size_t _i = 0; _i < cache->length; ++_i) {
        if (cache_node_is(curr, fd, offset)) {
            if (count < curr->size) {
                return CACHE_ERROR_TOO_SMALL_BUF;
            }
            curr->chance = true;
            memcpy(buf, curr->data, curr->size);
            return CACHE_NO_ERROR;
        }
    }

    return CACHE_ERROR_NO_BLOCK;
}

int cache_update(struct cache_list* cache, int fd, off_t offset, void* data_buf, size_t count) {
    struct cache_node* curr = cache->first;
    for (size_t _i = 0; _i < cache->length; ++_i) {
        if (cache_node_is(curr, fd, offset)) {
            curr->chance = true;
            size_t data_buf_size = min(count, CACHE_NODE_DATA_CAP);

            if (curr->size != data_buf_size) {
                void* new_data = realloc(curr->data, data_buf_size);
                if (new_data == NULL) {
                    return CACHE_ERROR_REALLOC_DATA;
                }

                curr->size = data_buf_size;
                curr->data = new_data;
            }

            memcpy(curr->data, data_buf, data_buf_size);

            return CACHE_NO_ERROR;
        }
    }

    return CACHE_ERROR_NO_BLOCK;
}

struct cache_node* cache_put(
    struct cache_list* cache, int fd, off_t offset, void* data_buf, size_t count
) {
    if (cache_update(cache, fd, offset, data_buf, count) != NULL) {
        return NULL;
    }

    struct cache_node* removed;
    if (cache->length == cache->capacity) {
        removed = cache_list_remove_oldest(cache);
    }

    if (cache_list_add_back(cache, fd, offset, data_buf, count) == NULL) {
        cache_node_free(removed);
        return &error_node;
    }

    cache->length++;

    return removed;
}
