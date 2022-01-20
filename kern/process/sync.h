//
// Created by zfl on 2022/1/15.
//

#ifndef XXOS_NASM_SYNC_H
#define XXOS_NASM_SYNC_H

#include "../../libs/list.h"
#include "../../libs/stdint.h"
#include "../mm/memory.h"

/* 信号量结构 */
struct semaphore {
    uint8_t  value;
    struct   list waiters;
};

/* 锁结构 */
struct lock {
    struct   task_struct* holder;	    // 锁的持有者
    struct   semaphore semaphore;	    // 用二元信号量实现锁
    uint32_t holder_repeat_nr;		    // 锁的持有者重复申请锁的次数
};

void sema_init(struct semaphore* psema, uint8_t value);
void sema_down(struct semaphore* psema);
void sema_up(struct semaphore* psema);
void lock_init(struct lock* plock);
void lock_acquire(struct lock* plock);
void lock_release(struct lock* plock);
#endif //XXOS_NASM_SYNC_H
