#include <stdlib.h>
#include <string.h>

#include "cache.h"

ssize_t cache_get_size(struct cache_list* cache, page_key key) {
    struct cache_node* curr = cache->first;
    for (size_t _i = 0; _i < cache->length; ++_i) {
        if (cache_node_is(curr, key)) {
            return curr->size;
        }
    }

    return -1;
}

// читает данные из ноды (если существует) в буфер, если count больше размера блока - ошибка
int cache_get(struct cache_list* cache, page_key key, off_t ioff, void* buf, size_t count) {
    struct cache_node* curr = cache->first;
    for (size_t _i = 0; _i < cache->length; ++_i) {
        if (cache_node_is(curr, key)) {
            if (count > curr->size) {
                return CACHE_ERROR_TOO_BIG_COUNT;
            }

            if (ioff < curr->ioff) {
                return CACHE_NO_DATA_BEFORE_IOFF;
            }

            curr->chance = true;
            memcpy(buf, curr->data + ioff, count);
            return CACHE_NO_ERROR;
        }
    }

    return CACHE_ERROR_NO_BLOCK;
}

int cache_update(
    struct cache_list* cache, page_key key, bool to_wr, off_t ioff, void* data_buf, size_t count
) {
    if (ioff + count > CACHE_NODE_DATA_CAP) {
        return CACHE_ERROR_TOO_BIG_COUNT;
    }

    struct cache_node* curr = cache->first;
    for (size_t _i = 0; _i < cache->length; ++_i) {
        if (cache_node_is(curr, key)) {
            curr->chance = true;

            if (curr->size + curr->ioff < count + ioff) {
                void* new_data = realloc(curr->data, count + ioff);
                if (new_data == NULL) {
                    return CACHE_ERROR_REALLOC_DATA;
                }
                
                curr->data = new_data;
            }
            off_t min_ioff = min(ioff, curr->ioff);
            curr->size = max(count + ioff - min_ioff, curr->size + curr->ioff - min_ioff);
            curr->ioff = min_ioff;

            curr->sync = !to_wr;

            memcpy(curr->data + ioff, data_buf, count);

            return CACHE_NO_ERROR;
        }
    }

    return CACHE_ERROR_NO_BLOCK;
}

struct cache_node* cache_put(
    struct cache_list* cache, page_key key, bool to_wr, off_t ioff, void* data_buf, size_t count
) {
    if (cache_update(cache, key, to_wr, ioff, data_buf, count) == CACHE_NO_ERROR) {
        return NULL;
    }

    struct cache_node* removed = NULL;
    if (cache->length == cache->capacity) {
        removed = cache_list_remove_oldest(cache);
    }

    if (cache_list_add_back(cache, key, !to_wr, ioff, data_buf, count) == NULL) {
        cache_node_free(removed);
        return &error_node;
    }

    cache->length++;

    return removed;
}
