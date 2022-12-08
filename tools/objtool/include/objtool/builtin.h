/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2015 Josh Poimboeuf <jpoimboe@redhat.com>
 */
#ifndef _BUILTIN_H
#define _BUILTIN_H

#include <subcmd/parse-options.h>

<<<<<<< HEAD
=======
extern const struct option check_options[];

>>>>>>> b7ba80a49124 (Commit)
struct opts {
	/* actions: */
	bool dump_orc;
	bool hack_jump_label;
	bool hack_noinstr;
<<<<<<< HEAD
	bool hack_skylake;
=======
>>>>>>> b7ba80a49124 (Commit)
	bool ibt;
	bool mcount;
	bool noinstr;
	bool orc;
	bool retpoline;
	bool rethunk;
	bool unret;
	bool sls;
	bool stackval;
	bool static_call;
	bool uaccess;
<<<<<<< HEAD
	int prefix;
	bool cfi;
=======
>>>>>>> b7ba80a49124 (Commit)

	/* options: */
	bool backtrace;
	bool backup;
	bool dryrun;
	bool link;
<<<<<<< HEAD
	bool mnop;
=======
>>>>>>> b7ba80a49124 (Commit)
	bool module;
	bool no_unreachable;
	bool sec_address;
	bool stats;
};

extern struct opts opts;

extern int cmd_parse_options(int argc, const char **argv, const char * const usage[]);

extern int objtool_run(int argc, const char **argv);

#endif /* _BUILTIN_H */
