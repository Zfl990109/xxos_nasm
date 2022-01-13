

#ifndef XXOS_MEMLAYOUT_H
#define XXOS_MEMLAYOUT_H
#include "../../libs/defs.h"

typedef uintptr_t pte_t;
typedef uintptr_t pde_t;

// 0x15 中断 0xe820 功能的常量
#define E820MAX             20      // number of entries in E820MAP
#define E820_ARM            1       // address range memory
#define E820_ARR            2       // address range reserved

struct e820map {
    int32_t nr_map;
    struct {
        uint64_t addr;              // 内存块首地址
        uint64_t size;              // 内存块大小
        uint32_t type;              // 内存块类型
    } __attribute__((packed)) map[E820MAX];
};

//
struct Page {
    int32_t ref;                        // 页面的引用计数
    uint32_t flags;                 // 描述页面状态
    uint32_t property;
    list_entry_t page_link;         // 空闲页链表
};

// 描述页面状态的标志位
#define PG_reserved                 0       // 0 表示页面用于 kernel 或者不能使用
#define PG_property                 1       // 0 表示页面已经被分配, 1 表示空闲

#define SetPageReserved(page)       set_bit(PG_reserved, &((page)->flags))
#define ClearPageReserved(page)     clear_bit(PG_reserved, &((page)->flags))
#define PageReserved(page)          test_bit(PG_reserved, &((page)->flags))
#define SetPageProperty(page)       set_bit(PG_property, &((page)->flags))
#define ClearPageProperty(page)     clear_bit(PG_property, &((page)->flags))
#define PageProperty(page)          test_bit(PG_property, &((page)->flags))

// 将链表地址转化为页表数据结构
#define le2page(le, member) to_struct((le), struct Page, member)

// free_area_t用来描述空闲内存块, 双向链表
typedef struct {
    list_entry_t free_list;         // 链表头
    uint32_t nr_free;           // 空闲链表中的空闲页个数
} free_area_t;
#endif// XXOS_MEMLAYOUT_H