#include "bitmap.h"

void ba_bitmap_set(unsigned char *bm, size_t idx) {
  bm[idx / 8] |= (unsigned char)(1u << (idx % 8));
}

void ba_bitmap_clear(unsigned char *bm, size_t idx) {
  bm[idx / 8] &= (unsigned char)~(1u << (idx % 8));
}

int ba_bitmap_test(unsigned char *bm, size_t idx) {
  return (bm[idx / 8] >> (idx % 8)) & 1u;
}

size_t ba_bitmap_count_set(unsigned char *bm, size_t total) {
  size_t count = 0;
  size_t i;
  for (i = 0; i < total; i++)
    if (ba_bitmap_test(bm, i))
      count++;
  return count;
}

ssize_t ba_bitmap_find_free_region(unsigned char *bm, size_t total,
                                   size_t num) {
  size_t run = 0;
  size_t i;
  for (i = 0; i < total; i++) {
    if (!ba_bitmap_test(bm, i)) {
      if (++run == num)
        return (ssize_t)(i - num + 1);
    } else {
      run = 0;
    }
  }
  return -1;
}

size_t ba_bitmap_longest_free_run(unsigned char *bm, size_t total) {
  size_t run = 0;
  size_t best = 0;
  size_t i;
  for (i = 0; i < total; i++) {
    if (!ba_bitmap_test(bm, i)) {
      run++;
      if (run > best)
        best = run;
    } else {
      run = 0;
    }
  }
  return best;
}