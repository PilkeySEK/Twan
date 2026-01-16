#include <include/kernel/sched/sched.h>
#include <include/kernel/kapi.h>
#include <include/subsys/debug/kdbg/kdbg.h>

#if !SCHED_GLOBAL_QUEUE

/* push mcsnodes outside of the stack, the wc stack size should preferably not
   be linear with cpu count */
static struct mcsnode mcs_nodes[NUM_CPUS][SCHED_NUM_QUEUES];

bool try_steal_task(u32 criticality_level)
{
    /* steal highest priority task we can find based on the criticality level, 
       however this approach has high contention, worst case snatch the highest
       priority task */

    struct scheduler *sched = scheduler();
    u32 this_queue_id = this_queue_id();
    u32 num_queues = ARRAY_LEN(sched->queues);
    struct mcsnode *nodes = mcs_nodes[this_processor_id()];

    struct sched_priorityq *chosen_sched_priorityq = NULL;
    int chosen_priority = LOWEST_PRIORITY;
    u32 chosen_criticality = 0;
    struct mcsnode *chosen_sched_node;

    for (u32 i = 0; i < num_queues; i++) {

        if (i == this_queue_id)
            continue;

        struct sched_priorityq *sched_priorityq = &sched->queues[i];
        u32 criticality = criticality_level;
        struct mcsnode *sched_node = &nodes[i];

        mcsnode_init(sched_node);
        mcs_lock_isr_save(&sched_priorityq->lock, sched_node);

        int priority = __sched_stealable_priorityq_highest_priority(
                            sched_priorityq, criticality);

        /* fallback to lower criticalities if we havent stole a task of 
           sufficient criticality yet */

        if (priority == -1) {

            if (chosen_criticality != criticality_level) {

                criticality = 0;
                priority = __sched_stealable_priorityq_highest_priority(
                                sched_priorityq, criticality);

                if (priority != -1)
                    goto recovered;
            }

            mcs_unlock_isr_restore(&sched_priorityq->lock, sched_node);
            continue;
        }

    recovered:

        if (chosen_sched_priorityq) {

            if (criticality < chosen_criticality ||
                (criticality == chosen_criticality && 
                priority <= chosen_priority)) {

                mcs_unlock_isr_restore(&sched_priorityq->lock, sched_node);
                continue;
            }

            mcs_unlock_isr_restore(&chosen_sched_priorityq->lock, chosen_sched_node);
        }

        chosen_sched_priorityq = sched_priorityq;
        chosen_priority = priority;
        chosen_criticality = criticality;
        chosen_sched_node = sched_node;
    }

    if (!chosen_sched_priorityq)
        return false;

    struct task *task = __sched_stealable_priorityq_peek(chosen_sched_priorityq,
                                                         chosen_criticality);
                                                    
    __sched_priorityq_dequeue(task);
    
    mcs_unlock_isr_restore(&chosen_sched_priorityq->lock, chosen_sched_node);

    sched_push_on_cpu(task, this_processor_id()); 
    return true;
}

#endif

void sched_worker(__unused void *unused)
{
    /* dummy scheduler worker task that'll just yield, and load balance when per
       cpu runqueues are used */

    while (1) {

#if !SCHED_GLOBAL_QUEUE

        /* the criticality level is written core local so we can access it 
           without grabbing our schedulers lock, we must have interrupts 
           disabled though, if we were to grab our schedulers lock it would 
           lead to a deadlock in try_steal_task */

        u8 conf = sched_load_balancing_conf[this_processor_id()];
        if (conf == LOAD_BALANCING_OFF)
            goto cont; 

        disable_interrupts();

        if (conf != LOAD_BALANCING_CONSERVATIVE ||
            sched_priorityq_highest_priority(this_sched_priorityq()) == -1) {
            
            try_steal_task(__sched_mcs_read_criticality_level());
        }

        enable_interrupts();

    cont:
#endif

        sched_yield();
    }
}

void sched_trampoline_ipi(__unused struct interrupt_info *info, 
                          __unused u64 unused)
{
    struct per_cpu *cpu = this_cpu_data();
    struct interrupt_info *ctx = task_ctx();

    cpu->flags.fields.sched_init = 1;

    struct task *worker = &cpu->worker;
    u64 stack_top = (u64)&cpu->worker_stack[sizeof(cpu->worker_stack)];
   
    task_init(worker, this_processor_id(), NULL, stack_top, sched_worker, 
              NULL, LOWEST_PRIORITY, 0, false, NULL, NULL);
   
    sched_push(worker);

    struct task *task = sched_pop_clean_spin();
    sched_set_ctx(ctx, task);

    sched_timer_init(SCHED_TIME_SLICE_MS);
}

void scheduler_init(void)
{
    struct scheduler *sched = scheduler();

    for (u32 i = 0; i < ARRAY_LEN(sched->queues); i++)
        __sched_init(&sched->queues[i]);

    ipi_run_func_others(sched_trampoline_ipi, 0, false);

    u64 num_drivers = num_registered_drivers();
    for (u32 i = 0; i < num_drivers; i++)
        INDIRECT_BRANCH_SAFE(__driver_registry_start[i].init());

    emulate_self_ipi(sched_trampoline_ipi, 0);
}

void sched_preempt(struct interrupt_info *ctx)
{
    struct task *current = current_task();

    if (is_preempted_early(current)) {
        clear_preempted_early(current);
        return;
    }

    if (!is_preemption_enabled(current)) {

        if (sched_should_request_yield(current))
            set_yield_request(current);

        return;
    }

    clear_yield_request(current);
    struct task *task = sched_pop(current);
    if (task)
        sched_switch_ctx(ctx, task, true);
}