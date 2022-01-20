//
// Created by zfl on 2022/1/19.
//

#ifndef XXOS_NASM_TSS_H
#define XXOS_NASM_TSS_H

#include "../../libs/stdint.h"
#include "thread.h"
void update_tss_esp(struct task_struct* pthread);
void tss_init(void);

#endif //XXOS_NASM_TSS_H
