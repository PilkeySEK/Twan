#include <include/kernel/kapi.h>
#include <include/kernel/isr/isr_dispatcher.h>
#include <include/kernel/isr/base_isrs.h>
#include <include/kernel/apic/apic.h>
#include <include/lib/libtwanvisor/libvcalls.h>
#include <include/errno.h>
#include <include/std.h>

extern void __emulate_interrupt(u64 rsp, u8 vector, u64 errcode);

int __map_irq(bool enable, u32 processor_id, u32 irq, u8 vector, 
             bool trig_explicit, bool trig)
{
    if (!cpu_valid(processor_id))
        return -EINVAL;

    return __ioapic_config_irq(!enable, lapic_id_of(processor_id), irq, vector,
                               trig_explicit, trig);
}

int map_irq(bool enable, u32 processor_id, u32 irq, u8 vector)
{
    return __map_irq(enable, processor_id, irq, vector, false, false);
}

int map_irq_trig(bool enable, u32 processor_id, u32 irq, u8 vector, bool trig)
{
    return __map_irq(enable, processor_id, irq, vector, true, trig);   
}

int register_isr(u32 processor_id, u8 vector, isr_func_t func)
{
    struct per_cpu *cpu_data = cpu_data(processor_id);
    if (!cpu_data)
        return -EINVAL;

    void *expected = NULL;
    return atomic_ptr_cmpxchg(&cpu_data->isr_table[vector], &expected, func) ?
           0 : -EALREADY;
}

int unregister_isr(u32 processor_id, u8 vector)
{
    struct per_cpu *cpu_data = cpu_data(processor_id);
    if (!cpu_data)
        return -EINVAL;
   
    atomic_ptr_set(&cpu_data->isr_table[vector], NULL);
    return 0;
}

int __ipi_run_func(u32 processor_id, ipi_func_t func, u64 arg, bool wait)
{
    if (!cpu_valid(processor_id))
        return -EINVAL;

    if (processor_id == this_processor_id())
        return -EINVAL;

    struct per_cpu *cpu = cpu_data(processor_id);
    if (!cpu_active(cpu->flags))
        return -EINVAL;

    spin_until(atomic32_read(&cpu->ipi_data.signal) == 0);

    cpu->ipi_data.func = func;
    cpu->ipi_data.arg = arg;
    cpu->ipi_data.wait = wait;

    atomic32_set(&cpu->ipi_data.signal, 1);

#if TWANVISOR_ON

    if (twan()->flags.fields.twanvisor_on != 0) {

        tv_vipi(processor_id, VIPI_DM_EXTERNAL, IPI_CMD_VECTOR, false);

    } else {
        lapic_send_ipi(lapic_id_of(processor_id), DM_NORMAL, 
                        LAPIC_DEST_PHYSICAL, SINGLE_TARGET, IPI_CMD_VECTOR);
    }

#else

    lapic_send_ipi(lapic_id_of(processor_id), DM_NORMAL, LAPIC_DEST_PHYSICAL,
                   SINGLE_TARGET, IPI_CMD_VECTOR);

#endif

    if (wait)
        spin_until(atomic32_read(&cpu->ipi_data.signal) == 0);

    return 0;
}

int ipi_run_func(u32 processor_id, ipi_func_t func, u64 arg, bool wait)
{
    struct per_cpu *this_cpu = this_cpu_data();

    KBUG_ON(this_cpu->handling_isr);
    KBUG_ON(!is_interrupts_enabled());

    if (!cpu_valid(processor_id))
        return -EINVAL;

    if (processor_id == this_processor_id())
        return -EINVAL;

    struct per_cpu *cpu = cpu_data(processor_id);
    if (!cpu_active(cpu->flags))
        return -EINVAL;

    struct mcsnode node = INITIALIZE_MCSNODE();

    if (this_cpu->flags.fields.sched_init == 0) {

        __pv_mcs_lock(&cpu->ipi_data.lock, &node);
        int ret = __ipi_run_func(processor_id, func, arg, wait);
        __pv_mcs_unlock(&cpu->ipi_data.lock, &node);
        
        return ret;
    }

    mcs_lock(&cpu->ipi_data.lock, &node);
    int ret = __ipi_run_func(processor_id, func, arg, wait);
    mcs_unlock(&cpu->ipi_data.lock, &node);

    return ret;
}

void ipi_run_func_others(ipi_func_t func, u64 arg, bool wait)
{
    u32 processor_id = this_processor_id();

    u32 num = num_cpus();
    for (u32 i = 0; i < num; i++) {

        struct per_cpu *cpu = cpu_data(i);
        
        if (i != processor_id && cpu_enabled(cpu->flags))
            ipi_run_func(i, func, arg, wait);
    }
}

void emulate_self_ipi(ipi_func_t func, u64 arg)
{
    u64 flags = read_flags_and_disable_interrupts();

    struct per_cpu *this_cpu = this_cpu_data();

    this_cpu->self_ipi_func = func;
    this_cpu->self_ipi_arg = arg;
    
    u64 rsp = (u64)&this_cpu->int_stack_stub[sizeof(this_cpu->int_stack_stub)];
    
    __emulate_interrupt(rsp, SELF_IPI_CMD_VECTOR, 0);

    write_flags(flags);
}

void dead_local(void)
{
    disable_interrupts();
    halt_loop();
}

void dead_global(void)
{
    if (twan()->flags.fields.multicore_initialized != 0)
        ipi_run_func_others((ipi_func_t)dead_local, 0, false);

    dead_local();
}