//
// Created by zfl on 2022/1/19.
//

#ifndef XXOS_NASM_USERPROG_H
#define XXOS_NASM_USERPROG_H

#include "../../libs/global.h"
#include "thread.h"

#define default_prio 31
#define USER_STACK3_VADDR  (0xc0000000 - 0x1000)
#define USER_VADDR_START 0x8048000
void process_execute(void* filename, char* name);
void start_process(void* filename_);
void process_activate(struct task_struct* p_thread);
void page_dir_activate(struct task_struct* p_thread);
uint32_t* create_page_dir(void);
void create_user_vaddr_bitmap(struct task_struct* user_prog);
#endif //XXOS_NASM_USERPROG_H
