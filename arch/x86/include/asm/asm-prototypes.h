/* SPDX-License-Identifier: GPL-2.0 */
#include <asm/ftrace.h>
#include <linux/uaccess.h>
#include <linux/pgtable.h>
#include <asm/string.h>
#include <asm/page.h>
#include <asm/checksum.h>
#include <asm/mce.h>

#include <asm-generic/asm-prototypes.h>

#include <asm/special_insns.h>
#include <asm/preempt.h>
#include <asm/asm.h>
<<<<<<< HEAD
#include <asm/gsseg.h>
=======
>>>>>>> b7ba80a49124 (Commit)

#ifndef CONFIG_X86_CMPXCHG64
extern void cmpxchg8b_emu(void);
#endif

