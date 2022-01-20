//
// Created by zfl on 2022/1/15.
//

#include "console.h"
#include "../../libs/print.h"
#include "../../libs/defs.h"
#include "../process/thread.h"
#include "../process/sync.h"
static struct lock console_lock;

void init_console(){
    init_lock(&console_lock);
}

void acquire_console(){
    acquire_lock(&console_lock);
}

void release_console(){
    release_lock(&console_lock);
}

void console_print_str(char* str){
    acquire_console();
    print_str(str);
    release_console();
}

void console_print_char(uint8_t char_ascii){
    acquire_console();
    print_char(char_ascii);
    release_console();
}

void console_print_int(uint32_t num){
    acquire_console();
    print_int(num);
    release_console();
}

