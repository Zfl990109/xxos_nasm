[bits 32]
%define error_code nop
%define zero push 0

extern print_str
extern intr_func_table
global intr_entry_table

section .data
intr_str db "interrupt occur!", 0xa, 0
intr_entry_table:

%macro vector 2
section .text
intr_%1_entry:
    %2
    ; 保存上下文环境
    push ds
    push es
    push fs
    push gs
    pushad
    mov al, 0x20            ; 中断结束命令EOI
    out 0xa0, al            ; 向从片发送
    out 0x20, al            ; 向主片发送
    push %1                 ; 压入中断向量号
    call [intr_func_table + %1 * 4]     ; 调用由 c 语言实现的中断处理函数
    jmp intr_exit

section .data
    dd intr_%1_entry

%endmacro

section .text
global intr_exit
intr_exit:
    add esp, 4              ; 跳过中断号
    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp, 4              ; 跳过 error_code
    iretd

vector 0x00, zero
vector 0x01, zero
vector 0x02, zero
vector 0x03, zero
vector 0x04, zero
vector 0x05, zero
vector 0x06, zero
vector 0x07, zero
vector 0x08, error_code
vector 0x09, zero
vector 0x0a, error_code
vector 0x0b, error_code
vector 0x0c, error_code
vector 0x0d, error_code
vector 0x0e, error_code
vector 0x0f, zero
vector 0x10, zero
vector 0x11, error_code
vector 0x12, zero
vector 0x13, zero
vector 0x14, zero
vector 0x15, zero
vector 0x16, zero
vector 0x17, zero
vector 0x18, zero
vector 0x19, zero
vector 0x1a, zero
vector 0x1b, zero
vector 0x1c, zero
vector 0x1d, zero
vector 0x1e, error_code
vector 0x1f, zero               ; 前32个用于异常
vector 0x20, zero               ; 时钟中断对应的入口
vector 0x21, zero               ; 键盘中断对应的入口
vector 0x22, zero               ; 级联
vector 0x23, zero               ; 串口 2 对应的入口
vector 0x24, zero               ; 串口 1 对应的入口
vector 0x25, zero               ; 并口 2 对应的入口
vector 0x26, zero               ; 软盘对应的入口
vector 0x27, zero               ; 并口 1 对应的入口
vector 0x28, zero               ; 实时时钟对应的入口
vector 0x29, zero               ; 重定向
vector 0x2a, zero               ; 保留
vector 0x2b, zero               ; 保留
vector 0x2c, zero               ; ps/2 鼠标
vector 0x2d, zero               ; fpu 浮点单元异常
vector 0x2e, zero               ; 硬盘
vector 0x2f, zero               ; 保留

