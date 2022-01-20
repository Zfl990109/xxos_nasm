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
#include "../process/tss.h"
#include "../process/userprog.h"
#include "../device/console.h"

void kernel_thread_a(void*);
void kernel_thread_b(void*);
void u_prog_a(void);
void u_prog_b(void);
void init_all(void);
int test_var_a = 0, test_var_b = 1;

int main(void){
    reset_screen();
    print_str("xxos_nasm is loading ...\n");
    init_all();
    thread_start("consumer_a", 31, kernel_thread_a, " A_");
    thread_start("consumer_b", 31, kernel_thread_b, " B_");
//    process_execute(u_prog_a, "userprog_a");
//    process_execute(u_prog_b, "userprog_b");
    enable_intr();
    while (true);
    return 0;
}

void kernel_thread_a(void* arg){
    char* para = arg;
    while (1){
        enum intr_status old_status = disable_intr();
        if (!ioq_empty(&kbd_buf)) {
            console_print_str(para);
            char byte = ioq_getchar(&kbd_buf);
            console_print_char(byte);
        }
        set_intr_status(old_status);
//        console_print_str("    v_a:0x");
//        console_print_int(test_var_a);
    }
//    print_str(para);
}
void kernel_thread_b(void* arg){
    char* para = arg;
    while (1){
        enum intr_status old_status = disable_intr();
        if (!ioq_empty(&kbd_buf)) {
            console_print_str(para);
            char byte = ioq_getchar(&kbd_buf);
            console_print_char(byte);
        }
        set_intr_status(old_status);
//        console_print_str("    v_b:0x");
//        console_print_int(test_var_b);
    }
//    print_str(para);
}

void u_prog_a(void){
    while (true){
        test_var_a++;
    }
}

void u_prog_b(void){
    while (true){
        test_var_b++;
    }
}

// 初始化所有模块
void init_all(){
    print_str("init all modules...\n");
    init_intr();
    init_timer();
    init_mem();
    init_thread();
    init_console();
    init_keyboard();
    init_tss();
}


