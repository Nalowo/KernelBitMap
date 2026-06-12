obj-m += kernel_alloc_module.o

kernel_alloc_module-y := src/main.o \
                         src/params.o \
                         src/bitmap.o \
                         src/allocator.o

ccflags-y := -I$(src)/src
