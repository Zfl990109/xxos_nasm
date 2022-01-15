//
// Created by zfl on 2022/1/10.
// 实现物理内存管理
//

#ifndef XXOS_PMM_H
#define XXOS_PMM_H
struct pmm_manager {
    const char *name;                                 //  内存管理方式
    void (*init)(void);                               // 初始化管理器
    void (*init_memmap)(struct Page *base, size_t n); // 根据空闲空间来初始化内存管理器
    struct Page *(*alloc_pages)(size_t n);            // 分配 >= n 个页面
    void (*free_pages)(struct Page *base, size_t n);  // 释放 >= n 个页面
    size_t (*nr_free_pages)(void);                    // 返回空闲页面个数
    void (*check)(void);                              // 验证管理器是否正常
};

#endif //XXOS_PMM_H
