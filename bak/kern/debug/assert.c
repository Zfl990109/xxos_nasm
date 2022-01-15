//
// Created by zfl on 2022/1/13.
//

#include "assert.h"
#include "../../libs/print.h"
#include "../trap/interrupt.h"

void panic_spin(char* filename, int line, const char* func, const char*condition){
    disable_intr();
    print_str("\n\n\n***************** error ********************\n");
    print_str("filename: ");print_str(filename);print_str("\n");
    print_str("line: 0x");print_int(line);print_str("\n");
    print_str("function: ");print_str((char *)func);print_str("\n");
    print_str("condition: ");print_str((char*)condition);print_str("\n");
    print_str("***************** error ********************\n");
    while (1);
}
