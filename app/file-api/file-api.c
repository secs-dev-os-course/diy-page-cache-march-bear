#define _GNU_SOURCE

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "file-api.h"
#include "cache/cache.h"

struct cache_list cache = {
    .capacity = 256, 
    .length = 0, 
    .first = NULL, 
    .last = NULL
};

int lab2_open(const char *path) {
    return open(path, O_CREAT | O_DIRECT | O_SYNC | O_RDWR, S_IRWXU);
}

// синхронизация ноды, в случае успеха возвращает 0
int cache_node_fsync(struct cache_node* node) {
    if (node == NULL || node->sync) {
        return 0;
    }

    if (node->key.fd == -1 || node->data == NULL) {
        return -2;
    }

    ssize_t res = pwrite(
        node->key.fd, node->data + node->ioff, node->size, node->key.offset + node->ioff
    );

    if (res == node->size) {
        node->sync = true;
        return 0;
    }

    return res;
}

// синхронизация всех нод в листе по fd с последующим удалением
// возвращает 0 в случае успеха
int cache_list_fsync_and_clear(struct cache_list* list, int fd) {
    int sync_res = lab2_fsync(fd);
    if (sync_res != 0) {
        return sync_res;
    }

    struct cache_node* prev = NULL;
    struct cache_node* curr = list->first;
    
    while (curr != NULL) {
        struct cache_node* next = curr->next;
        if (curr->key.fd == fd) {
            cache_node_free(curr);
            curr = NULL;
            list->length--;
            if (prev == NULL) {
                list->first = next;
            } else {
                prev->next = next;
            }

            if (next == NULL) {
                list->last = prev;
            }
        } else {
            prev = curr;
        }
        
        curr = next;
    }
    
    return 0;
}

int lab2_close(int fd) {
    int fsynced = cache_list_fsync_and_clear(&cache, fd);
    if (fsynced != 0) {
        return fsynced;
    }

    return close(fd);
}

// аписать данные из буфера в кеш (работа с файлом в случае вытеснения)
int cache_put_buf(
    struct cache_list* cache, int fd, off_t offset, bool to_wr, void* data_buf, size_t count
) {
    if (data_buf == NULL || count == 0) {
        return -2;
    }

    off_t curr_ioff = offset % CACHE_NODE_DATA_CAP;
    offset = offset - curr_ioff;

    void* curr_buf = data_buf;
    while (count != 0) {
        size_t curr_count = min(count, CACHE_NODE_DATA_CAP - curr_ioff);
        struct cache_node* removed = cache_put(
            cache, (page_key) { fd, offset }, to_wr, curr_ioff, curr_buf, curr_count
        );

        int synched = cache_node_fsync(removed);

        if (synched != 0) {
            return synched;
        }

        curr_ioff = 0;
        data_buf += curr_count;
        count -= curr_count;
    }
    return 0;
}

// записать данные из кэша в буфер любого размера, возвращает кол-во прочитанных байт
// не производит чтение файла
size_t cache_get_buf(
    struct cache_list* cache, int fd, off_t offset, void* data_buf, size_t count
) {
    page_key key = (page_key) { fd, offset};

    if (data_buf == NULL || count == 0) {
        return -2;
    }
    void* curr_buf = data_buf;
    size_t readed = 0;
    off_t curr_ioff = offset % CACHE_NODE_DATA_CAP;
    offset = offset - curr_ioff;

    while (count != 0) {
        size_t curr_count = cache_get_size(cache, key);
        int res = cache_get(cache, key, curr_ioff, curr_buf, curr_count);

        if (res != CACHE_NO_ERROR) {
            return readed;
        }

        data_buf += curr_count;
        readed += curr_count;
        count -= curr_count;
        curr_ioff = 0;

        if (count != 0 && curr_ioff + curr_count != CACHE_NODE_DATA_CAP) {
            return readed;
        }
    }

    return readed;
}

// чтение из файла в буфер
// если есть что-то в кеше - читается из кеша
// если прочитано не все - читается из файла и заносится в кеш
ssize_t lab2_read(int fd, void* buf, size_t count) {
    size_t off = lseek(fd, 0, SEEK_CUR);

    size_t readed = cache_get_buf(&cache, fd, off, buf, count);
    if (readed != count) {
        off_t off = lseek(fd, readed, SEEK_CUR);
        if (off == (off_t) -1) {
            return -1;
        }

        ssize_t res = read(fd, buf + readed, count - readed);
        if (res == -1) {
            return -1;
        }

        if (cache_put_buf(&cache, fd, off + readed, false, buf + readed, res) != 0) {
            return -1;
        }

        return readed + res;
    }

    return count;
}


ssize_t lab2_write(int fd, const void* buf, size_t count) {
    size_t off = lseek(fd, 0, SEEK_CUR);

    int puted = cache_put_buf(&cache, fd, off, true, buf, count);

    if (puted != 0) {
        return -1;
    }

    if(lseek(fd, count, SEEK_CUR) != (off_t) -1) {
        return count;
    }

    return -1;
}

off_t lab2_lseek(int fd, off_t offset, int whence) {
    return lseek(fd, offset, whence);
}


// синхронизация всех нод в листе по fd
int lab2_fsync(int fd) {
    struct cache_node* curr = cache.first;
    while (curr != NULL) {
        if (curr->key.fd == fd) {
            int res = cache_node_fsync(curr);
            if (!(curr->sync)) {
                return -1;
            }
        }

        curr = curr->next;
    }

    return 0;
}