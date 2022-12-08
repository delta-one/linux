/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __ASM_VDSO_PROCESSOR_H
#define __ASM_VDSO_PROCESSOR_H

#ifndef __ASSEMBLY__

<<<<<<< HEAD
#include <asm/barrier.h>

static inline void cpu_relax(void)
{
#ifdef __riscv_muldiv
	int dummy;
	/* In lieu of a halt instruction, induce a long-latency stall. */
	__asm__ __volatile__ ("div %0, %0, zero" : "=r" (dummy));
#endif

#ifdef __riscv_zihintpause
	/*
	 * Reduce instruction retirement.
	 * This assumes the PC changes.
	 */
	__asm__ __volatile__ ("pause");
#else
	/* Encoding of the pause instruction */
	__asm__ __volatile__ (".4byte 0x100000F");
#endif
=======
#include <linux/jump_label.h>
#include <asm/barrier.h>
#include <asm/hwcap.h>

static inline void cpu_relax(void)
{
	if (!static_branch_likely(&riscv_isa_ext_keys[RISCV_ISA_EXT_KEY_ZIHINTPAUSE])) {
#ifdef __riscv_muldiv
		int dummy;
		/* In lieu of a halt instruction, induce a long-latency stall. */
		__asm__ __volatile__ ("div %0, %0, zero" : "=r" (dummy));
#endif
	} else {
		/*
		 * Reduce instruction retirement.
		 * This assumes the PC changes.
		 */
#ifdef __riscv_zihintpause
		__asm__ __volatile__ ("pause");
#else
		/* Encoding of the pause instruction */
		__asm__ __volatile__ (".4byte 0x100000F");
#endif
	}
>>>>>>> b7ba80a49124 (Commit)
	barrier();
}

#endif /* __ASSEMBLY__ */

#endif /* __ASM_VDSO_PROCESSOR_H */
