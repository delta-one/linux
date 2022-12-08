// SPDX-License-Identifier: GPL-2.0-only
#include <stddef.h>
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
<<<<<<< HEAD
#include "bpf_misc.h"
=======
>>>>>>> b7ba80a49124 (Commit)

struct S;

__noinline int foo(const struct S *s)
{
	if (s)
		return bpf_get_prandom_u32() < *(const int *) s;

	return 0;
}

SEC("cgroup_skb/ingress")
<<<<<<< HEAD
__failure __msg("reference type('FWD S') size cannot be determined")
int global_func14(struct __sk_buff *skb)
=======
int test_cls(struct __sk_buff *skb)
>>>>>>> b7ba80a49124 (Commit)
{

	return foo(NULL);
}
