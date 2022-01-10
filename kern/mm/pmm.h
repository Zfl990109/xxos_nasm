//
// Created by zfl on 2022/1/10.
// 实现物理内存管理
//

#ifndef XXOS_PMM_H
#define XXOS_PMM_H
struct pmm_manager {
    const char *name;                                 //
    void (*init)(void);                               // initialize internal description&management data structure
    // (free block list, number of free block) of XXX_pmm_manager
    void (*init_memmap)(struct Page *base, size_t n); // setup description&management data structcure according to
    // the initial free physical memory space
    struct Page *(*alloc_pages)(size_t n);            // allocate >=n pages, depend on the allocation algorithm
    void (*free_pages)(struct Page *base, size_t n);  // free >=n pages with "base" addr of Page descriptor structures(memlayout.h)
    size_t (*nr_free_pages)(void);                    // return the number of free pages
    void (*check)(void);                              // check the correctness of XXX_pmm_manager
};

#endif //XXOS_PMM_H
