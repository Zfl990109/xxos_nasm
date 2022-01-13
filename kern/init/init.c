//
// Created by zfl on 2022/1/8.
//
#include "../../libs/print.h"
#include "../trap/interrupt.h"
#include "../driver/timer.h"
#include "../debug/assert.h"
#include "../mm/memory.h"
#include "../../libs/string.h"

void init_all(void);

void main(void){
    reset_screen();
    print_str("xxos is loading ...\n");
    init_all();
//    ASSERT(strcmp("bbb", "bbb"));
    void* addr = apply_kernel_pages(3);
    print_str("\n apply_kernel_page start vaddr is ");
    print_int((uint32_t)addr);
    print_str("\n");
//    asm volatile("sti");
    //TODO:分页处理
//    int i = 100000;
//    while (i--){}
//    asm volatile("cli");
    while (1);
}

// 初始化所有模块
void init_all(){
    print_str("init all modules...\n");
    init_intr();
    init_timer();
    init_mem();
}


