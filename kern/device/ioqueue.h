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
};







#endif //XXOS_NASM_IOQUEUE_H
