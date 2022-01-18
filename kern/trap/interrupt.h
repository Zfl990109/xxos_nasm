//
// Created by zfl on 2022/1/11.
//

#ifndef XXOS_INTERRUPT_H
#define XXOS_INTERRUPT_H

#include "../../libs/defs.h"

#define IDT_DESC_CNT    0x30         // 支持的中断数

#define PIC_MAIN_CTRL   0x20        //主片控制端口
#define PIC_MAIN_DATA   0x21        //主片数据端口
#define PIC_SUBO_CTRL   0xa0        //从片控制端口, SUBO -- subordinate(从属)的简写
#define PIC_SUBO_DATA   0xa1        //从片数据端口

#define EFLAGS_IF       0x00000200  // eflags 寄存器
#define GET_EFLAGS(EFLAG_VAR) asm volatile("pushfl; popl %0" : "=g"(EFLAG_VAR))

typedef void* intr_handler;



struct gate_desc{
    uint16_t func_offset_low;
    uint16_t selector;
    uint8_t dcount;
    uint8_t attribute;
    uint16_t func_offset_high;
};

static void set_intr_func(uint8_t vector_no);

static void intr_func_regist(void);

static void set_gate(struct gate_desc* gate, uint8_t attr, intr_handler func);

static void init_idt(void);

static void init_pic(void);

void init_intr(void);

enum intr_status{
    INTR_OFF,
    INTR_ON
};

enum intr_status enable_intr(void);

enum intr_status disable_intr(void);

enum intr_status set_intr_status(enum intr_status status);

enum intr_status intr_get_status(void);

void register_intr_func(uint8_t vector_no, intr_handler function);

#endif //XXOS_INTERRUPT_H
