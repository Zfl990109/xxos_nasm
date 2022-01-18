//
// Created by zfl on 2022/1/15.
//
#include "thread.h"
#include "../trap/interrupt.h"
#include "../debug/assert.h"
#include "sync.h"

void init_sema(struct semaphore* psema, uint8_t value){
    psema->value = value;
    list_init(&psema->waiters);
}

void init_lock(struct lock* plock){
    plock->holder = NULL;
    plock->holder_repeat_nr = 0;
    init_sema(&plock->semaphore, 1);
}

// 二元信号量的 wait() 操作
void wait(struct semaphore* psema){
    enum intr_status old_status = disable_intr();
    while (psema->value == 0){
        ASSERT(!elem_find(&psema->waiters, &running_thread()->general_tag));
        if (elem_find(&psema->waiters, &running_thread()->general_tag))
            PANIC("wait: thread has been blocked in waiters list\n");
        list_append(&psema->waiters, &running_thread()->general_tag);
        thread_block(TASK_BLOCKED);
    }
    psema->value--;
    ASSERT(psema->value == 0);
    set_intr_status(old_status);
}

// 二元信号量的 signal() 操作
void signal(struct semaphore* psema){
    enum intr_status old_status = disable_intr();
    ASSERT(psema->value == 0);
    if (!list_empty(&psema->waiters)){
        struct task_struct* thread_blocked = elem2entry(struct task_struct, general_tag, list_pop(&psema->waiters));
        thread_unblock(thread_blocked);
    }
    psema->value++;
    ASSERT(psema->value == 1);
    set_intr_status(old_status);
}

void acquire_lock(struct lock* plock){
    if (plock->holder != running_thread()){
        wait(&plock->semaphore);
        plock->holder = running_thread();
        ASSERT(plock->holder_repeat_nr == 0);
        plock->holder_repeat_nr = 1;
    }else{
        plock->holder_repeat_nr++;
    }
}

void release_lock(struct lock* plock){
    ASSERT(plock->holder == running_thread());
    if (plock->holder_repeat_nr > 1){
        plock->holder_repeat_nr--;
        return;
    }
    ASSERT(plock->holder_repeat_nr == 1);
    plock->holder = NULL;
    plock->holder_repeat_nr = 0;
    signal(&plock->semaphore);
}