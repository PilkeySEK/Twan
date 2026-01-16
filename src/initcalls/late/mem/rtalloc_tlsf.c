#include <include/initcalls/late_initcalls_conf.h>
#if LATE_MEM_RTALLOC_TLSF

#include <include/kernel/kapi.h>
#include <include/subsys/mem/rtalloc.h>
#include <include/lib/tlsf_alloc.h>

/* TODO: alter tlsf library and this to use more fine grained locks, global lock
   on the allocator wouldn't scale very well */

static struct tlsf tlsf_global;

static void *rtalloc_tlsf(size_t size)
{
    struct mcsnode node = INITIALIZE_MCSNODE();

    tlsf_lock(&tlsf_global, &node);
    void *addr = __tlsf_alloc(&tlsf_global, size);
    tlsf_unlock(&tlsf_global, &node);

    return addr;
}

static void rtfree_tlsf(void *addr)
{
    struct mcsnode node = INITIALIZE_MCSNODE();

    tlsf_lock(&tlsf_global, &node);
    __tlsf_free(&tlsf_global, addr);
    tlsf_unlock(&tlsf_global, &node);
}

static void *rtrealloc_tlsf(void *addr, size_t size)
{
    struct mcsnode node = INITIALIZE_MCSNODE();

    tlsf_lock(&tlsf_global, &node);
    void *new_addr = __tlsf_realloc(&tlsf_global, addr, size);
    tlsf_unlock(&tlsf_global, &node);
    
    return new_addr;
}

static void *rtalloc_p2aligned_tlsf(size_t size, u32 alignment)
{
    struct mcsnode node = INITIALIZE_MCSNODE();

    tlsf_lock(&tlsf_global, &node);
    void *addr = __tlsf_alloc_p2aligned(&tlsf_global, size, alignment);
    tlsf_unlock(&tlsf_global, &node);

    return addr;
}

static void rtfree_p2aligned_tlsf(void *addr)
{
    struct mcsnode node = INITIALIZE_MCSNODE();

    tlsf_lock(&tlsf_global, &node);
    __tlsf_free_p2aligned(&tlsf_global, addr);
    tlsf_unlock(&tlsf_global, &node);
}

static void *rtrealloc_p2aligned_tlsf(void *addr, size_t size, u32 alignment)
{
    struct mcsnode node = INITIALIZE_MCSNODE();

    tlsf_lock(&tlsf_global, &node);

    void *new_addr =  __tlsf_realloc_p2aligned(&tlsf_global, addr, size,
                                               alignment);
                                               
    tlsf_unlock(&tlsf_global, &node);

    return new_addr;
}

static struct rtalloc_interface rtalloc_interface = {
    .rtalloc_func = rtalloc_tlsf,
    .rtfree_func = rtfree_tlsf,
    .rtrealloc_func = rtrealloc_tlsf,

    .rtalloc_p2aligned_func = rtalloc_p2aligned_tlsf,
    .rtfree_p2aligned_func = rtfree_p2aligned_tlsf,
    .rtrealloc_p2aligned_func = rtrealloc_p2aligned_tlsf
};

static __late_initcall void rtalloc_tlsf_init(void)
{
    struct twan_kernel *kernel = twan();
    
    u64 heap_start = kernel->mem.heap_start;
    u64 heap_size = kernel->mem.heap_size;
    
    if (__tlsf_init(&tlsf_global, heap_start, heap_size) == 0)
        rtalloc_init(&rtalloc_interface);
}

REGISTER_LATE_INITCALL(rtalloc_tlsf, rtalloc_tlsf_init, 
                       LATE_MEM_RTALLOC_TLSF_ORDER);
#endif