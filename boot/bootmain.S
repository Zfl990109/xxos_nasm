%include "asm.inc"

SECTION BOOTMAIN vstart=0x900
bits 32
main:
    mov eax, loader_msg
    mov ebx, 0x2
    mov ecx, loader_len
    call print
    mov eax, 4
    mov ebx, KERNEL_BIN_ADDR
    mov ecx, 10
    call read_mem
    call kernel_init
    mov eax, kern_msg
    mov ebx, 0x3
    mov ecx, kern_msg_len
    call print
    mov esp, KERNEL_ADDR
    jmp select_code:KERNEL_ADDR


    loader_msg: db "loader kernel..."
    loader_len equ $ - loader_msg
    kern_msg: db "kernel is inited!"
    kern_msg_len equ $ - kern_msg

; ======================================================
; print(char* str, uint_32 line, uint_32 len) 函数, str 表示字符串的起始位置, line 表示行数, len 表示字符串长度
; ______________________________________________________
bits 32
print:
    push eax
    push ebx
    push ecx
    mov esi, eax;   esi = eax = 字符串起始地址
    mov edi, ebx;   edi = ebx = 行数
    mov ax, select_data
    mov ds, ax
    mov ax, select_video
    mov gs, ax
    mov eax, edi
    mov ebx, 160
    mul ebx
    mov edx, eax
    mov ah, 0x0b
    .pt:
        mov byte al, [ds:esi]
        mov word [gs:edx], ax
        inc esi
        add edx, 0x2
        loop .pt
    pop ecx
    pop ebx
    pop eax
    ret
; ======================================================
; kernel_init() 函数
; ebx 表示程序头表地址, cx 表示程序头表中的 program header 个数, dx 表示 program header 尺寸
; ______________________________________________________
kernel_init:
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    mov dx, [KERNEL_BIN_ADDR + 42]
    mov ebx, [KERNEL_BIN_ADDR + 28]
    add ebx, KERNEL_BIN_ADDR
    mov cx, [KERNEL_BIN_ADDR + 44]
    .each_segment:
        cmp byte [ebx + 0], PT_NULL
        je .ptnull
        push dword [ebx + 16]
        mov eax, [ebx + 4]
        add eax, KERNEL_BIN_ADDR
        push eax
        push dword [ebx + 8]
        call mem_cpy
        add esp, 12
    .ptnull:
        add ebx, edx
    loop .each_segment
    ret
; ======================================================
; mem_cpy(void* dest, void* src, uint_32 size) 函数
; dest 表示读入内存的目的地址, src 表示源地址, size 表示读取字节数
; ______________________________________________________
mem_cpy:
    cld
    push ebp
    mov ebp, esp
    push ecx
    mov edi, [ebp + 8]
    mov esi, [ebp + 12]
    mov ecx, [ebp + 16]
    rep movsb
    pop ecx
    pop ebp
    ret

; ======================================================
; read_men(void* dest, uint_32 sector_no, uint_32 count) 函数
; dest 表示读入内存的目的地址, sector_no 表示起始扇区, count 表示读取的扇区个数
; ______________________________________________________
bits 32
read_mem:
    push eax;           eax 表示读取的扇区起始编号
    push ebx;           ebx 表示读入的目的地址
    push ecx;           ecx 表示读取的扇区个数
    mov esi, eax;       esi = eax = 扇区起始编号
    mov edi, ecx;       edi = ecx = 读取扇区个数

    mov edx, 0x1f2
    mov al, cl
    out dx, al;               设置读取的扇区个数, 设置 0x1f2

    mov eax, esi
    mov dx, 0x1f3
    out dx, al;                设置读取的起始扇区编号, 设置 0x1f3
    mov cl, 8
    shr eax, cl
    mov dx, 0x1f4
    out dx, al;                设置 0x1f4
    shr eax, cl;
    mov dx, 0x1f5
    out dx, al;                设置 0x1f5
    shr eax, cl
    and al, 0x0f
    or al, 0xe0;
    mov dx, 0x1f6
    out dx, al;                设置 0x1f6 及相关参数
    mov al, 0x20
    mov dx, 0x1f7
    out dx, al;              设置 0x1f7, 读命令
    .wait_disk:                            ; 等待硬盘
        in al, dx
        and al, 0xc0
        cmp al, 0x40
        jnz .wait_disk

    mov eax, edi;                           设置读取次数, 每次读取两个字节, 因此总共次数为 ecx * 256
    mov edx, 256
    mul edx;
    mov ecx, eax;                           利用 ecx 和 loop 指令自动判断

    mov dx, 0x1f0
    .read_data:
        in ax, dx
        mov [ebx], ax
        add ebx, 2
        loop .read_data
    pop ecx
    pop ebx
    pop eax
    ret







