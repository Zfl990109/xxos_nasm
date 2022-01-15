//
// Created by zfl on 2022/1/13.
//

#ifndef XXOS_ASSERT_H
#define XXOS_ASSERT_H

// 打印文件名、行号、函数名、条件并使程序悬停
void panic_spin(char* filename, int line, const char* func, const char*condition);

#define PANIC(...) panic_spin (__FILE__, __LINE__, __func__, __VA_ARGS__)

#ifdef NDEBUG
    #define ASSERT(CONDITION) ((void)0)
#else
#define ASSERT(CONDITION) \
    if(CONDITION){}       \
    else{ PANIC(#CONDITION); }
#endif // NDEBUG
#endif //XXOS_ASSERT_H
