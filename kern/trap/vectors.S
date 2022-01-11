[bits 32]
%define error_code nop
%define zero push 0

extern print_str
global intr_entry_table

section .data
intr_str db "interrupt occur!", 0xa, 0
intr_entry_table:

%macro vector 2
section .text
intr_%1_entry:
    %2
    ; 保存上下文环境
    push intr_str
    call print_str
    add esp, 4

    mov al, 0x20
    out 0xa0, al
    out 0x20, al

    add esp, 4
    iret

section .data
    dd intr_%1_entry

%endmacro

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
vector 0x1f, zero
vector 0x20, zero

