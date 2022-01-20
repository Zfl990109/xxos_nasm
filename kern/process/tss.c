//
// Created by zfl on 2022/1/19.
//

#include "tss.h"
#include "thread.h"
#include "../mm/memory.h"
#include "../../libs/print.h"
#include "../../libs/string.h"
#include "../../libs/global.h"
#include "../debug/assert.h"

// 将 tss 中的 esp0 字段的值更新为 pthread 的0级栈
void update_tss_esp(struct task_struct* pthread){
    tss.esp0 = (uint32_t*)((uint32_t)pthread + PG_SIZE);
    return;
}

//创建 gdt 描述符
static struct gdt_desc create_gdt_desc(uint32_t* desc_addr, uint32_t limit, uint8_t attr_low, uint8_t attr_high){
    uint32_t desc_base = (uint32_t) desc_addr;
    struct gdt_desc desc;
    desc.limit_low_word = limit & 0x0000ffff;
    desc.base_low_word = desc_base & 0x0000ffff;
    desc.base_mid_byte = ((desc_base & 0x00ff0000) >> 16);
    desc.attr_low_byte = attr_low;
    desc.limit_high_attr_high = (((limit & 0x000f0000) >> 16) + (uint8_t)(attr_high));
    desc.base_high_byte = desc_base >> 24;
    return desc;

}

// 在 gdt 中创建 tss 并重新加载 gdt
void init_tss(){
    print_str("tss_init start ... \n");
    uint32_t tss_size = sizeof(tss);
    memset(&tss, 0, tss_size);
    tss.ss0 = KERNEL_STACK;
    tss.io_base = tss_size;
//    ASSERT(1 == 2);
    // 在 gdt 中添加 DPL 为 0 的 TSS 描述符
    *((struct gdt_desc*)(GDT_BASE + 0x20)) =
            create_gdt_desc((uint32_t*)&tss, tss_size - 1, TSS_ATTR_LOW, TSS_ATTR_HIGH);

    // 在 gdt 中添加 DPL 为 3 的用户代码段描述符
    *((struct gdt_desc*)(GDT_BASE + 0x28)) =
            create_gdt_desc((uint32_t*)0, 0xffffffff, GDT_CODE_ATTR_LOW_DPL3, GDT_ATTR_HIGH);

    // 在 gdt 中添加 DPL 为 3 的用户数据段描述符
    *((struct gdt_desc*)(GDT_BASE + 0x30)) =
            create_gdt_desc((uint32_t*)0, 0xffffffff, GDT_DATA_ATTR_LOW_DPL3, GDT_ATTR_HIGH);

    uint64_t gdt_ptr = ((8 * 7 - 1) | ((uint64_t)(uint32_t)GDT_BASE << 16));
    asm volatile("lgdt %0": : "m"(gdt_ptr));
    asm volatile("ltr %w0": : "r"(TSS));
    print_str("tss_init and ltr done!\n");
}

