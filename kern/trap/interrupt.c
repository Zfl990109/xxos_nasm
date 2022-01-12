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
// 中断名称
char* intr_name[IDT_DESC_CNT];
// 中断处理函数
intr_handler intr_func_table[IDT_DESC_CNT];

static void set_intr_func(uint8_t vector_no){
    if (vector_no == 0x27 || vector_no == 0x2f)
        return;
    print_str("int vector: 0x");
    print_int(vector_no);
    print_char('\n');
}

static void intr_func_regist(void){
    int i;
    for (i = 0; i < IDT_DESC_CNT; ++i) {
        intr_func_table[i] = set_intr_func;
        intr_name[i] = "unknow";
    }
    intr_name[0] = "#DE Divide Error";
    intr_name[1] = "#DB Debug Exception";
    intr_name[2] = "#NMI Interrupt";
    intr_name[3] = "#BP Breakpoint Exception";
    intr_name[4] = "#OF Overflow Exception";
    intr_name[5] = "#BR BOUND Range Exceeded Exception";
    intr_name[6] = "#UD Invalid Opcode Exception";
    intr_name[7] = "#NM Device Not Available Exception";
    intr_name[8] = "#DF Double Fault Exception";
    intr_name[9] = "Coprocessor Segment Overturn";
    intr_name[10] = "#TS Invalid TSS Exception";
    intr_name[11] = "#NP Segment Not Present";
    intr_name[12] = "#SS Stack Fault Exception";
    intr_name[13] = "#GP General Protection Exception";
    intr_name[14] = "#PF Page-Fault Exception";
//    intr_name[15] = "#DE Divide Error";   intel 保留项, 未使用
    intr_name[16] = "#MF x87 FPU Floating-Point Error";
    intr_name[17] = "#AC Alignment Check Exception";
    intr_name[18] = "#MC Machine-Check Exception";
    intr_name[19] = "#XF SIMD Floating-Point Exception";
}

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
    intr_func_regist();
    init_pic();

    // 加载 idt
    uint64_t idt_ptr = ((sizeof(idt) - 1) | ((uint64_t)((uint32_t)idt << 16)));
    asm volatile("lidt %0": :"m"(idt_ptr));
    print_str("interrupt init done!\n");
}



