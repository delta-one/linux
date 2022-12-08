// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2020 Facebook */
#include <stddef.h>
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
<<<<<<< HEAD
#include "bpf_misc.h"
=======
>>>>>>> b7ba80a49124 (Commit)

__noinline int foo(struct __sk_buff *skb)
{
	return bpf_get_prandom_u32();
}

SEC("cgroup_skb/ingress")
<<<<<<< HEAD
__success
int global_func8(struct __sk_buff *skb)
=======
int test_cls(struct __sk_buff *skb)
>>>>>>> b7ba80a49124 (Commit)
{
	if (!foo(skb))
		return 0;

	return 1;
}
