//
// Created by zfl on 2022/1/11.
//

#ifndef XXOS_IO_H
#define XXOS_IO_H

#include "defs.h"
/************** 机器模式 *****************************
 * b -- 输出寄存器, [a~d]l
 * w -- 输出寄存器模式, [a~d]x
 * HImode -- "Half-Integer" 模式, 表示一个两字节的整数
 * QImode -- "Quarter-Integer" 模式, 表示一个一字节的整数
 **************************************************/
 /* 向端口 port 写入一个字节
  * N 表示 0~255, d 表示 dx 存储端口号
  * %b0 表示 al, %w1 表示 dx
  * */
static inline void outb(uint16_t port, uint8_t data){
    asm volatile ("outb %b0, %w1": : "a"(data), "Nd"(port));
}
/* 从端口 port 读入从一个字节并返回 */
static inline uint8_t inb(uint16_t port){
    uint8_t data;
    asm volatile ("inb %w1, %b0": "=a"(data): "Nd"(port));
    return data;
}
/* 向端口 port 写入从 addr 开始的 count 个字 */
static inline void outsw(uint16_t port, const void* addr, uint32_t count){
    asm volatile ("cld; rep outsw": "+S"(addr), "+c"(count) : "d"(port));
}
/* 从端口 port 读入 count 个字到以 addr 开始的地址中 */
static inline void insw(uint16_t port, void* addr, uint32_t count){
    asm volatile ("cld; rep insw": "+D"(addr), "+c"(count) : "d"(port): "memory");
}

#endif //XXOS_IO_H
