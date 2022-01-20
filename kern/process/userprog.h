//
// Created by zfl on 2022/1/19.
//

#ifndef XXOS_NASM_USERPROG_H
#define XXOS_NASM_USERPROG_H

#include "../../libs/global.h"
#include "thread.h"
#define USER_STACK3_VADDR           (0xc0000000 - 0x1000)
#define USER_VADDR_START             0x8048000

//// 创建用户进程初始上下文信息
//void start_process(void* filename_);
//
//// 激活页表
//void page_dir_activate(struct task_struct* p_thread);
//
//// 激活线程或进程的页表, 更新 tss 中的 esp0 为进程的特权级为 0 的栈
//void process_activate(struct task_struct* p_thread);
//
//// 创建页目录表, 将当前页表的表示内核空间的 pde 复制, 成功则返回页目录的虚拟地址
//uint32_t* create_page_dir(void);
//
//// 创建用户进程虚拟地址位图
//void create_user_vaddr_bitmap(struct task_struct* user_prog);
//
//// 创建用户进程
//void process_execute(void* filename,char* name);

void process_execute(void* filename, char* name);
void start_process(void* filename_);
void process_activate(struct task_struct* p_thread);
void page_dir_activate(struct task_struct* p_thread);
uint32_t* create_page_dir(void);
void create_user_vaddr_bitmap(struct task_struct* user_prog);

#endif //XXOS_NASM_USERPROG_H
