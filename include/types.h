#ifndef _TYPES_H_
#define _TYPES_H_

#define _Bool int

#ifndef bool
typedef int bool;
#define true 1
#define false 0
#endif

#ifndef NULL
#define NULL 0
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef long unsigned int size_t;

#endif