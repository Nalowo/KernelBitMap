#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sysfs.h>

#include "allocator.h"

static int param_set_alloc(const char *val, const struct kernel_param *kp) {
  unsigned long bytes;
  void *ptr;

  if (kstrtoul(val, 0, &bytes))
    return -EINVAL;
  if (bytes == 0)
    return -EINVAL;

  ptr = allocator_alloc(bytes);
  if (!ptr)
    return -ENOMEM;

  pr_info("%s: allocated %lu bytes at %px\n", KERNEL_ALLOC_MODULE, bytes, ptr);
  return 0;
}

static int param_set_free(const char *val, const struct kernel_param *kp) {
  unsigned long addr;
  int ret;

  if (kstrtoul(val, 0, &addr)) /* база 0 — понимает префикс 0x */
    return -EINVAL;

  ret = allocator_free((void *)addr);
  if (ret == ALLOC_INVALID)
    return -EINVAL;
  if (ret == ALLOC_NOT_FOUND)
    return -ENOENT;

  pr_info("%s: freed memory at %px\n", KERNEL_ALLOC_MODULE, (void *)addr);
  return 0;
}

static int param_get_stats(char *buf, const struct kernel_param *kp) {
  struct stats_info s = allocator_get_stats();

  return sysfs_emit(buf,
                    "Total: %zu KB | Free: %zu KB | Allocated: %zu KB | "
                    "Fragmentation: %zu%%\n",
                    s.total_memory / 1024, s.free_memory / 1024,
                    s.allocated_memory / 1024, s.fragmentation_percent);
}

#define BITMAP_VIEW_CELLS 64
static int param_get_bitmap_info(char *buf, const struct kernel_param *kp) {
  char view[BITMAP_VIEW_CELLS + 1];
  size_t per_cell = g_alloc.total_blocks / BITMAP_VIEW_CELLS;
  size_t cell, i;

  if (per_cell == 0)
    per_cell = 1;

  for (cell = 0; cell < BITMAP_VIEW_CELLS; cell++) {
    char c = '.';
    for (i = 0; i < per_cell; i++) {
      size_t idx = cell * per_cell + i;
      if (idx < g_alloc.total_blocks && ba_bitmap_test(g_alloc.bitmap, idx)) {
        c = 'X';
        break;
      }
    }
    view[cell] = c;
  }
  view[BITMAP_VIEW_CELLS] = '\0';

  return sysfs_emit(buf, "[%s]\n", view);
}

static const struct kernel_param_ops alloc_ops = {.set = param_set_alloc};
static const struct kernel_param_ops free_ops = {.set = param_set_free};
static const struct kernel_param_ops stats_ops = {.get = param_get_stats};
static const struct kernel_param_ops bitmap_ops = {.get =
                                                       param_get_bitmap_info};

module_param_cb(alloc, &alloc_ops, NULL, 0200);
MODULE_PARM_DESC(alloc, "Allocate N bytes");
module_param_cb(free, &free_ops, NULL, 0200);
MODULE_PARM_DESC(free, "Free memory by address");
module_param_cb(stats, &stats_ops, NULL, 0400);
MODULE_PARM_DESC(stats, "Allocator statistics");
module_param_cb(bitmap_info, &bitmap_ops, NULL, 0400);
MODULE_PARM_DESC(bitmap_info, "Bitmap occupancy visualization");