//
// Created by zfl on 2022/1/13.
//

#ifndef XXOS_STRING_H
#define XXOS_STRING_H
#include "defs.h"
void memset(void* dst_, uint8_t value, uint32_t size);

// 将从 src_ 起始的 size 个字节复制到 dst_
void memcpy(void* dst_, const void* src_, uint32_t size);

// 比较以地址 a_ 和地址 b_ 开头的 size 个字节, 若相等则返回 0, a_ > b_, 则返回 +1, 否则返回 -1
int memcmp(const void* a_, const void* b_, uint32_t size);

// 将字符串从 src_ 复制到 dst_
char* strcpy(char* dst_, const char* src_);

// 返回字符串长度
uint32_t strlen(const char* str);

// 比较字符串
int8_t strcmp(const char* a, const char* b);

// 从左到右查找字符串 str 中首次出现字符 ch 的地址
char* strchr(const char* str, const uint8_t ch);

// 从右向左查找字符串 str 中最后出现字符 ch 的地址
char* strrchr(const char* str, const uint8_t ch);

// 将字符串 src_ 拼接到 dst_ 后, 返回拼接的串地址
char* strcat(char* dst_, const char* src_);

// 统计字符 ch 在字符串 str 中出现的次数
uint32_t strchrs(const char* str, uint8_t ch);

#endif //XXOS_STRING_H
