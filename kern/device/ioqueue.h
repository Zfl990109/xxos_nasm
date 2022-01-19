//
// Created by zfl on 2022/1/18.
//

#ifndef XXOS_NASM_IOQUEUE_H
#define XXOS_NASM_IOQUEUE_H

#include "../../libs/defs.h"
#include "../process/thread.h"
#include "../process/sync.h"

#define bufsize         64
// 环形队列
struct ioqueue{
    struct lock lock;
    struct task_struct* producer;
    struct task_struct* consumer;
    char buf[bufsize];
    int32_t head;
    int32_t tail;
};

void init_ioqueue(struct ioqueue* ioq);

static int32_t next_pos(int32_t pos);

bool ioq_full(struct ioqueue* ioq);

bool ioq_empty(struct ioqueue* ioq);

static void ioq_wait(struct task_struct** waiter);

static void wakeup(struct task_struct** waiter);

char ioq_getchar(struct ioqueue* ioq);

void ioq_putchar(struct ioqueue* ioq, char byte);

#endif //XXOS_NASM_IOQUEUE_H
