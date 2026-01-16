#include <include/generated/autoconf.h>
#if CONFIG_DEMO_DRIVERS

#include <include/kernel/kapi.h>
#include <include/subsys/debug/kdbg/kdbg.h>

static __driver_init void demo(void)
{
    kdbg("Hello World!\n");
}

REGISTER_DRIVER(demo, demo);

#endif