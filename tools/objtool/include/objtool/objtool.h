/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2020 Matt Helsley <mhelsley@vmware.com>
 */

#ifndef _OBJTOOL_H
#define _OBJTOOL_H

#include <stdbool.h>
#include <linux/list.h>
#include <linux/hashtable.h>

#include <objtool/elf.h>

#define __weak __attribute__((weak))

struct pv_state {
	bool clean;
	struct list_head targets;
};

struct objtool_file {
	struct elf *elf;
<<<<<<< HEAD
=======
	struct list_head insn_list;
>>>>>>> b7ba80a49124 (Commit)
	DECLARE_HASHTABLE(insn_hash, 20);
	struct list_head retpoline_call_list;
	struct list_head return_thunk_list;
	struct list_head static_call_list;
	struct list_head mcount_loc_list;
	struct list_head endbr_list;
<<<<<<< HEAD
	struct list_head call_list;
=======
>>>>>>> b7ba80a49124 (Commit)
	bool ignore_unreachables, hints, rodata;

	unsigned int nr_endbr;
	unsigned int nr_endbr_int;

	unsigned long jl_short, jl_long;
	unsigned long jl_nop_short, jl_nop_long;

	struct pv_state *pv_ops;
};

struct objtool_file *objtool_open_read(const char *_objname);

void objtool_pv_add(struct objtool_file *file, int idx, struct symbol *func);

int check(struct objtool_file *file);
int orc_dump(const char *objname);
int orc_create(struct objtool_file *file);

#endif /* _OBJTOOL_H */
