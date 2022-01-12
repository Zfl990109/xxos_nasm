//
// Created by zfl on 2022/1/8.
//
#include "../../libs/print.h"
#include "../trap/interrupt.h"
#include "../driver/timer.h"

void init_all(void);

void main(void){
    reset_screen();
    print_str("xxos is loading ...\n");
    init_all();
    asm volatile("sti");
    //TODO:分页处理
//    int i = 100000;
//    while (i--){}
//    asm volatile("cli");
    while (1);
}

// 初始化所有模块
void init_all(){
    print_str("init all modules...\n");
    init_intr();
    init_timer();
}


