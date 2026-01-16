#include <include/kernel/sched/sched_timer.h>
#include <include/kernel/isr/base_isrs.h>
#include <include/kernel/apic/apic.h>
#include <include/subsys/twanvisor/vconf.h>
#include <include/lib/libtwanvisor/libvcalls.h>
#include <include/lib/libtwanvisor/libvc.h>

void sched_timer_init(u32 time_slice_ms)
{
#if TWANVISOR_ON

    if (twan()->flags.fields.twanvisor_on != 0) {

        u32 ticks = ms_to_ticks(time_slice_ms, 
                                this_cpu_data()->vtimer_frequency_hz);

        sched_timer_enable(ticks);
        return;
    }

#endif

    lapic_timer_init(SCHED_TIMER_VECTOR, time_slice_ms);
}

u32 sched_timer_disable(void)
{
#if TWANVISOR_ON

    if (twan()->flags.fields.twanvisor_on != 0) {

        long rem = tv_vdisarm_timern(PV_SCHED_TIMER);
        KBUG_ON(rem < 0);
    
        return rem;
    }

#endif

    return lapic_timer_disable();
}

void sched_timer_enable(u32 initial)
{
#if TWANVISOR_ON

    if (twan()->flags.fields.twanvisor_on != 0) {

        KBUG_ON(tv_varm_timern(SCHED_TIMER_VECTOR, PV_SCHED_TIMER, 
                false, initial, true) < 0);

        return;
    }

#endif

    lapic_timer_enable(initial);
}

bool sched_is_timer_pending(void)
{
#if TWANVISOR_ON

    if (twan()->flags.fields.twanvisor_on != 0)
        return tv_vintc_is_pending(SCHED_TIMER_VECTOR);

#endif

    return is_lapic_timer_pending(SCHED_TIMER_VECTOR);
}