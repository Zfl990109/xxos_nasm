//
// Created by zfl on 2022/1/8.
//
#include "../../libs/print.h"
#include "../trap/interrupt.h"
#include "../driver/timer.h"
#include "../debug/assert.h"
#include "../mm/memory.h"
#include "../../libs/string.h"
#include "../process/thread.h"
void kernel_thread_a(void*);
void kernel_thread_b(void*);
void init_all(void);

int main(void){
    reset_screen();
    print_str("xxos_nasm is loading ...\n");
    init_all();
//    ASSERT(strcmp("bbb", "bbb"));
//    void* addr = apply_kernel_pages(3);
    thread_start("kernel_thread_a", 31, kernel_thread_a, "argA ");
    thread_start("kernel_thread_b", 8, kernel_thread_b, "argB ");
//    asm volatile("sti");
    //TODO:分页处理
//    int i = 100000;
//    while (i--){}
//    asm volatile("cli");
    while (true){
        disable_intr();
        print_str("MAIN ");
        enable_intr();
    }
    return 0;
}

void kernel_thread_a(void* arg){
    char* para = arg;
    while (1){
        disable_intr();
        print_str(para);
        enable_intr();
    }
//    print_str(para);
}
void kernel_thread_b(void* arg){
    char* para = arg;
    while (1){
        disable_intr();
        print_str(para);
        enable_intr();
    }
//    print_str(para);
}

// 初始化所有模块
void init_all(){
    print_str("init all modules...\n");
    init_intr();
    init_timer();
    init_mem();
    init_thread();
}


