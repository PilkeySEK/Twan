#ifndef _LATE_INITCALLS_CONF_H_
#define _LATE_INITCALLS_CONF_H_

/* mem */

#define LATE_MEM_RTALLOC_TLSF 1
#define LATE_MEM_RTALLOC_TLSF_ORDER 0

/* time */

#define LATE_TIME_HPET 1
#define LATE_TIME_HPET_ORDER 0

#define LATE_TIME_CMOS 1
#define LATE_TIME_CMOS_ORDER 3

/* cokernel */

#define LATE_COKERNEL_TWANVISOR_ORDER 2

/* paravirt */

#define LATE_PV_TWANVISOR_ORDER 3

#endif