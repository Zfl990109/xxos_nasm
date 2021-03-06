//
// Created by zfl on 2022/1/13.
//

#include "memory.h"
#include "../../libs/print.h"
#include "../debug/assert.h"
#include "../../libs/string.h"

#define PDE_IDX(addr) ((addr & 0xffc00000) >> 22)
#define PTE_IDX(addr) ((addr & 0x003ff000) >> 12)

struct pool kernel_pool, user_pool;
struct virtual_addr kernel_vaddr;

static void mem_pool_init(uint32_t all_mem){
    print_str("    mem_pool init start ...\n");
    uint32_t page_table_size = PG_SIZE * 256;
    uint32_t used_mem = page_table_size + 0x100000;
    uint32_t free_mem = all_mem - used_mem;
    uint16_t all_free_pages = free_mem / PG_SIZE;
    uint16_t kernel_free_pages = all_free_pages / 2;
    uint16_t user_free_pages = all_free_pages - kernel_free_pages;
    uint32_t kernel_bitmap_len = kernel_free_pages / 8;
    uint32_t user_bitmap_len = user_free_pages / 8;
    uint32_t kernel_pool_addr = used_mem;
    uint32_t user_pool_addr = kernel_pool_addr + kernel_free_pages * PG_SIZE;

    kernel_pool.phy_addr = kernel_pool_addr;
    kernel_pool.size = kernel_free_pages * PG_SIZE;
    kernel_pool.pool_bitmap.len = kernel_bitmap_len;
    kernel_pool.pool_bitmap.flags = (void*) MEM_BITMAP_ADDR;

    user_pool.phy_addr = user_pool_addr;
    user_pool.size = user_free_pages * PG_SIZE;
    user_pool.pool_bitmap.len = user_bitmap_len;
    user_pool.pool_bitmap.flags = (void*)(MEM_BITMAP_ADDR + kernel_bitmap_len);

    print_str("        kernel_pool_bitmap_start: ");
    print_int((int)kernel_pool.pool_bitmap.flags);
    print_str(" kernel_pool phy_addr start: ");
    print_int(kernel_pool.phy_addr);
    print_str("\n");

    print_str("        user_pool_bitmap_start: ");
    print_int((int)user_pool.pool_bitmap.flags);
    print_str(" user_pool phy_addr start: ");
    print_int(user_pool.phy_addr);
    print_str("\n");

    init_bitmap(&kernel_pool.pool_bitmap);
    init_bitmap(&user_pool.pool_bitmap);

    kernel_vaddr.virtual_bitmap.len = kernel_bitmap_len;
    kernel_vaddr.virtual_bitmap.flags = (void*)(MEM_BITMAP_ADDR + kernel_bitmap_len + user_bitmap_len);
    kernel_vaddr.vaddr_start = KERNEL_HEAP_ADDR;
    init_bitmap(&kernel_vaddr.virtual_bitmap);
    print_str("    mem_pool_init done!\n");
}

// ??????????????????????????? pg_count ????????????, ?????????????????????????????????
static void* apply_virtual_page(enum pool_flags pf, uint32_t pg_count){
    int vaddr_start = 0, bit_idx = -1;
    uint32_t count = 0;
    if (pf == PF_KERNEL){
        bit_idx = bitmap_scan(&kernel_vaddr.virtual_bitmap, pg_count);
        if (bit_idx == -1){
            return NULL;
        }
        while (count < pg_count){
            bitmap_set(&kernel_vaddr.virtual_bitmap, bit_idx + count++, 1);
        }
        vaddr_start = kernel_vaddr.vaddr_start + bit_idx * PG_SIZE;
    }else{
        //TODO: ???????????????
    }
    return (void*) vaddr_start;
}

// ?????????????????? vaddr ????????? pte ??????, 0xffc00000 ??????????????????????????????
uint32_t* pte_ptr(uint32_t vaddr){
    uint32_t* pte = (uint32_t*)(0xffc00000 + ((vaddr & 0xffc00000) >> 10) + PTE_IDX(vaddr) * 4);
    return pte;
}

// ?????????????????? vaddr ????????? pde ??????, 0xfffff000 ??????????????????????????????
uint32_t* pde_ptr(uint32_t vaddr){
    uint32_t* pde = (uint32_t*)((0xfffff000) +  PDE_IDX(vaddr) * 4);
    return pde;
}

// ??? m_pool ????????????????????????????????? 1 ???, ?????????????????????
static void* palloc(struct pool* m_pool){
    int bit_idx = bitmap_scan(&m_pool->pool_bitmap, 1);         //?????????????????????????????????????????????
    if (bit_idx == -1)
        return NULL;
    bitmap_set(&m_pool->pool_bitmap, bit_idx, 1);
    uint32_t page_phyaddr = (m_pool->phy_addr + (bit_idx * PG_SIZE));
    return (void*) page_phyaddr;
}

// ?????????????????????????????? vaddr ??? ???????????? page_phyaddr ?????????
static void page_table_add(void* _vaddr, void* _page_phyaddr){
    uint32_t vaddr = (uint32_t) _vaddr, page_phyaddr = (uint32_t) _page_phyaddr;
    uint32_t* pde = pde_ptr(vaddr);
    uint32_t* pte = pte_ptr(vaddr);
    if (*pde & 0x00000001){
        ASSERT(!(*pte & 0x00000001));
        if (!(*pte & 0x00000001)){
            *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
        } else{
            PANIC("pte repeat!");
            *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
        }
    } else{
        uint32_t pde_phyaddr = (uint32_t) palloc(&kernel_pool);
        *pde = (pde_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
        memset((void*)((int)pte & 0xfffff000), 0, PG_SIZE);
        ASSERT(!(*pte & 0x00000001));
        *pte = (page_phyaddr | PG_US_U | PG_RW_W | PG_P_1);
    }
}

// ?????? pg_count ????????????, ?????????????????????????????????
void* malloc_page(enum pool_flags pf, uint32_t pg_count){
    ASSERT(pg_count > 0 && pg_count < 3840);
    void* vaddr_start = apply_virtual_page(pf, pg_count);
    if (vaddr_start == NULL)
        return NULL;
    uint32_t vaddr = (uint32_t)vaddr_start, count = pg_count;
    struct pool* mem_pool = pf & PF_KERNEL ? &kernel_pool : &user_pool;
    while (count-- > 0){
        void* page_phyaddr = palloc(mem_pool);
        if (page_phyaddr == NULL)
            return NULL;
        page_table_add((void*)vaddr, page_phyaddr);
        vaddr += PG_SIZE;
    }
    return vaddr_start;
}

// ??????????????????????????? pg_count ???????????????, ?????????????????????
void* apply_kernel_pages(uint32_t pg_count){
    void* vaddr = malloc_page(PF_KERNEL, pg_count);
    if (vaddr != NULL)
        memset(vaddr, 0, pg_count * PG_SIZE);
    return vaddr;
}

void init_mem(){
    print_str("mem_init start ...\n");
    uint32_t total_mem = (*(uint32_t*)(0x500));
    mem_pool_init(total_mem);
    print_str("mem_init done!\n");
}

