#ifndef _VSCHED_YIELD_H_
#define _VSCHED_YIELD_H_

#include <include/subsys/twanvisor/vsched/vcpu.h>


bool __vsched_is_current_preemptible(struct vcpu *current);
bool vsched_should_request_yield(struct vcpu *current);

void vsched_yield_ipi(__unused struct interrupt_info *unused0,
                      __unused u64 unused1);

void vsched_yield(void);


void vsched_pause_ipi(__unused struct interrupt_info *unused0,
                      u64 pv_spin);
                      
void vsched_pause(bool pv_spin);


void vsched_recover_ipi(__unused struct interrupt_info *unused0, 
                        __unused u64 unused1);

void vsched_recover(void);

#endif