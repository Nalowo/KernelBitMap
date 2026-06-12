#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include "allocator.h"

#define KERNEL_ALLOC_MODULE "kernel_alloc_module"

static int __init kernel_alloc_module_init(void) {
  int res = allocator_init();
  if (res != ALLOC_OK)
  {
    pr_err("%s: init ERR: %d\n", KERNEL_ALLOC_MODULE, res);
    return res;
  }

  pr_info("%s: init OK: \n", KERNEL_ALLOC_MODULE);
  return res;
}

static void __exit kernel_alloc_module_exit(void) {
  allocator_cleanup();
  pr_info("%s: exit OK: \n", KERNEL_ALLOC_MODULE);
}

module_init(kernel_alloc_module_init);
module_exit(kernel_alloc_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vlad");
MODULE_DESCRIPTION("bitmap алокатор памяти");
MODULE_VERSION("1.0");