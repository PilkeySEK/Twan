#ifndef _EXTERN_H_
#define _EXTERN_H_

#include <include/kernel/isr/isr_index.h>
#include <include/lib/x86_index.h>

extern char __bss_start[];
extern char __bss_end[];

extern char __kernel_start[];
extern char __kernel_end[];

extern struct descriptor_table64 gdtr64;
extern struct descriptor_table64 idtr;

/* kernel page tables */
extern pml4e_t pml4[512];

/* kernel image & kernel heap */
extern pdpte_t pdpt[512]; // first mapped pdpt
extern pde_huge_t pd_kernel[512]; // kernel 
extern pte_t pt_kernel_start[512]; // kernel start with 4KB hole
extern pde_huge_t pd_heap[512]; // kernel heap

/* kernel reserved page tables */
extern pdpte_t pdpt_reserved[512]; // pdpt for mappers and io
extern pde_huge_t pd_pfn[512]; // used for mapping specific pfn's
extern pde_huge_t pd_pfn_io[512]; // used for mapping specific pfn's as io mem
extern pde_huge_t pd_keyholes[512]; // used as keyholes for pt walks

extern struct idt_descriptor64 idt[256];

/* ap data */
extern char __wakeup[];
extern char ap_rsp[8]; // set this to a stack ptr for the ap
extern char __wakeup_end[];

extern u64 __wakeup_blob_size;
extern char __wakeup_blob_save_area[];

/* isr entry point addresses */
extern u64 isr_entry_table[NUM_VECTORS];

#endif