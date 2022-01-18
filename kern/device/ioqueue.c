//
// Created by zfl on 2022/1/18.
//

#include "ioqueue.h"
#include "../trap/interrupt.h"
#include "../debug/assert.h"
#include "../../libs/global.h"
#include "../../libs/defs.h"


void init_ioqueue(struct ioqueue* ioq){
    init_lock(&ioq->lock);
    ioq->producer = ioq->consumer = NULL;
    ioq->head = ioq->tail = 0;
}

static int32_t next_pos(int32_t pos){
    return (pos + 1) & bufsize;
}

bool ioq_full(struct ioqueue* ioq){
    ASSERT(intr_get_status() == INTR_OFF);
    return next_pos(ioq->head) == ioq->tail;
}

static bool ioq_empty(struct ioqueue* ioq){
    ASSERT(intr_get_status() == INTR_OFF);
    return ioq->head == ioq->tail;
}

static void ioq_wait(struct task_struct** waiter){
    ASSERT(*waiter == NULL && waiter != NULL);
    *waiter = running_thread();
    thread_block(TASK_BLOCKED);
}

static void wakeup(struct task_struct** waiter){
    ASSERT(*waiter != NULL);
    thread_unblock(*waiter);
    *waiter = NULL;
}

char ioq_getchar(struct ioqueue* ioq){
    ASSERT(intr_get_status() == INTR_OFF);
    while (ioq_empty(ioq)){
        acquire_lock(&ioq->lock);
        ioq_wait(&ioq->consumer);
        release_lock(&ioq->lock);
    }
    char byte = ioq->buf[ioq->tail];
    ioq->tail = next_pos(ioq->tail);
    if (ioq->producer != NULL){
        wakeup(&ioq->producer);
    }
    return byte;
}

void ioq_putchar(struct ioqueue* ioq, char byte){
    ASSERT(intr_get_status() == INTR_OFF);
    while (ioq_full(ioq)){
        acquire_lock(&ioq->lock);
        ioq_wait(&ioq->producer);
        release_lock(&ioq->lock);
    }
    ioq->buf[ioq->head] = byte;
    ioq->head = next_pos(ioq->head);
    if (ioq->consumer != NULL){
        wakeup(&ioq->consumer);
    }

}
