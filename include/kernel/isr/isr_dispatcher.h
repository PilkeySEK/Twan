#ifndef _ISR_DISPATCHER_H_
#define _ISR_DISPATCHER_H_

#include <include/kernel/isr/isr_index.h>
#include <include/kernel/sched/sched.h>

void __acknowledge_interrupt(void);
bool __in_service(u8 vector);

bool is_critical_exception(u8 vector);
void log_exception(struct interrupt_info *stack_trace);

void ipi_handler(struct interrupt_info *info);
void self_ipi_handler(struct interrupt_info *info);

void __isr_dispatcher(struct interrupt_info *stack_trace);

#endif