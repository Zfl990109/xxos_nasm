//
// Created by zfl on 2022/1/8.
//
#include "../../libs/print.h"
#include "../trap/interrupt.h"
#include "../driver/timer.h"
#include "../device/keyboard.h"
#include "../device/ioqueue.h"
#include "../debug/assert.h"
#include "../mm/memory.h"
#include "../../libs/string.h"
#include "../process/thread.h"
#include "../device/console.h"

void kernel_thread_a(void*);
void kernel_thread_b(void*);
void init_all(void);

int main(void){
    reset_screen();
    print_str("xxos_nasm is loading ...\n");
    init_all();
//    ASSERT(strcmp("bbb", "bbb"));
//    void* addr = apply_kernel_pages(3);
//    thread_start("consumer_a", 31, kernel_thread_a, " A_");
//    thread_start("consumer_b", 31, kernel_thread_b, " B_");
    enable_intr();
//    asm volatile("sti");
    //TODO:分页处理
//    int i = 100000;
//    while (i--){}
//    asm volatile("cli");
    while (true);
    return 0;
}

//void kernel_thread_a(void* arg){
//    char* para = arg;
//    while (1){
//        enum intr_status old_status = disable_intr();
//        if (!ioq_empty(&kbd_buf)){
//            console_print_str(para);
//            char byte = ioq_getchar(&kbd_buf);
//            console_print_char(byte);
//        }
//        set_intr_status(old_status);
//    }
////    print_str(para);
//}
//void kernel_thread_b(void* arg){
//    char* para = arg;
//    while (1){
//        enum intr_status old_status = disable_intr();
//        if (!ioq_empty(&kbd_buf)){
//            console_print_str(para);
//            char byte = ioq_getchar(&kbd_buf);
//            console_print_char(byte);
//        }
//        set_intr_status(old_status);
//    }
////    print_str(para);
//}

// 初始化所有模块
void init_all(){
    print_str("init all modules...\n");
    init_intr();
    init_timer();
    init_mem();
    init_thread();
    init_console();
    init_keyboard();
}


