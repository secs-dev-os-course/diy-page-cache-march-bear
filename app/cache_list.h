#pragma once

#include <sys/types.h>
#include <stdbool.h>

#define CACHE_NODE_DATA_CAP 4 * 1024

typedef struct {
    int fd;
    off_t offset;
} page_key;

struct cache_node {
    struct cache_node* next;
    page_key key;
    bool chance;
    bool sync;
    size_t size;
    void* data;
};

const struct cache_node error_node;

struct cache_list {
    size_t capacity;
    size_t length;
    struct cache_node* first;
    struct cache_node* last;
};

size_t min(size_t a, size_t b);

struct cache_node* cache_node_create(int fd, off_t offset, void* data, size_t count);
bool cache_node_is(struct cache_node* node, int fd, off_t offset);
void cache_node_free(struct cache_node* node);

struct cache_node* cache_list_add_back(struct cache_list* list, int fd, off_t offset, void* data_buf, size_t count);
struct cache_node* cache_list_remove_oldest(struct cache_list* list);
void cache_list_destroy(struct cache_list* list);
