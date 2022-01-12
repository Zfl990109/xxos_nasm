//
// Created by zfl on 2022/1/12.
//
#include "timer.h"
#include "../../libs/io.h"
#include "../../libs/print.h"
#include "../../libs/defs.h"

// 设置定时器频率
static void set_frequency(uint8_t counter_port, uint8_t counter_no,
                          uint8_t rwl, uint8_t counter_mode, uint16_t counter_value){
    outb(PIT_CONTROL_PORT, (uint8_t)(counter_no << 6 | rwl << 4 | counter_mode << 1));
    outb(counter_port, (uint8_t)counter_value);
    outb(counter_port, (uint8_t)counter_value);

}

// 初始化 PIT8253
void init_timer(){
    print_str("timer init start ...\n");
    set_frequency(COUNTER0_PORT, COUNTER0_NO, READ_WRITE_LATCH, COUNTER_MODE, COUNTER0_VALUE);
    print_str("timer init done!\n");
}
