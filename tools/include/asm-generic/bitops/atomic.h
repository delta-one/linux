/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _TOOLS_LINUX_ASM_GENERIC_BITOPS_ATOMIC_H_
#define _TOOLS_LINUX_ASM_GENERIC_BITOPS_ATOMIC_H_

#include <asm/types.h>
#include <asm/bitsperlong.h>

<<<<<<< HEAD
/*
 * Just alias the test versions, all of the compiler built-in atomics "fetch",
 * and optimizing compile-time constants on x86 isn't worth the complexity.
 */
#define set_bit test_and_set_bit
#define clear_bit test_and_clear_bit
=======
static inline void set_bit(int nr, unsigned long *addr)
{
	addr[nr / __BITS_PER_LONG] |= 1UL << (nr % __BITS_PER_LONG);
}

static inline void clear_bit(int nr, unsigned long *addr)
{
	addr[nr / __BITS_PER_LONG] &= ~(1UL << (nr % __BITS_PER_LONG));
}
>>>>>>> b7ba80a49124 (Commit)

#endif /* _TOOLS_LINUX_ASM_GENERIC_BITOPS_ATOMIC_H_ */
