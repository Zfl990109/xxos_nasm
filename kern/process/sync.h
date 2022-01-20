//
// Created by zfl on 2022/1/15.
//

#ifndef XXOS_NASM_SYNC_H
#define XXOS_NASM_SYNC_H

#include "../../libs/list.h"
#include "../../libs/defs.h"
#include "../mm/memory.h"
//#include "thread.h"

struct semaphore{
    uint8_t value;
    struct list waiters;
};

struct lock{
    struct task_struct* holder;
    struct semaphore semaphore;
    uint32_t holder_repeat_nr;
};

void init_sema(struct semaphore* psema, uint8_t value);

void init_lock(struct lock* plock);

void wait(struct semaphore* psema);

void signal(struct semaphore* psema);

void acquire_lock(struct lock* plock);

void release_lock(struct lock* plock);

#endif //XXOS_NASM_SYNC_H
