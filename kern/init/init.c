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

extern struct ioqueue kbd_buf;	   // 定义键盘缓冲区
int test_var_a = 2, test_var_b = 1;

int main(void){
    put_str("xxos_nasm is loading ...\n");
    init_all();
    thread_start("consumer_a", 31, kernel_thread_a, " A_");
    thread_start("consumer_b", 31, kernel_thread_b, " B_");
//    process_execute(u_prog_a, "userprog_a");
//    process_execute(u_prog_b, "userprog_b");
    intr_enable();
    while (true);
    return 0;
}

void kernel_thread_a(void* arg){
    char* para = arg;
    while (1){
        enum intr_status old_status = intr_disable();
        if (!ioq_empty(&kbd_buf)) {
            char byte = ioq_getchar(&kbd_buf);
            console_put_str(para);
            console_put_char(byte);
        }
        intr_set_status(old_status);
    }
}
void kernel_thread_b(void* arg){
    char* para = arg;
    while (1){
        enum intr_status old_status = intr_disable();
        if (!ioq_empty(&kbd_buf)) {
            char byte = ioq_getchar(&kbd_buf);
            console_put_str(para);
            console_put_char(byte);
        }
        intr_set_status(old_status);
    }
}

void u_prog_a(void){
    while (true){
//        console_print_int(test_var_a);
        test_var_a++;
    }
}

void u_prog_b(void){
    while (true){
//        console_print_int(test_var_b);
        test_var_b++;
    }
}

// 初始化所有模块
void init_all(){
    put_str("init_all\n");
    idt_init();	     // 初始化中断
    mem_init();	     // 初始化内存管理系统
    thread_init();    // 初始化线程相关结构
    timer_init();     // 初始化PIT
    console_init();   // 控制台初始化最好放在开中断之前
    keyboard_init();  // 键盘初始化
    tss_init();       // tss初始化
}


