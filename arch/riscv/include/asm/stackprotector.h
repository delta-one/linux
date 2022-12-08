/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _ASM_RISCV_STACKPROTECTOR_H
#define _ASM_RISCV_STACKPROTECTOR_H

<<<<<<< HEAD
=======
#include <linux/random.h>
#include <linux/version.h>

>>>>>>> b7ba80a49124 (Commit)
extern unsigned long __stack_chk_guard;

/*
 * Initialize the stackprotector canary value.
 *
 * NOTE: this must only be called from functions that never return,
 * and it must always be inlined.
 */
static __always_inline void boot_init_stack_canary(void)
{
<<<<<<< HEAD
	unsigned long canary = get_random_canary();
=======
	unsigned long canary;

	/* Try to get a semi random initial value. */
	get_random_bytes(&canary, sizeof(canary));
	canary ^= LINUX_VERSION_CODE;
	canary &= CANARY_MASK;
>>>>>>> b7ba80a49124 (Commit)

	current->stack_canary = canary;
	if (!IS_ENABLED(CONFIG_STACKPROTECTOR_PER_TASK))
		__stack_chk_guard = current->stack_canary;
}
#endif /* _ASM_RISCV_STACKPROTECTOR_H */
