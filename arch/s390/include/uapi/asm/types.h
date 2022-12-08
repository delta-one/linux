/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/*
 *  S390 version
 *
 *  Derived from "include/asm-i386/types.h"
 */

#ifndef _UAPI_S390_TYPES_H
#define _UAPI_S390_TYPES_H

#include <asm-generic/int-ll64.h>

#ifndef __ASSEMBLY__

<<<<<<< HEAD
typedef unsigned long addr_t;
typedef __signed__ long saddr_t;

typedef struct {
	union {
		struct {
			__u64 high;
			__u64 low;
		};
		__u32 u[4];
	};
} __attribute__((packed, aligned(4))) __vector128;
=======
/* A address type so that arithmetic can be done on it & it can be upgraded to
   64 bit when necessary 
*/
typedef unsigned long addr_t; 
typedef __signed__ long saddr_t;

typedef struct {
	__u32 u[4];
} __vector128;
>>>>>>> b7ba80a49124 (Commit)

#endif /* __ASSEMBLY__ */

#endif /* _UAPI_S390_TYPES_H */
