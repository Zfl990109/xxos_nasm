//
// Created by zfl on 2022/1/11.
//

#include "../../libs/defs.h"
#include "global.h"
#include "interrupt.h"
#include "../../libs/print.h"
#include "../../libs/io.h"

// IDT 中断描述符表
static struct gate_desc idt[IDT_DESC_CNT];
// 声明在 vectors.S 中的中断处理函数入口
extern intr_handler intr_entry_table[IDT_DESC_CNT];

char* intr_name[IDT_DESC_CNT];

//  设置中断描述符
static void set_gate(struct gate_desc* gate, uint8_t attr, intr_handler func){
    gate->func_offset_low = (uint32_t) func & 0x0000ffff;
    gate->selector = KERNEL_CODE;
    gate->dcount = 0;
    gate->attribute = attr;
    gate->func_offset_high = ((uint32_t)func & 0xffff0000) >> 16;
}

// 初始化中断描述符表
static void init_idt(void){
    int i;
    for (i = 0; i < IDT_DESC_CNT; ++i) {
        set_gate(&idt[i], IDT_DESC_ATTR_DPL0, intr_entry_table[i]);
    }
    print_str("    idt init done!\n");
}

#define PIC_MAIN_CTRL   0x20        //主片控制端口
#define PIC_MAIN_DATA   0x21        //主片数据端口
#define PIC_SUBO_CTRL   0xa0        //从片控制端口, SUBO -- subordinate(从属)的简写
#define PIC_SUBO_DATA   0xa1        //从片数据端口
// 对可编程中断处理芯片初始化(初始化 8259A)
static void init_pic(void){
    // 初始化主片
    outb(PIC_MAIN_CTRL, 0x11);      // ICW1:  边沿触发, 级联, 需要ICW4
    outb(PIC_MAIN_DATA, 0x20);      // ICW2:  起始中断向量号为 0x20, IRQ[0~7] 为 0x20~0x27
    outb(PIC_MAIN_DATA, 0x04);      // ICW3:  第 2 位置 1, 表示从片连接在 IRQ2 的引脚
    outb(PIC_MAIN_DATA, 0x01);      // ICW4:  8086模式, 正常 EOI

    // 初始化从片
    outb(PIC_SUBO_CTRL, 0x11);      // ICW1:  边沿触发, 级联, 需要ICW4
    outb(PIC_SUBO_DATA, 0x28);      // ICW2:  起始中断向量号为 0x28, IRQ[8~15] 为 0x28~0x2f
    outb(PIC_SUBO_DATA, 0x02);      // ICW3:  表示从片连接在主片的 IRQ2 的引脚上
    outb(PIC_SUBO_DATA, 0x01);      // ICW4:  8086模式, 正常 EOI

    // 打开主片上的 IRQ0, 即打开时钟中断
    outb(PIC_MAIN_DATA, 0xfe);
    outb(PIC_SUBO_DATA, 0xff);

    print_str("    pic init done!\n");

}

// 中断初始化
void init_intr(){
    print_str("interrupt init start ...\n");
    init_idt();
    init_pic();

    // 加载 idt
    uint64_t idt_ptr = ((sizeof(idt) - 1) | ((uint64_t)((uint32_t)idt << 16)));
    asm volatile("lidt %0": :"m"(idt_ptr));
    print_str("interrupt init done!\n");
}



