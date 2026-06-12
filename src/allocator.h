#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <linux/bitmap.h>
#include <linux/bitops.h>
#include <linux/spinlock.h>

#include "bitmap.h"

#define ALLOC_OK        0       /* Операция успешна */
#define ALLOC_NOMEM     -1      /* Недостаточно памяти */
#define ALLOC_INVALID   -2      /* Неверный параметр */
#define ALLOC_NOT_FOUND -3      /* Блок не найден при освобождении */

struct memory_allocator {
    unsigned char *bitmap;          /* Bitmap для отслеживания блоков (1 бит = 1 блок) */
    void *memory_pool;              /* Пул памяти для выделения */
    size_t total_blocks;            /* Общее количество блоков */
    size_t block_size;              /* Размер одного блока в байтах */
    spinlock_t lock;                /* Спинлок для синхронизации доступа */
};

struct allocation_info {
    size_t start_block;             /* Индекс начального блока */
    size_t num_blocks;              /* Количество выделенных блоков */
};

struct stats_info {
    size_t total_blocks;            /* Общее количество блоков */
    size_t free_blocks;             /* Количество свободных блоков */
    size_t allocated_blocks;        /* Количество выделенных блоков */
    size_t total_memory;            /* Общий объем памяти в байтах */
    size_t free_memory;             /* Объем свободной памяти в байтах */
    size_t allocated_memory;        /* Объем выделенной памяти в байтах */
    size_t fragmentation_percent;   /* Процент фрагментации */
};

int allocator_init(void);
void* allocator_alloc(size_t bytes);
int allocator_free(void *ptr);
struct stats_info allocator_get_stats(void);
void allocator_cleanup(void);

#endif // ALLOCATOR_H