#include <include/subsys/sync/mutex.h>
#include <include/kernel/kapi.h>

int mutex_ipcp_init(struct mutex_ipcp *mutex_ipcp, u8 priority_ceiling, 
                    u32 criticality_ceiling)

{
    if (criticality_ceiling >= SCHED_NUM_CRITICALITIES)
        return -EINVAL;

    mutex_ipcp->priority_ceiling = priority_ceiling;
    mutex_ipcp->criticality_ceiling = criticality_ceiling;

    atomic_ptr_set(&mutex_ipcp->holder, NULL);
    mutex_ipcp->count = 0;
    waitq_init(&mutex_ipcp->waitq);

    return 0;
}

bool mutex_ipcp_trylock(struct mutex_ipcp *mutex_ipcp)
{
    KBUG_ON(this_cpu_data()->handling_isr);

    struct task *current = current_task();

    current_task_disable_preemption();

    void *expected = NULL;
    if (atomic_ptr_cmpxchg(&mutex_ipcp->holder, &expected, current)) {
        
        current_task_ipcp_boost(mutex_ipcp->priority_ceiling, 
                                mutex_ipcp->criticality_ceiling);

        mutex_ipcp->count = 1;
        current_task_enable_preemption();
        return true;
    }

    bool ret = expected == current;
    if (ret) {
        KBUG_ON(mutex_ipcp->count == UINT64_MAX);
        mutex_ipcp->count++;
    }

    current_task_enable_preemption();
    return ret;
}

void mutex_ipcp_lock(struct mutex_ipcp *mutex_ipcp)
{
    current_task_ipcp_boost(mutex_ipcp->priority_ceiling, 
                            mutex_ipcp->criticality_ceiling);
                            
    wait_until_insert_real(&mutex_ipcp->waitq, mutex_ipcp_trylock(mutex_ipcp));
}

bool mutex_ipcp_lock_timeout(struct mutex_ipcp *mutex_ipcp, u32 ticks)
{
    current_task_ipcp_boost(mutex_ipcp->priority_ceiling, 
                            mutex_ipcp->criticality_ceiling);

    bool ret = wait_until_insert_real_timeout(&mutex_ipcp->waitq, 
                                              mutex_ipcp_trylock(mutex_ipcp), 
                                              ticks);
                                    
    if (!ret)
        current_task_ipcp_sync();

    return ret;
}

void mutex_ipcp_unlock(struct mutex_ipcp *mutex_ipcp)
{
    KBUG_ON(atomic_ptr_read(&mutex_ipcp->holder) != current_task());

    mutex_ipcp->count--;

    if (mutex_ipcp->count > 0)
        return;

    waitq_wakeup_front(&mutex_ipcp->waitq, &mutex_ipcp->holder);
    current_task_ipcp_sync();
}