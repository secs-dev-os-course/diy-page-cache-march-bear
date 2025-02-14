#include <stdlib.h>
#include <string.h>

#include "cache_list.h"

struct cache_node error_node = { .key = { .fd = -1 } };

size_t min(size_t a, size_t b) {
    return (a <= b) ? a : b;
}

size_t max(size_t a, size_t b) {
    return (a >= b) ? a : b;
}

struct cache_node* cache_node_create(page_key key, bool sync, off_t ioff, void* data, size_t count) {
    struct cache_node* node = malloc(sizeof(struct cache_node));
    if (node == NULL) {
        return NULL;
    }

    node->next = NULL;
    node->key = key;
    node->chance = false;
    node->sync = sync;
    node->ioff = ioff;
    node->size = count;

    if (data != NULL) {
        if (count + ioff > CACHE_NODE_DATA_CAP) {
            free(node);
            return NULL;
        }
        node->data = malloc(count + ioff);
        if (node->data == NULL) {
            free(node);
            return NULL;
        }

        memcpy((node->data + ioff), data, count);
    } else {
        node->data = NULL;
    }

    return node;
}

void cache_node_free(struct cache_node* node) {
    if (node == NULL) {
        return ;
    }

    if (node->data != NULL) {
        free(node->data);
    }

    free(node);
}

bool cache_node_is(struct cache_node* node, page_key key) {
    if (node == NULL) {
        return false;
    }

    return node->key.fd == key.fd && node->key.offset == key.offset;
}

struct cache_node* cache_list_add_back(
    struct cache_list* list, page_key key, bool sync, off_t ioff, void* data, size_t count
) {
    if (list == NULL) {
        return NULL;
    }

    struct cache_node* node = cache_node_create(key, sync, ioff, data, count);
    if (node == NULL) {
        return NULL;
    }

    if (list->first == NULL) {
        list->first = node;
        list->last = node;
    } else if (list->first == list->last) {
        list->first->next = node;
        list->last = node;
    } else {
        list->last->next = node;
        list->last = list->last->next;
    }

    return node;
}

struct cache_node* cache_list_remove_oldest(struct cache_list* list) {
    if (list == NULL || list->length == 0) {
        return NULL;
    }

    struct cache_node* prev = NULL;
    struct cache_node* curr = list->first;
    while (true) {
        if (curr->chance) {
            curr->chance = false;
            prev = curr;
            curr = (curr->next == NULL) ? list->first : curr->next;
        } else {
            if (curr == list->first) {
                list->first = list->first->next;
                if (curr == list->last) {
                    list->last = NULL;
                }
            } else if (curr == list->last) {
                list->last = prev;
                list->last->next = NULL;
            } else {
                prev->next = curr->next;
            }
            break;
        }
    }

    list->length--;
    return curr;
}

void cache_list_destroy(struct cache_list* list) {
    if (list == NULL) {
        return;
    }

    struct cache_node* node = list->first;
    while(node != NULL) {
        struct cache_node* next = node->next;
        cache_node_free(node);
        node = next;
    }

    free(list);
}
