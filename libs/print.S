%include "../boot/asm.inc"

global print_char
global print_str
global reset_screen
section .text
bits 32
; ======================================================
; reset_screen 函数, 清屏并将光标指向左上角
; ______________________________________________________
reset_screen:
    pushad
    cld
    mov edi, 0;              80 * 25 * 2 = 40000
    .clean:
        mov byte [gs:edi], 0
        add edi, 2
        cmp edi, 4000
        jne .clean
    ; 先设置高 8 位
    mov dx, 0x3d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x3d5
    mov al, 0
    out dx, al
    ; 设置低 8 位
    mov dx, 0x3d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x3d5
    mov al, 0
    out dx, al
    popad
    ret
; ======================================================
; print_str 函数, 循环调用 print_char 来打印字符串
; ______________________________________________________
print_str:
    push ebx
    push ecx
    xor ecx, ecx
    mov ebx, [esp +12]
    .goon:
        mov cl, [ebx]
        cmp cl, 0
        jz .str_over
        push ecx
        call print_char
        add esp, 4
        inc ebx
        jmp .goon
    .str_over:
        pop ecx
        pop ebx
        ret
; ======================================================
; 将一个字符写入光标所在处
; ______________________________________________________
print_char:
    pushad
    mov ax, select_video
    mov gs, ax
; ======================================================
; 获取当前光标所在位置
; ______________________________________________________
    ; 先获取高 8 位
    mov dx, 0x3d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x3d5
    in al, dx
    mov ah, al
    ; 获取低 8 位
    mov dx, 0x3d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x3d5
    in al, dx

    ; 将光标存入 bx
    mov bx, ax
    mov byte cl, [esp + 36]     ; pushad 压入了 4 * 8 = 32 字节, 再加上返回地址 4 字节
    cmp cl, 0xd
    je .is_carriage_return
    cmp cl, 0xa
    je .is_line_feed
    cmp cl, 0x8
    je .is_backspace
    jmp .put_other

.is_backspace:
    cmp bx, 0
    je .set_cursor
    dec bx
    shl bx, 1
    mov word [gs:bx], 0x0720
    shr bx, 1
    jmp .set_cursor

.put_other:
    shl bx, 1
    mov [gs:bx], cl
    inc bx
    mov byte [gs:bx], 0x0b
    inc bx
    shr bx, 1
    cmp bx, 2000
    jl .set_cursor

.is_line_feed:
.is_carriage_return:
    xor dx, dx
    mov ax, bx
    mov si, 80
    div si
    sub bx, dx
.is_carriage_return_end:
    add bx, 80
    cmp bx, 2000
.is_line_feed_end:
    jl .set_cursor

.roll_screen:
    cld
    mov ax, select_data
    mov es, ax
    mov di, es
    mov ecx, 920
    mov esi, 0xb80a0
    mov edi, 0xb8000
    rep movsd

    mov ebx, 3840
    mov ecx, 80
    mov esi, 0

.clean_last_row:
    mov word [ebx + esi * 2], 0x0720
    inc esi
    loop .clean_last_row
    mov ebx, 1920
.set_cursor:
    ; 先设置高 8 位
    mov dx, 0x3d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x3d5
    mov al, bh
    out dx, al
    ; 设置低 8 位
    mov dx, 0x3d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x3d5
    mov al, bl
    out dx, al
.print_char_done:
    popad
    ret





