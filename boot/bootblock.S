%include "asm.inc"
org 0x7c00
global print
global read_mem
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
; 物理内存探测(0x820)
; ______________________________________________________
probe_memory:
    mov dword [ards_nr], 0
    xor ebx, ebx
    mov word di, ards_buf
start_probe:
    mov eax, 0xe820
    mov ecx, 20
    mov edx, SMAP
    int 0x15
    jnc cont
    mov word [ards_nr], 12345
    jmp finish_probe
cont:
    add word di, 20
    inc dword [ards_nr]
    cmp ebx, 0
    jnz start_probe
finish_probe:
    mov cx, [ards_nr]
    mov ebx, ards_buf
    xor edx, edx
    .find_max_mem_area:
        mov eax, [ebx]
        add eax, [ebx + 8]
        add ebx, 20
        cmp edx, eax
        jge .next_ards
        mov edx, eax
    .next_ards:
        loop .find_max_mem_area
        mov [ards_nr], edx;        内存中 0x500 处存放了总共的内存大小 0x2000000, 换算过来是 32M

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
    mov ax, select_data
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, LOADER_ADDR

    mov eax, msg
    mov ebx, 0x0
    mov ecx, msg_len
    call print
    mov eax, ll
    mov ebx, 0x1
    mov ecx, ll_len
    call print
    mov eax, 2
    mov ebx, LOADER_ADDR
    mov ecx, 1
    call read_mem

    jmp select_code:LOADER_ADDR

    msg: db "boot sector"
    msg_len equ ($-msg)
    ll: db "boot main"
    ll_len equ ($-ll)

; GDT
    gdt:    set_null                0,              0,          0
    code:   set_segment             0,     0xffffffff,          STA_X | STA_R
    data:   set_segment             0,     0xffffffff,          STA_W
    video:  set_segment       0xb8000,         0xffff,          STA_W

    gdt_ptr     dw  0x1f
                dd  gdt

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



    times 510-($-$$) db 0
    dw 0xaa55

