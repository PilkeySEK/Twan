#ifndef _MEM_CONF_H_
#define _MEM_CONF_H_

#include <include/kernel/kernel.h>

#define MAX_HEAP_SIZE CONFIG_KERNEL_MAX_HEAP_SIZE
STATIC_ASSERT(MAX_HEAP_SIZE <= PAGE_SIZE_1GB);
 
#endif