//
// Created by zfl on 2022/1/13.
//

#include "thread.h"
#include "../../libs/defs.h"
#include "../../libs/string.h"
#include "../../libs/global.h"
#include "../../libs/print.h"
#include "../mm/memory.h"
#include "../debug/assert.h"
#include "../trap/interrupt.h"
#include "../../libs/list.h"

struct task_struct* main_thread;        // 主线程 PCB
struct list thread_ready_list;          // 就绪队列
struct list thread_all_list;            // 所有任务队列
static struct list_elem* thread_tag;    // 用于保存队列中的线程结点

extern void switch_to(struct task_struct* cur_thread, struct task_struct* next);


// 获取当前线程的 PCB 指针
struct task_struct* running_thread(){
    uint32_t esp;
    asm ("mov %%esp, %0": "=g"(esp));
    return (struct task_struct*)(esp & 0xfffff000);
}


// 由 kernel_thread 执行 function(func_arg)
static void kernel_thread(thread_func* function, void* func_arg){
    enable_intr();
    function(func_arg);
}

// 初始化线程栈 thread_stack, 将待执行的函数和参数放到 thread_stack 中的相应位置
void thread_create(struct task_struct* pthread, thread_func function, void* func_arg){
    // 预留中断使用栈的空间
    pthread->self_kernel_stack -= sizeof(struct intr_stack);

    // 留出线程栈空间
    pthread->self_kernel_stack -= sizeof(struct thread_stack);

    struct thread_stack* kernel_thread_stack = (struct thread_stack*)pthread->self_kernel_stack;
    kernel_thread_stack->eip = kernel_thread;
    kernel_thread_stack->function = function;
    kernel_thread_stack->func_arg = func_arg;
    kernel_thread_stack->ebp = kernel_thread_stack->ebx = kernel_thread_stack->edi = kernel_thread_stack->esi = 0;
}

// 初始化线程基本信息
void thread_init(struct task_struct* pthread, char* name, int prio){
    memset(pthread, 0, sizeof(*pthread));
    strcpy(pthread->name, name);
    if (pthread == main_thread){
        pthread->status = TASK_RUNNING;
    } else{
        pthread->status = TASK_READY;
    }
//    pthread->status = TASK_RUNNING;
    pthread->self_kernel_stack = (uint32_t*)((uint32_t)pthread + PG_SIZE);
    pthread->priority = prio;
    pthread->ticks = prio;
    pthread->elapsed_ticks = 0;
    pthread->pgdir = NULL;
    pthread->stack_magic = 0x19990109;

}

struct task_struct* thread_start(char* name, int prio, thread_func function, void* func_arg){
    struct task_struct* thread = apply_kernel_pages(1);
    thread_init(thread, name, prio);
    thread_create(thread, function, func_arg);

    ASSERT(!elem_find(&thread_ready_list, &thread->general_tag));
    list_append(&thread_ready_list, &thread->general_tag);
    ASSERT(!elem_find(&thread_all_list, &thread->all_list_tag));
    list_append(&thread_all_list, &thread->all_list_tag);

//    asm volatile("movl %0,%%esp; pop %%ebp; pop %%ebx; pop %%edi; pop %%esi; ret" : : "g"(thread->self_kernel_stack) :"memory"); //栈顶的位置为 thread->self_kstack

    return thread;
}

void schedule(void){
    ASSERT(intr_get_status() == INTR_OFF);
    struct task_struct* cur = running_thread();
    if (cur->status == TASK_RUNNING){
        ASSERT(!elem_find(&thread_ready_list, &cur->general_tag));
        list_append(&thread_ready_list, &cur->general_tag);
        cur->ticks = cur->priority;
        cur->status = TASK_READY;
    } else{

    }
    ASSERT(!list_empty(&thread_ready_list));
    thread_tag = NULL;
    thread_tag = list_pop(&thread_ready_list);
    struct task_struct* next = elem2entry(struct task_struct, general_tag, thread_tag);
    next->status = TASK_RUNNING;
    switch_to(cur, next);
}

static void make_main_thread(void){
    main_thread = running_thread();
    thread_init(main_thread, "main", 31);
    ASSERT(!elem_find(&thread_all_list, &main_thread->all_list_tag));
    list_append(&thread_all_list, &main_thread->all_list_tag);
}

void init_thread(void){
    print_str("thread_init start ...\n");
    list_init(&thread_ready_list);
    list_init(&thread_all_list);
    make_main_thread();
    print_str("thread_init done!\n");
}