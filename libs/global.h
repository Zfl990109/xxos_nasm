//
// Created by zfl on 2022/1/11.
//

#ifndef XXOS_GLOBAL_H
#define XXOS_GLOBAL_H

#include "defs.h"

#define GDT_BASE    0xc0007d01

#define RPL0    0
#define RPL1    1
#define RPL2    2
#define RPL3    3

#define TI_GDT 0
#define TI_LDT 1

// GDT 描述符属性
#define DESC_G_4K           1
#define DESC_D_32           1
#define DESC_L              0
#define DESC_AVL            0
#define DESC_P              1
#define DESC_DPL_0          0
#define DESC_DPL_1          1
#define DESC_DPL_2          2
#define DESC_DPL_3          3

#define DESC_S_CODE         1
#define DESC_S_DATA         DESC_S_CODE
#define DESC_S_SYS          0
#define DESC_TYPE_CODE      8
#define DESC_TYPE_DATA      2
#define DESC_TYPE_TSS       9

#define KERNEL_CODE     ((1 << 3) + (TI_GDT << 2) + RPL0)
#define KERNEL_DATA     ((2 << 3) + (TI_GDT << 2) + RPL0)
#define KERNEL_STACK    KERNEL_DATA
#define KERNEL_VIDEO    ((3 << 3) + (TI_GDT << 2) + RPL0)
#define TSS             ((4 << 3) + (TI_GDT << 2) + RPL0)
#define USER_CODE       ((5 << 3) + (TI_GDT << 2) + RPL3)
#define USER_DATA       ((6 << 3) + (TI_GDT << 2) + RPL3)
#define USER_STACK      USER_DATA

#define GDT_ATTR_HIGH       \
        ((DESC_G_4K << 7) + (DESC_D_32 << 6) + (DESC_L << 5) + (DESC_AVL << 4))
#define GDT_CODE_ATTR_LOW_DPL3       \
        ((DESC_P << 7) + (DESC_DPL_3 << 5) + (DESC_S_CODE << 4) + DESC_TYPE_CODE)
#define GDT_DATA_ATTR_LOW_DPL3       \
        ((DESC_P << 7) + (DESC_DPL_3 << 5) + (DESC_S_DATA << 4) + DESC_TYPE_DATA)

// TSS 描述符属性
#define TSS_DESC_D          0
#define TSS_ATTR_HIGH       \
        ((DESC_G_4K << 7) + (TSS_DESC_D << 6) + (DESC_L << 5) + (DESC_AVL << 4) + 0x0)
#define TSS_ATTR_LOW       \
        ((DESC_P << 7) + (DESC_DPL_0 << 5) + (DESC_S_SYS << 4) + DESC_TYPE_TSS)


// IDT描述符的属性
#define IDT_DESC_P          1
#define IDT_DESC_DPL0       0
#define IDT_DESC_DPL3       3
#define IDT_DESC_TYPE_32    0xe
#define IDT_DESC_TYPE_16    0x6

#define IDT_DESC_ATTR_DPL0 \
    ((IDT_DESC_P << 7) + (IDT_DESC_DPL0 << 5) + IDT_DESC_TYPE_32)
#define IDT_DESC_ATTR_DPL3 \
    ((IDT_DESC_P << 7) + (IDT_DESC_DPL3 << 5) + IDT_DESC_TYPE_32)

struct gdt_desc{
    uint16_t limit_low_word;
    uint16_t base_low_word;
    uint8_t base_mid_byte;
    uint8_t attr_low_byte;
    uint8_t limit_high_attr_high;
    uint8_t base_high_byte;
};

#define EFLAGS_MBS      (1 << 1)
#define EFLAGS_IF_1     (1 << 9)
#define EFLAGS_IF_0     0
#define EFLAGS_IOPL_3   (3 << 12)
#define EFLAGS_IOPL_0   (0 << 12)
#define DIV_ROUND_UP(X, STEP) ((X + STEP - 1) / (STEP))


#endif //XXOS_GLOBAL_H
