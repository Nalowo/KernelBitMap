#ifndef BITMAP_H
#define BITMAP_H

#include <linux/types.h>

void ba_bitmap_set(unsigned char *bm, size_t idx);
void ba_bitmap_clear(unsigned char *bm, size_t idx);
int ba_bitmap_test(unsigned char *bm, size_t idx);
size_t ba_bitmap_count_set(unsigned char *bm, size_t total);
ssize_t ba_bitmap_find_free_region(unsigned char *bm, size_t total, size_t num);
size_t ba_bitmap_longest_free_run(unsigned char *bm, size_t total);

#endif // BITMAP_H