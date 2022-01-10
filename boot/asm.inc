; 参数分别为 段起始地址(32), 段界限(32), 属性(4)
%macro set_null 3
    dd 0
    dd 0
%endmacro
%macro set_segment 3
    dw (%2 >> 12) & 0xffff
    dw %1 & 0xffff
    db (%1 >> 16) & 0xff
    db (%3 | 0x90)    ; 10010000 第 7 位为1表示p存在, 第 4 位为1表示s非系统段
    db (0xc0  | (%2 >> 28))     ;段粒度单位为 4KB, 32 位寄存器寻址
    db (%1 >> 24) & 0xff
%endmacro

SMAP        equ    0x534d4150;      内存探测标记
ards_nr     equ    0x500;           内存探测结果位置
ards_buf    equ    0x504;           内存探测 ards 结构缓冲区

STA_X   equ    0x8     ; Executable segment
STA_E   equ    0x4     ; Expand down (non-executable segments)
STA_C   equ    0x4     ; Conforming code segment (executable only)
STA_W   equ    0x2     ; Writeable (non-executable segments)
STA_R   equ    0x2     ; Readable (executable segments)
STA_A   equ    0x1     ; Accessed

; 选择子
    select_code     equ     0x08
    select_data     equ     0x10
    select_video    equ     0x18

PT_NULL             equ     0

LOADER_ADDR         equ     0x900
KERNEL_BIN_ADDR     equ     0x1000
KERNEL_ADDR         equ     0x100000
KERNEL_STACK        equ     0x9f000
