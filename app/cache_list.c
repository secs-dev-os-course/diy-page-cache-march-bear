#include <stdlib.h>
#include <string.h>

#include "cache_list.h"

struct cache_node* cache_node_create(int fd, off_t offset, void* data, size_t count) {
    struct cache_node* node = malloc(sizeof(struct cache_node));
    if (node == NULL) {
        return NULL;
    }

    node->next = NULL;
    node->key = (page_key) { .fd = fd, .offset = offset };
    node->chance = false;
    node->sync = false;

    if (data != NULL) {
        node->data = malloc(count);
        if (node->data == NULL) {
            free(node);
            return NULL;
        }

        memcpy(node->data, data, count);
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

bool cache_node_is(struct cache_node* node, int fd, off_t offset) {
    if (node == NULL) {
        return false;
    }

    return node->key.fd == fd && node->key.offset == offset;
}

struct cache_node* cache_list_add_back(
    struct cache_list* list, int fd, off_t offset, void* data, size_t count
) {
    if (list == NULL) {
        return NULL;
    }

    struct cache_node* node = cache_node_create(fd, offset, data, count);
    if (node == NULL) {
        return NULL;
    }

    if (list->first == NULL) {
        list->first = node;
        list->last = node;
    } else if (list->first == list->last) {
        list->first->next = node;
        list->last = node;
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
            } else if (curr == list->last) {
                list->last = prev;
                list->last->next = NULL;
            } else {
                prev->next = curr->next;
            }
            break;
        }
    }

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
