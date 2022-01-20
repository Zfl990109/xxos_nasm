//
// Created by zfl on 2022/1/12.
//
#include "timer.h"
#include "../../libs/io.h"
#include "../../libs/print.h"
#include "../../libs/defs.h"
#include "../trap/interrupt.h"
#include "../process/thread.h"
#include "../debug/assert.h"
uint32_t ticks;

// 设置定时器频率
static void set_frequency(uint8_t counter_port, uint8_t counter_no,
                          uint8_t rwl, uint8_t counter_mode, uint16_t counter_value){
    outb(PIT_CONTROL_PORT, (uint8_t)(counter_no << 6 | rwl << 4 | counter_mode << 1));
    outb(counter_port, (uint8_t)counter_value);
    outb(counter_port, (uint8_t)counter_value);
}

static void intr_time_func(void){
    struct task_struct* cur_thread = running_thread();
    ASSERT(cur_thread->stack_magic == 0x19990109);
    cur_thread->elapsed_ticks++;
    ticks++;
//    print_str(cur_thread->name);
//    print_str(" ticks ......\n");
    if (cur_thread->ticks == 0){
//        print_str(cur_thread->name);
//        print_str(" ticks is 0, schedule...\n");
        schedule();
    }
    else
        cur_thread->ticks--;
}

// 初始化 PIT8253
void init_timer(){
    print_str("timer init start ...\n");
    set_frequency(COUNTER0_PORT, COUNTER0_NO, READ_WRITE_LATCH, COUNTER_MODE, COUNTER0_VALUE);
    register_intr_func(0x20, intr_time_func);
    print_str("timer init done!\n");
}
