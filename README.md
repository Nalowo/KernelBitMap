# KernelBitMap

> Модуль ядра Linux: аллокатор памяти на основе bitmap — управление пулом блоков с поиском непрерывных регионов и потокобезопасностью.

Загружаемый модуль ядра, реализующий собственный блочный аллокатор поверх предвыделенного пула памяти. Свободные/занятые блоки отслеживаются битовой картой, выделение ищет непрерывный регион нужного размера, доступ к структуре защищён спинлоком. Управление и статистика — через интерфейс параметров модуля в sysfs.

## Что демонстрирует

- **Жизненный цикл модуля ядра:** `module_init` / `module_exit`, `__init` / `__exit`, коды возврата ядра (`-ENOMEM`).
- **Собственный аллокатор:** пул `10 MiB`, блок `4 KiB` (2560 блоков), битовая карта на 320 байт (1 бит = 1 блок).
- **Стратегия размещения:** поиск непрерывного свободного региона из `N` блоков (first-fit) — `ba_bitmap_find_free_region`.
- **Битовые операции:** set / clear / test бита, подсчёт занятых блоков, длина наибольшего свободного участка.
- **Синхронизация:** `spinlock_t` для защиты состояния аллокатора от гонок.
- **Статистика:** свободная/занятая память, процент фрагментации.
- **Интерфейс sysfs:** `module_param_cb` с кастомными `kernel_param_ops` (alloc / free / stats / bitmap_info).

## Архитектура

```
allocator.c   — allocator_init / allocator_alloc(bytes) / allocator_free(ptr); пул + spinlock + статистика
bitmap.c      — ba_bitmap_set/clear/test, find_free_region (непрерывный run), longest_free_run
params.c      — module_param_cb: alloc, free, stats, bitmap_info
main.c        — module_init/exit, инициализация и очистка аллокатора
```

## Сборка

```bash
make            # сборка -> kernel_alloc.ko
make load       # insmod
make unload     # rmmod
make clean
make format     # clang-format
```

## Проверка

```bash
sudo insmod kernel_alloc.ko

# выделение и освобождение через sysfs-параметры
echo <bytes> > /sys/module/kernel_alloc/parameters/alloc
echo <ptr>   > /sys/module/kernel_alloc/parameters/free

# статистика и состояние битовой карты
cat /sys/module/kernel_alloc/parameters/stats
cat /sys/module/kernel_alloc/parameters/bitmap_info

sudo rmmod kernel_alloc
dmesg | tail
```
