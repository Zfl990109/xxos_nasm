//
// Created by zfl on 2022/1/9.
//

#ifndef XXOS_PRINT_H
#define XXOS_PRINT_H

#include "stdint.h"
void put_char(uint8_t char_asci);
void put_str(char* message);
void put_int(uint32_t num);	 // 以16进制打印
void set_cursor(uint32_t cursor_pos);

#endif //XXOS_PRINT_H
