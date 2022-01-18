//
// Created by zfl on 2022/1/15.
//

#ifndef XXOS_NASM_CONSOLE_H
#define XXOS_NASM_CONSOLE_H

#include "../../libs/defs.h"

void init_console();

void acquire_console();

void release_console();

void console_print_str(char* str);

void console_print_char(uint8_t char_ascii);

void console_print_int(uint32_t num);


#endif //XXOS_NASM_CONSOLE_H
