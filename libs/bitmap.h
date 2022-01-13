//
// Created by zfl on 2022/1/13.
//

#ifndef XXOS_BITMAP_H
#define XXOS_BITMAP_H
#include "global.h"
#define BITMAP_MASK     1
struct bitmap{
    uint32_t len;
    uint8_t* flags;
};

void init_bitmap(struct bitmap* btmp);
bool bitmap_scan_test(struct bitmap* btmp, uint32_t k);
int bitmap_scan(struct bitmap* btmp, uint32_t cnt);
void bitmap_set(struct bitmap* btmp, uint32_t k, int8_t value);

#endif //XXOS_BITMAP_H
