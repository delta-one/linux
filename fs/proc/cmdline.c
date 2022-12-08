// SPDX-License-Identifier: GPL-2.0
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
<<<<<<< HEAD
#include "internal.h"
=======
>>>>>>> b7ba80a49124 (Commit)

static int cmdline_proc_show(struct seq_file *m, void *v)
{
	seq_puts(m, saved_command_line);
	seq_putc(m, '\n');
	return 0;
}

static int __init proc_cmdline_init(void)
{
<<<<<<< HEAD
	struct proc_dir_entry *pde;

	pde = proc_create_single("cmdline", 0, NULL, cmdline_proc_show);
	pde_make_permanent(pde);
	pde->size = saved_command_line_len + 1;
=======
	proc_create_single("cmdline", 0, NULL, cmdline_proc_show);
>>>>>>> b7ba80a49124 (Commit)
	return 0;
}
fs_initcall(proc_cmdline_init);
