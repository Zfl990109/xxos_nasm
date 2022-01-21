//
// Created by zfl on 2022/1/21.
//

#ifndef B_SYSCALL_H
#define B_SYSCALL_H
#include "../stdint.h"
enum SYSCALL_NR {
    SYS_GETPID,
    SYS_WRITE
};
uint32_t getpid(void);
uint32_t write(char* str);

#endif //B_SYSCALL_H
