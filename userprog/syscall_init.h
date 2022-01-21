//
// Created by zfl on 2022/1/21.
//

#ifndef B_SYSCALL_INIT_H
#define B_SYSCALL_INIT_H
// 返回当前任务的 pid
uint32_t sys_getpid(void);

// 初始化系统调用
void syscall_init(void);

#endif //B_SYSCALL_INIT_H
