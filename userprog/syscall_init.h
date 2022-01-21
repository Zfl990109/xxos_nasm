//
// Created by zfl on 2022/1/21.
//

#ifndef B_SYSCALL_INIT_H
#define B_SYSCALL_INIT_H

#include "../lib/stdint.h"
void syscall_init(void);
uint32_t sys_getpid(void);
uint32_t sys_write(char* str);

#endif //B_SYSCALL_INIT_H
