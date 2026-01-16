#ifndef _PAGING_CONF_H_
#define _PAGING_CONF_H_

#include <include/kernel/mem/mem_conf.h>
#include <include/kernel/kernel.h>

#define MAX_HEAP_PFN_COUNT (MAX_HEAP_SIZE / PAGE_SIZE_2MB)
STATIC_ASSERT(MAX_HEAP_PFN_COUNT <= ARRAY_LEN(pd_heap));

#endif