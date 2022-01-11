//
// Created by zfl on 2022/1/11.
//

#ifndef XXOS_INTERRUPT_H
#define XXOS_INTERRUPT_H

#include "../../libs/defs.h"

#define IDT_DESC_CNT 0x21           // 支持的中断数
typedef void* intr_handler;

struct gate_desc{
    uint16_t func_offset_low;
    uint16_t selector;
    uint8_t dcount;
    uint8_t attribute;
    uint16_t func_offset_high;
};

static void set_gate(struct gate_desc* gate, uint8_t attr, intr_handler func);

static void init_idt(void);

static void init_pic(void);

void init_intr(void);

#endif //XXOS_INTERRUPT_H
