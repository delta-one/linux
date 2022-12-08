// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022 Loongson Technology Corporation Limited
 */
<<<<<<< HEAD
=======
#include <linux/kernel.h>

>>>>>>> b7ba80a49124 (Commit)
#include <asm/unwind.h>

unsigned long unwind_get_return_address(struct unwind_state *state)
{
<<<<<<< HEAD
	return __unwind_get_return_address(state);
=======
	if (unwind_done(state))
		return 0;
	else if (state->first)
		return state->pc;

	return *(unsigned long *)(state->sp);
>>>>>>> b7ba80a49124 (Commit)
}
EXPORT_SYMBOL_GPL(unwind_get_return_address);

void unwind_start(struct unwind_state *state, struct task_struct *task,
		    struct pt_regs *regs)
{
<<<<<<< HEAD
	__unwind_start(state, task, regs);
=======
	memset(state, 0, sizeof(*state));

	if (regs) {
		state->sp = regs->regs[3];
		state->pc = regs->csr_era;
	}

	state->task = task;
	state->first = true;

	get_stack_info(state->sp, state->task, &state->stack_info);

>>>>>>> b7ba80a49124 (Commit)
	if (!unwind_done(state) && !__kernel_text_address(state->pc))
		unwind_next_frame(state);
}
EXPORT_SYMBOL_GPL(unwind_start);

bool unwind_next_frame(struct unwind_state *state)
{
<<<<<<< HEAD
	return default_next_frame(state);
=======
	struct stack_info *info = &state->stack_info;
	unsigned long addr;

	if (unwind_done(state))
		return false;

	if (state->first)
		state->first = false;

	do {
		for (state->sp += sizeof(unsigned long);
		     state->sp < info->end;
		     state->sp += sizeof(unsigned long)) {
			addr = *(unsigned long *)(state->sp);

			if (__kernel_text_address(addr))
				return true;
		}

		state->sp = info->next_sp;

	} while (!get_stack_info(state->sp, state->task, info));

	return false;
>>>>>>> b7ba80a49124 (Commit)
}
EXPORT_SYMBOL_GPL(unwind_next_frame);
