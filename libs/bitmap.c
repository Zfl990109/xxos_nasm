//
// Created by zfl on 2022/1/13.
//

#include "bitmap.h"
#include "string.h"
#include "print.h"
#include "../kern/trap/interrupt.h"
#include "../kern/debug/assert.h"

void init_bitmap(struct bitmap* btmp){
    memset(btmp->flags, 0, btmp->len);
}

// 测试第 k 个元素是否存在
bool bitmap_scan_test(struct bitmap* btmp, uint32_t k){
    uint32_t i = k >> 3;
    uint32_t j = k & 0x7;
    return (btmp->flags[i] & (BITMAP_MASK << j));
}

// 在 bitmap 中申请连续 cnt 个位
int bitmap_scan(struct bitmap* btmp, uint32_t cnt){
    uint32_t index = 0;
    while ((0xff == btmp->flags[index]) && (index < btmp->len)){
        index++;
    }
    ASSERT(index < btmp->len);
    if (index == btmp->len)
        return -1;
    int i = 0;
    while ((uint8_t)(BITMAP_MASK << i) & btmp->flags[index]){
        i++;
    }
    int k = (index << 3) + i;
    if (cnt == 1)
        return k;
    uint32_t remain = (btmp->len << 3) - k;
    uint32_t next = k + 1;
    uint32_t count = 1;
    k = -1;
    while (remain-- > 0){
        if (!bitmap_scan_test(btmp, next))
            count++;
        else
            count = 0;
        if (count == cnt){
            k = next -cnt + 1;
            break;
        }
        next++;
    }
    return k;
}

void bitmap_set(struct bitmap* btmp, uint32_t k, int8_t value){
    ASSERT(value == 0 || value == 1);
    uint32_t i = k >> 3;
    uint32_t j = k & 0x7;
    if (value)
        btmp->flags[i] |= (BITMAP_MASK << j);
    else
        btmp->flags[i] &= ~(BITMAP_MASK << j);
}