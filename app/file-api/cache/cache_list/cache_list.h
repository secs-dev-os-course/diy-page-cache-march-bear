#pragma once

#include <sys/types.h>
#include <stdbool.h>

#define CACHE_NODE_DATA_CAP (4 * 1024)

/*
    Структура cache_list, фактическое представление кэша
*/

typedef struct {
    int fd;
    off_t offset;
} page_key;

struct cache_node {
    struct cache_node* next;
    page_key key;
    bool chance;
    bool sync;
    off_t ioff;
    size_t size;
    void* data;
};

extern struct cache_node error_node;

struct cache_list {
    size_t capacity;
    size_t length;
    struct cache_node* first;
    struct cache_node* last;
};

size_t min(size_t a, size_t b);
size_t max(size_t a, size_t b);

struct cache_node* cache_node_create(page_key key, bool sync, off_t ioff, void* data_buf, size_t count);
bool cache_node_is(struct cache_node* node, page_key key);
void cache_node_free(struct cache_node* node);

struct cache_node* cache_list_add_back(
    struct cache_list* list, page_key key, bool sync, off_t ioff, void* data_buf, size_t count
);
struct cache_node* cache_list_remove_oldest(struct cache_list* list);
void cache_list_destroy(struct cache_list* list);
