#include "allocator.h"

#include <linux/math.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

struct memory_allocator g_alloc;
static struct allocation_info *g_alloc_table;

int allocator_init(void) {
  g_alloc.total_blocks = ALLOC_TOTAL_BLOCKS;
  g_alloc.block_size = ALLOC_BLOCK_SIZE;
  spin_lock_init(&g_alloc.lock);

  g_alloc.bitmap = kzalloc(ALLOC_BITMAP_BYTES, GFP_KERNEL);
  if (!g_alloc.bitmap)
    return ALLOC_NOMEM;

  /* 10 MiB — крупный буфер, берём vmalloc (виртуально непрерывный). */
  g_alloc.memory_pool = vmalloc(ALLOC_TOTAL_MEMORY);
  if (!g_alloc.memory_pool)
    goto err_pool;

  g_alloc_table =
      kcalloc(g_alloc.total_blocks, sizeof(*g_alloc_table), GFP_KERNEL);
  if (!g_alloc_table)
    goto err_table;

  return ALLOC_OK;

err_table:
  vfree(g_alloc.memory_pool);
  g_alloc.memory_pool = NULL;
err_pool:
  kfree(g_alloc.bitmap);
  g_alloc.bitmap = NULL;
  return ALLOC_NOMEM;
}

void *allocator_alloc(size_t bytes) {
  if (bytes == 0)
    return NULL;

  size_t num = DIV_ROUND_UP(bytes, g_alloc.block_size);
  spin_lock(&g_alloc.lock);
  ssize_t start =
      ba_bitmap_find_free_region(g_alloc.bitmap, g_alloc.total_blocks, num);
  if (start < 0) {
    spin_unlock(&g_alloc.lock);
    return NULL;
  } // нет места

  for (int i = 0; i < num; i++)
    ba_bitmap_set(g_alloc.bitmap, start + i);

  g_alloc_table[start].start_block = start;
  g_alloc_table[start].num_blocks = num;
  spin_unlock(&g_alloc.lock);
  return (char *)g_alloc.memory_pool + start * g_alloc.block_size;
}

int allocator_free(void *ptr) {

  char *base = g_alloc.memory_pool;
  if (!ptr || (char *)ptr < base || (char *)ptr >= base + ALLOC_TOTAL_MEMORY)
    return ALLOC_INVALID;

  size_t off = (char *)ptr - base;
  if (off % g_alloc.block_size)
    return ALLOC_INVALID;

  size_t idx = off / g_alloc.block_size;
  spin_lock(&g_alloc.lock);
  size_t num = g_alloc_table[idx].num_blocks;

  if (num == 0) {
    spin_unlock(&g_alloc.lock);
    return ALLOC_NOT_FOUND;
  } // двойное free

  for (int i = 0; i < num; i++)
    ba_bitmap_clear(g_alloc.bitmap, idx + i);

  g_alloc_table[idx].num_blocks = 0;
  spin_unlock(&g_alloc.lock);
  return ALLOC_OK;
}

struct stats_info allocator_get_stats(void) {
  struct stats_info s = {0};
  size_t longest;

  spin_lock(&g_alloc.lock);
  s.total_blocks = g_alloc.total_blocks;
  s.allocated_blocks =
      ba_bitmap_count_set(g_alloc.bitmap, g_alloc.total_blocks);
  s.free_blocks = s.total_blocks - s.allocated_blocks;
  longest = ba_bitmap_longest_free_run(g_alloc.bitmap, g_alloc.total_blocks);
  spin_unlock(&g_alloc.lock);

  s.total_memory = s.total_blocks * g_alloc.block_size;
  s.allocated_memory = s.allocated_blocks * g_alloc.block_size;
  s.free_memory = s.free_blocks * g_alloc.block_size;

  if (s.free_blocks > 0)
    s.fragmentation_percent = (s.free_blocks - longest) * 100 / s.free_blocks;
  else
    s.fragmentation_percent = 0;

  return s;
}

void allocator_cleanup(void) {
  kfree(g_alloc_table);
  g_alloc_table = NULL;
  vfree(g_alloc.memory_pool);
  g_alloc.memory_pool = NULL;
  kfree(g_alloc.bitmap);
  g_alloc.bitmap = NULL;
}