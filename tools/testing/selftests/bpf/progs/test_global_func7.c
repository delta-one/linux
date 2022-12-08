// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2020 Facebook */
#include <stddef.h>
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
<<<<<<< HEAD
#include "bpf_misc.h"
=======
>>>>>>> b7ba80a49124 (Commit)

__attribute__ ((noinline))
void foo(struct __sk_buff *skb)
{
	skb->tc_index = 0;
}

SEC("tc")
<<<<<<< HEAD
__failure __msg("foo() doesn't return scalar")
int global_func7(struct __sk_buff *skb)
=======
int test_cls(struct __sk_buff *skb)
>>>>>>> b7ba80a49124 (Commit)
{
	foo(skb);
	return 0;
}
