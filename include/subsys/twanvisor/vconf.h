#ifndef _VCONF_H_
#define _VCONF_H_

#include <include/generated/autoconf.h>

#define TWANVISOR_ON CONFIG_TWANVISOR_ON
#define VIPI_DRAIN_STRICT_CONF CONFIG_TWANVISOR_VIPI_STRICT_ON

#if TWANVISOR_ON

#define TWANVISOR_PV_LOCKS_ON CONFIG_TWANVISOR_PV_LOCKS_ON

#else 

#define TWANVISOR_PV_LOCKS_ON 0

#endif 

#endif