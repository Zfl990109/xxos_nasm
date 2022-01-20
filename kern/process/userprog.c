//
// Created by zfl on 2022/1/19.
//

#include "userprog.h"
#include "thread.h"
#include "tss.h"
#include "../debug/assert.h"
#include "../device/console.h"
#include "../../libs/string.h"
#include "../../libs/print.h"
#include "../trap/interrupt.h"

extern void intr_exit(void);

// 创建用户进程初始上下文信息
void start_process(void* filename_){
    void* function = filename_;
    struct task_struct* cur = running_thread();
    cur->self_kernel_stack += sizeof(struct thread_stack);
    struct intr_stack* proc_stack = (struct intr_stack*)cur->self_kernel_stack;
    proc_stack->edi = proc_stack->esi = proc_stack->ebp = proc_stack->esp_dummy = 0;
    proc_stack->ebx = proc_stack->edx = proc_stack->ecx = proc_stack->eax = 0;
    proc_stack->gs = 0;
    proc_stack->ds = proc_stack->es = proc_stack->fs = USER_DATA;
    proc_stack->eip = function;
    proc_stack->cs = USER_CODE;
    proc_stack->eflags = (EFLAGS_IOPL_0 | EFLAGS_MBS | EFLAGS_IF_1);
    proc_stack->esp = (void*)((uint32_t)bind_page_vaddr(PF_USER, USER_STACK3_VADDR) + PG_SIZE);
    proc_stack->ss = USER_DATA;
    asm volatile("movl %0, %%esp; jmp intr_exit": : "g"(proc_stack) : "memory");
}

// 激活页表
void page_dir_activate(struct task_struct* pthread){
    uint32_t pagedir_phyaddr = 0x100000;
    if (pthread->pgdir != NULL){
        pagedir_phyaddr = vaddr2phyaddr((uint32_t)pthread->pgdir);
    }
    asm volatile("movl %0, %%cr3": : "r"(pagedir_phyaddr) : "memory");
}

// 激活线程或进程的页表, 更新 tss 中的 esp0 为进程的特权级为 0 的栈
void process_activate(struct task_struct* pthread){
    ASSERT(pthread != NULL);
    page_dir_activate(pthread);
    if (pthread->pgdir){
        update_tss_esp(pthread);
    }
}

// 创建页目录表, 将当前页表的表示内核空间的 pde 复制, 成功则返回页目录的虚拟地址
uint32_t* create_page_dir(void){
    uint32_t* page_dir_vaddr = apply_kernel_pages(1);
    if (page_dir_vaddr == NULL){
        console_print_str("create_page_dir: apply_kernel_pages failed!");
        return NULL;
    }
    // 复制页表
    memcpy((uint32_t*)((uint32_t)page_dir_vaddr + 0x300 * 4), (uint32_t*)(0xfffff000 + 0x300 * 4), 1024);
    // 更新页目录地址
    uint32_t new_page_dir_phyaddr = vaddr2phyaddr((uint32_t)page_dir_vaddr);
    page_dir_vaddr[1023] = new_page_dir_phyaddr | PG_US_U | PG_RW_W | PG_P_1;
    return page_dir_vaddr;
}

// 创建用户进程虚拟地址位图
void create_user_vaddr_bitmap(struct task_struct* userprog){
    userprog->userprog_vaddr.vaddr_start = USER_VADDR_START;
    uint32_t bitmap_page_count = DIV_ROUND_UP((0xc0000000 - USER_VADDR_START) / PG_SIZE / 8, PG_SIZE);
    userprog->userprog_vaddr.virtual_bitmap.flags = apply_kernel_pages(bitmap_page_count);
    userprog->userprog_vaddr.virtual_bitmap.len = (0xc0000000 - USER_VADDR_START) / PG_SIZE / 8;
    init_bitmap(&userprog->userprog_vaddr.virtual_bitmap);
}

// 创建用户进程
void process_execute(void* filename, char* name){
    struct task_struct* thread = apply_kernel_pages(1);
    thread_init(thread, name, default_prio);
    print_int(thread->priority);
    print_str("\n");
    create_user_vaddr_bitmap(thread);
    thread_create(thread, start_process, filename);

    thread->pgdir = create_page_dir();

    enum intr_status old_status = disable_intr();
    ASSERT(!elem_find(&thread_ready_list, &thread->general_tag));
    list_append(&thread_ready_list, &thread->general_tag);

    ASSERT(!elem_find(&thread_all_list, &thread->all_list_tag));
    list_append(&thread_all_list, &thread->all_list_tag);
    set_intr_status(old_status);
}