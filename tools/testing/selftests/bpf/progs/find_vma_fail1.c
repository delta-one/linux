// SPDX-License-Identifier: GPL-2.0
/* Copyright (c) 2021 Facebook */
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
<<<<<<< HEAD
#define vm_flags vm_start
=======
>>>>>>> b7ba80a49124 (Commit)

char _license[] SEC("license") = "GPL";

struct callback_ctx {
	int dummy;
};

static long write_vma(struct task_struct *task, struct vm_area_struct *vma,
		      struct callback_ctx *data)
{
	/* writing to vma, which is illegal */
<<<<<<< HEAD
	vma->vm_start = 0xffffffffff600000;
=======
	vma->vm_flags |= 0x55;
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

SEC("raw_tp/sys_enter")
int handle_getpid(void)
{
	struct task_struct *task = bpf_get_current_task_btf();
	struct callback_ctx data = {};

	bpf_find_vma(task, 0, write_vma, &data, 0);
	return 0;
}
