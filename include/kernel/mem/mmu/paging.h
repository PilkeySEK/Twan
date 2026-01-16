#ifndef _PAGING_H_
#define _PAGING_H_

#include <include/kernel/kernel.h>
#include <include/lib/dsa/bmp512.h>
#include <include/subsys/sync/mcslock.h>

struct mapper
{
    struct bmp512 bmp;
    struct mcslock_isr lock;
};

struct keyhole 
{
    struct mcslock_isr lock;
};

typedef enum 
{
    PGTABLE_NONE,
    PML4,
    PDPT,
    PD,
    PT
} pgtable_t;

typedef enum 
{
    PGTABLE_LEVEL_NONE,
    PGTABLE_LEVEL_4K,
    PGTABLE_LEVEL_2MB,
    PGTABLE_LEVEL_1GB,
} pgtable_level_t;

#define virt_to_phys_static(addr) ((u64)addr)

void *__pt_walk(va_t va, int *level);
void *pt_walk(va_t va, int *level);

u64 __virt_to_phys(va_t va);
u64 virt_to_phys(void *addr);

void init_mappers(void);
void init_heap(void);

/* mapping specific pfns */

void *__map_pfn_noflush(u64 pfn);
void *__map_pfn_local(u64 pfn);

void __unmap_pfn_noflush(void *addr);
void __unmap_pfn_local(void *addr);

void *map_pfn(u64 pfn);
void unmap_pfn(void *addr);

void *__map_phys_pg_noflush(u64 phys);
void *__map_phys_pg_local(u64 phys);

void __unmap_phys_pg_noflush(void *addr);
void __unmap_phys_pg_local(void *addr);

void *map_phys_pg(u64 phys);
void unmap_phys_pg(void *addr);

/* mapping io memory */

void *__map_pfn_io_noflush(u64 pfn);
void *__map_pfn_io_local(u64 pfn);

void __unmap_pfn_io_noflush(void *addr);
void __unmap_pfn_io_local(void *addr);

void __remap_pfn_noflush(void *addr, u32 pfn);
void __remap_pfn_local(void *addr, u32 pfn);

void *map_pfn_io(u64 pfn);
void unmap_pfn_io(void *addr);
void remap_pfn_local(void *addr, u32 pfn);
void remap_pfn(void *addr, u32 pfn);

void *__map_phys_pg_io_noflush(u64 phys);
void *__map_phys_pg_io_local(u64 phys);

void __unmap_phys_pg_io_noflush(void *addr);
void __unmap_phys_pg_io_local(void *addr);

void *map_phys_pg_io(u64 phys);
void unmap_phys_pg_io(void *addr);

/* keyholes */

void __remap_keyhole_local(void *addr, u32 pfn);

#endif