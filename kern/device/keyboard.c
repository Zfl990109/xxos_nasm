//
// Created by zfl on 2022/1/18.
//

#include "keyboard.h"
#include "ioqueue.h"
#include "../../libs/print.h"
#include "../trap/interrupt.h"
#include "../../libs/io.h"
#include "../../libs/global.h"

struct ioqueue kbd_buf;

static void intr_keyboard_func(void){

    // 判断这次中断发生之前的上一次中断, 以下任意三个按键是否有按下
    bool ctrl_down_last = ctrl_status;
    bool shift_down_last = shift_status;
    bool caps_lock_last = caps_lock_status;
    bool break_code;
    uint16_t scancode = inb(KBD_BUF_PORT);
    if (scancode == 0xe0){
        ext_scancode = true;
        return;
    }
    if (ext_scancode){
        scancode = ((0xe000) | scancode);
        ext_scancode = false;
    }
    break_code = ((scancode & 0x0080) != 0);
    if (break_code){
        uint16_t make_code = (scancode &= 0xff7f);
        if (make_code == ctrl_l_make || make_code == ctrl_r_make){
            ctrl_status = false;
        }else if (make_code == shift_l_make || make_code == shift_r_make){
            shift_status = false;
        }else if (make_code == alt_l_make || make_code == alt_r_make){
            alt_status = false;
        }
        return;
    }else if((scancode > 0x00 && scancode < 0x3b) || (scancode == alt_r_make) || (scancode == ctrl_r_make)){
        bool shift = false;
        if ((scancode < 0x0e) || (scancode == 0x29) || (scancode == 0x1a) || (scancode == 0x1b) ||
            (scancode == 0x2b) || (scancode == 0x27) || (scancode == 0x28) || (scancode == 0x33) ||
            (scancode == 0x34) || (scancode == 0x35)){
            if (shift_down_last) shift = true;
        } else{
            if (shift_down_last && caps_lock_last){
                shift = false;
            }else if(shift_down_last || caps_lock_last){
                shift = true;
            } else{
                shift = false;
            }
        }
        uint8_t index = (scancode &= 0x00ff);
        char cur_char = keymap[index][shift];
        if (cur_char){
//            if (!ioq_full(&kbd_buf)){
                print_char(cur_char);
//                ioq_putchar(&kbd_buf, cur_char);
//            }
            return;
        }

        if (scancode == ctrl_l_make || scancode == ctrl_r_make){
            ctrl_status = true;
        }else if (scancode == shift_l_make || scancode == shift_r_make){
            shift_status = true;
        }else if (scancode == alt_l_make || scancode == alt_r_make){
            alt_status = true;
        }else if (scancode == caps_lock_make){
            caps_lock_status = !caps_lock_status;
        }
    } else{
        print_str("unknown key\n");
    }
    return;
}

void init_keyboard(){
    print_str("keyboard init start ...\n");
    init_ioqueue(&kbd_buf);
    register_intr_func(0x21, intr_keyboard_func);
    print_str("keyboard init done!\n");
}