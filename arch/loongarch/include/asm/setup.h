/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020-2022 Loongson Technology Corporation Limited
 */

#ifndef _LOONGARCH_SETUP_H
#define _LOONGARCH_SETUP_H

#include <linux/types.h>
#include <uapi/asm/setup.h>

#define VECSIZE 0x200

extern unsigned long eentry;
extern unsigned long tlbrentry;
<<<<<<< HEAD
extern char init_command_line[COMMAND_LINE_SIZE];
extern void tlb_init(int cpu);
extern void cpu_cache_init(void);
extern void cache_error_setup(void);
=======
extern void cpu_cache_init(void);
>>>>>>> b7ba80a49124 (Commit)
extern void per_cpu_trap_init(int cpu);
extern void set_handler(unsigned long offset, void *addr, unsigned long len);
extern void set_merr_handler(unsigned long offset, void *addr, unsigned long len);

<<<<<<< HEAD
#ifdef CONFIG_RELOCATABLE

struct rela_la_abs {
	long offset;
	long symvalue;
};

extern long __la_abs_begin;
extern long __la_abs_end;
extern long __rela_dyn_begin;
extern long __rela_dyn_end;

extern void * __init relocate_kernel(void);

#endif

=======
>>>>>>> b7ba80a49124 (Commit)
#endif /* __SETUP_H */
