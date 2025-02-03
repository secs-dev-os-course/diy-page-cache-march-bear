#pragma once

#include <sys/types.h>
#include <stdbool.h>

typedef struct {
    int fd;
    off_t offset;
} page_key;

struct cache_node {
    struct cache_node* next;
    page_key key;
    bool chance;
    bool sync;
    void* data;
};

struct cache_list {
    size_t capacity;
    size_t length;
    struct cache_node* first;
    struct cache_node* last;
};

struct cache_node* cache_node_create(int fd, off_t offset, void* data, size_t count);
bool cache_node_is(struct cache_node* node, int fd, off_t offset);
void cache_node_free(struct cache_node* node);

struct cache_node* cache_list_add_back(struct cache_list* list, int fd, off_t offset, void* data_buf, size_t count);
struct cache_node* cache_list_remove_oldest(struct cache_list* list);
void cache_list_destroy(struct cache_list* list);
