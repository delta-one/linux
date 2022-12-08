/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_SHARED_IO_H
#define _ASM_X86_SHARED_IO_H

#include <linux/types.h>

#define BUILDIO(bwl, bw, type)						\
<<<<<<< HEAD
static __always_inline void __out##bwl(type value, u16 port)		\
=======
static inline void __out##bwl(type value, u16 port)			\
>>>>>>> b7ba80a49124 (Commit)
{									\
	asm volatile("out" #bwl " %" #bw "0, %w1"			\
		     : : "a"(value), "Nd"(port));			\
}									\
									\
<<<<<<< HEAD
static __always_inline type __in##bwl(u16 port)				\
=======
static inline type __in##bwl(u16 port)					\
>>>>>>> b7ba80a49124 (Commit)
{									\
	type value;							\
	asm volatile("in" #bwl " %w1, %" #bw "0"			\
		     : "=a"(value) : "Nd"(port));			\
	return value;							\
}

BUILDIO(b, b, u8)
BUILDIO(w, w, u16)
BUILDIO(l,  , u32)
#undef BUILDIO

#define inb __inb
#define inw __inw
#define inl __inl
#define outb __outb
#define outw __outw
#define outl __outl

#endif
