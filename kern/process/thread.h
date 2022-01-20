//
// Created by zfl on 2022/1/13.
//

#ifndef XXOS_THREAD_H
#define XXOS_THREAD_H

#include "../../libs/defs.h"
#include "../../libs/list.h"
#include "../../libs/bitmap.h"
#include "../mm/memory.h"

#define default_prio 31

typedef void thread_func(void*);
extern struct task_struct* main_thread;        // 主线程 PCB
extern struct list thread_ready_list;          // 就绪队列
extern struct list thread_all_list;            // 所有任务队列

enum task_status{
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_WAITING,
    TASK_HANGING,
    TASK_DIED
};

struct intr_stack{
    uint32_t vector_no;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    // 当 cpu 从低特权级进入高特权级时压入以下信息
    uint32_t error_code;
    void (*eip) (void);
    uint32_t cs;
    uint32_t eflags;
    void* esp;
    uint32_t ss;
};

struct thread_stack{
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edi;
    uint32_t esi;

    // 线程第一次执行时, eip 指向待调用的函数 kernel_thread
    // 其余时候, eip 指向 switch_to 的返回地址
    void (*eip) (thread_func* func, void* func_arg);

    // 以下信息仅供第一次被调度上 cpu 时使用
    void (*unused_retaddr);         // 起到占位作用
    thread_func* function;
    void* func_arg;
};

struct task_struct{
    uint32_t* self_kernel_stack;
    enum task_status status;
    char name[16];
    uint8_t priority;
    uint8_t ticks;               // 处理器上执行的时间数

    uint32_t elapsed_ticks;      // 此任务已经执行的时间数
    struct list_elem general_tag;   // 在线程队列中的结点
    struct list_elem all_list_tag;   // 在线程队列中的所有

    uint32_t* pgdir;            // 进程页表的虚拟地址
    struct virtual_addr userprog_vaddr;     // 用户进程的虚拟地址
    uint32_t stack_magic;        // 栈的边界标记, 用于检测栈的溢出
};


// 获取当前线程的 PCB 指针
struct task_struct* running_thread();


// 由 kernel_thread 执行 function(func_arg)
static void kernel_thread(thread_func* function, void* func_arg);

// 初始化线程栈 thread_stack, 将待执行的函数和参数放到 thread_stack 中的相应位置
void thread_create(struct task_struct* pthread, thread_func function, void* func_arg);

// 初始化线程基本信息
void thread_init(struct task_struct* pthread, char* name, int prio);

struct task_struct* thread_start(char* name, int prio, thread_func function, void* func_arg);

void thread_block(enum task_status status);

void thread_unblock(struct task_struct* pthread);

void schedule(void);

static void make_main_thread(void);

void init_thread(void);

#endif //XXOS_THREAD_H
