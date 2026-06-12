#include "allocator.h"

struct memory_allocator g_memAlloc;

int allocator_init(void)
{
    return ALLOC_NOMEM;
}

void* allocator_alloc(size_t bytes)
{
    return NULL;
}

int allocator_free(void *ptr)
{
    return ALLOC_NOMEM;
}

struct stats_info allocator_get_stats(void)
{
    struct stats_info res =
    {
         .total_blocks = 0,
         .free_blocks = 0,
         .allocated_blocks = 0,
         .total_memory = 0,
         .free_memory = 0,
         .allocated_memory = 0,
         .fragmentation_percent = 0
    };
    return res;
}

void allocator_cleanup(void)
{

}