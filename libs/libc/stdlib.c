#include <syscalls.h>
#include <stddef.h>
#include <stdatomic.h>
#include <stdlib.h>

typedef struct block {
    uint64_t size;
    struct block *next;
} block_t;

static block_t *free_list = nullptr;
static atomic_flag lock = ATOMIC_FLAG_INIT;

static inline void spinlock_acquire(void) {
    while (atomic_flag_test_and_set_explicit(&lock, memory_order_acquire));
}

static inline void spinlock_release(void) {
    atomic_flag_clear_explicit(&lock, memory_order_release);
}

// Request memory from the heap
void *malloc(uint64_t size) {
    if (size == 0) return nullptr;
    size = (size + 15) & ~15ULL;

    spinlock_acquire();

    block_t **curr = &free_list;
    while (*curr) {
        if ((*curr)->size >= size) {
            block_t *blk = *curr;
            *curr = blk->next;
            spinlock_release();
            return (void *)(blk + 1);
        }
        curr = &(*curr)->next;
    }

    block_t *blk = sbrk(sizeof(block_t) + size);
    if (blk == (void *)-1) {
        spinlock_release();
        return nullptr;
    }

    blk->size = size;
    blk->next = nullptr;

    spinlock_release();
    return (void *)(blk + 1);
}

// Free requested memory
void free(void *ptr) {
    if (ptr == nullptr) return;
    block_t *blk = (block_t *)ptr - 1;

    spinlock_acquire();
    blk->next = free_list;
    free_list = blk;
    spinlock_release();
}