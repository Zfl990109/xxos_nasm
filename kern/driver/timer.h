//
// Created by zfl on 2022/1/12.
//

#ifndef XXOS_TIMER_H
#define XXOS_TIMER_H
#include "../../libs/defs.h"

#define IRQ0_FREQUENCY      100
#define INPUT_FREQUENCY     1193182
#define COUNTER0_VALUE      INPUT_FREQUENCY / IRQ0_FREQUENCY
#define COUNTER0_PORT       0x40
#define COUNTER0_NO         0
#define COUNTER_MODE        2
#define READ_WRITE_LATCH    3
#define PIT_CONTROL_PORT    0x43

static void set_frequency(uint8_t counter_port, uint8_t counter_no,
                          uint8_t rwl, uint8_t counter_mode, uint16_t counter_value);

static void intr_time_func(void);

void init_timer(void);
#endif //XXOS_TIMER_H
