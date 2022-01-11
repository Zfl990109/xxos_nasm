//
// Created by zfl on 2022/1/11.
//

#ifndef XXOS_GLOBAL_H
#define XXOS_GLOBAL_H

#include "../../libs/defs.h"

#define RPL0    0
#define RPL1    1
#define RPL2    2
#define RPL3    3

#define TI_GDT 0
#define TI_LDT 1

#define KERNEL_CODE     ((1 << 3) + (TI_GDT << 2) + RPL0)
#define KERNEL_DATA     ((2 << 3) + (TI_GDT << 2) + RPL0)
#define KERNEL_STACK    KERNEL_DATA
#define KERNEL_VIDEO    ((3 << 3) + (TI_GDT << 2) + RPL0)

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


#endif //XXOS_GLOBAL_H
