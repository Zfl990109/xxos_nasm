%include "asm.inc"

SECTION BOOTMAIN vstart=LOADER_ADDR
[bits 32]
main:
    mov esp, LOADER_ADDR
    ; 读取内核
    mov eax, 5
    mov ebx, KERNEL_BIN_ADDR
    mov ecx, 100
    call read_mem

    ; 开启分页
    call setup_page
    sgdt [gdt_base]
    mov ebx, [gdt_base + 2]
    or dword [ebx + 0x18 + 4], 0xc0000000
    add dword [gdt_base + 2], 0xc0000000
    add esp, 0xc0000000
    mov eax, PDT_ADDR
    mov cr3, eax
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    lgdt [gdt_base]
    mov eax, page_msg
    mov ebx, 0x2
    mov ecx, page_msg_len
    call print

    jmp select_code:enter_kernel

enter_kernel:
    call kernel_init
    mov eax, kern_msg
    mov ebx, 0x3
    mov ecx, kern_msg_len
    call print
    mov esp, KERNEL_STACK
    mov ax, select_code
    jmp select_code:KERNEL_ADDR

    page_msg: db "page table is established"
    page_msg_len equ $ - page_msg
    kern_msg: db "kernel is inited!"
    kern_msg_len equ $ - kern_msg


[bits 32]
; ======================================================
; setup_page 函数, 开启分页机制
; ______________________________________________________
setup_page:
    mov ecx, 4096;
    mov esi, 0
    .clear_page_dir:                        ; 将页目录表所在空间全清 0
        mov byte [PDT_ADDR + esi], 0
        inc esi
        loop .clear_page_dir
    .create_pde:                            ; 创建页目录表
        mov eax, PDT_ADDR
        add eax, 0x1000
        mov ebx, eax                        ; 此时 ebx 寄存器中的值为第一个页表所在的位置
        or eax, PG_US_U | PG_RW_W | PG_P
        mov [PDT_ADDR + 0x0], eax           ; 设置第一个页目录项
        mov [PDT_ADDR + 0xc00], eax         ; 设置第 768 个页目录项(内核的第一个页目录项), 设置 3 GB 以上的内存空间的页目录表
                                            ; 令第 768 个和第 1 个页目录项都指向第一个页表,
                                            ; 保证高 1GB 内存空间中的操作系统能够映射到最低的 1GB空间中
                                            ; 使得每个用户程序都能够与操作系统结合
        sub eax, 0x1000
        mov [PDT_ADDR + 4092], eax          ; 最后一个页目录项指向页目录表基址, 实现对页目录的访问
    mov ecx, 256                            ; 二级页表中的页表项为 256 个, 1KB / 4 = 256
    mov esi, 0
    mov edx, PG_US_U | PG_RW_W | PG_P
    .create_pte:                            ; 设置第一个页表, ebx 为第一个页表的基址
        mov [ebx + esi * 4], edx            ; 第一个页表的中的第一个页表项从 0开始
        add edx, 0x1000                     ; 每个页表项占 4 字节
        inc esi
        loop .create_pte

    mov eax, PDT_ADDR                       ; 设置内核的其他页目录项
    add eax, 0x2000                         ; eax 为第二个页表的位置
    or eax, PG_US_U | PG_RW_W | PG_P
    mov ebx, PDT_ADDR
    mov ecx, 254
    mov esi, 769
    .create_kernel_pde:                     ; 创建内核的页目录项
        mov [ebx + esi * 4], eax
        inc esi
        add eax, 0x1000
        loop .create_kernel_pde
    ret

; ======================================================
; print(char* str, uint_32 line, uint_32 len) 函数, str 表示字符串的起始位置, line 表示行数, len 表示字符串长度
; ______________________________________________________
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
[bits 32]
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







