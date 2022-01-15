//
// Created by zfl on 2022/1/9.
//

#ifndef XXOS_DEFS_H
#define XXOS_DEFS_H

typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int int64_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned  int uint32_t;
typedef unsigned long long int uint64_t;

typedef int32_t intptr_t;
typedef uint32_t uintptr_t;

typedef uint32_t bool;
#define NULL 0
#define true 1
#define false 0

// 用与内存对象大小
typedef uintptr_t size_t;

// 用于页面数量
typedef size_t ppn_t;

// 将 a 向下调整为 n = 2 ^ k
#define ROUNDDOWN(a, n) ({                                          \
            size_t __a = (size_t)(a);                               \
            (typeof(a))(__a - __a % (n));                           \
        })

// 将 a 向上调整为 n = 2 ^ k
#define ROUNDUP(a, n) ({                                            \
            size_t __n = (size_t)(n);                               \
            (typeof(a))(ROUNDDOWN((size_t)(a) + __n - 1, __n));     \
        })

// 返回某个数据结构中的某个成员变量在该数据结构中的偏移量
#define offsetof(type, member)                                      \
    ((size_t)(&((type *)0)->member))

// 将某个指针转化为某个数据结构
#define to_struct(ptr, type, member)                               \
    ((type *)((char *)(ptr) - offsetof(type, member)))


#endif //XXOS_DEFS_H
