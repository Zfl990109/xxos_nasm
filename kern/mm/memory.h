//
// Created by zfl on 2022/1/13.
//

#ifndef XXOS_MEMORY_H
#define XXOS_MEMORY_H
#include "../../libs/defs.h"
#include "../../libs/bitmap.h"

#define PG_SIZE 4096
#define MEM_BITMAP_ADDR  0xc009a000
#define KERNEL_HEAP_ADDR 0xc0100000      // 内核堆的起始地址

struct virtual_addr{
    struct bitmap virtual_bitmap;       // 管理虚拟地址的位图
    uint32_t vaddr_start;               // 虚拟地址的起始地址
};

struct pool{
    struct bitmap pool_bitmap;
    uint32_t phy_addr;
    uint32_t size;
};


#define PG_P_1      1       // 页表项或页目录项存在属性位
#define PG_P_0      0
#define PG_RW_R     0       // R/W 属性位, 读 / 执行
#define PG_RW_W     2       // R/W 属性位, 读 / 写 / 执行
#define PG_US_S     0       // U/S 属性位
#define PG_US_U     4
enum pool_flags{
    PF_KERNEL = 1,
    PF_USER = 2
};

extern struct pool kernel_pool, user_pool;

static void mem_pool_init(uint32_t all_mem);

// 从虚拟内存池中申请 pg_count 个虚拟页, 并返回虚拟页的起始地址
static void* apply_virtual_page(enum pool_flags pf, uint32_t pg_count);

// 得到虚拟地址 vaddr 对应的 pte 指针, 0xffc00000 从启动分页处可以得到
uint32_t* pte_ptr(uint32_t vaddr);

// 得到虚拟地址 vaddr 对应的 pde 指针, 0xfffff000 从启动分页处可以得到
uint32_t* pde_ptr(uint32_t vaddr);

// 在 m_pool 指向的物理内存池中分配 1 页, 并返回物理地址
static void* palloc(struct pool* m_pool);

// 在页表中添加虚拟地址 vaddr 和 物理地址 page_phyaddr 的映射
static void page_table_add(void* _vaddr, void* _page_phyaddr);

// 分配 pg_count 个页空间, 成功则返回起始虚拟地址
void* malloc_page(enum pool_flags pf, uint32_t pg_count);

// 在内核内存池中申请 pg_count 数量的页面, 并返回虚拟地址
void* apply_kernel_pages(uint32_t pg_count);

void init_mem(void);

#endif //XXOS_MEMORY_H
