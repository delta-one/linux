/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASM_SIGNAL_H
#define __ASM_SIGNAL_H

#include <uapi/asm/signal.h>
#include <uapi/asm/ptrace.h>

asmlinkage __visible
<<<<<<< HEAD
void do_work_pending(struct pt_regs *regs, unsigned long thread_info_flags);
=======
void do_notify_resume(struct pt_regs *regs, unsigned long thread_info_flags);
>>>>>>> b7ba80a49124 (Commit)

#endif
