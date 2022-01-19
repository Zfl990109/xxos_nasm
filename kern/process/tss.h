//
// Created by zfl on 2022/1/19.
//

#ifndef XXOS_NASM_TSS_H
#define XXOS_NASM_TSS_H

#include "../../libs/defs.h"
#include "thread.h"
struct tss{
    uint32_t backlink;
    uint32_t* esp0;
    uint32_t ss0;
    uint32_t* esp1;
    uint32_t ss1;
    uint32_t* esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t (*eip)(void);
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint32_t trace;
    uint32_t io_base;
};
static struct tss tss;

// 将 tss 中的 esp0 字段的值更新为 pthread 的0级栈
void update_tss_esp(struct task_struct* pthread);

//创建 gdt 描述符
static struct gdt_desc create_gdt_desc(uint32_t* desc_addr, uint32_t limit, uint8_t attr_low, uint8_t attr_high);

// 在 gdt 中创建 tss 并重新加载 gdt
void init_tss();

#endif //XXOS_NASM_TSS_H
