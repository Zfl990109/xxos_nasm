%include "asm.inc"
org 0x7c00
SECTION MBR
bits 16
start:
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov sp, 0x7c00
; ======================================================
; 清屏
; ______________________________________________________
    mov ax, 0x600
    mov bx, 0x600
    mov cx, 0
    mov dx, 0x184f
    int 0x10
; ======================================================
; 打开 A20 开关
; ______________________________________________________
    in al, 0x92
    or al, 0x02
    out 0x92, al
; ======================================================
; 加载 gdt
; ______________________________________________________
    lgdt [gdt_ptr]
; ======================================================
; 将 cr0 的 0 号位置 1
; ______________________________________________________
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp select_code:loader

bits 32
loader:
    mov ax, select_video
    mov gs, ax
    mov byte [gs:0x0], 'p'
    mov byte [gs:0x01], 0x0b
    jmp $
; 选择子
    select_code     equ     0x08
    select_data     equ     0x10
    select_video    equ     0x18
; GDT
    gdt:    set_null                0,              0,          0
    code:   set_segment             0,     0xffffffff,          STA_X | STA_R
    data:   set_segment             0,     0xffffffff,          STA_W
    video:  set_segment       0xb8000,         0xffff,          STA_W

    gdt_ptr     dw  0x1f
                dd  gdt

    times 510-($-$$) db 0
    dw 0xaa55

