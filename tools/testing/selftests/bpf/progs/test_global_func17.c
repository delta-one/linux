// SPDX-License-Identifier: GPL-2.0-only
#include <vmlinux.h>
#include <bpf/bpf_helpers.h>
<<<<<<< HEAD
#include "bpf_misc.h"
=======
>>>>>>> b7ba80a49124 (Commit)

__noinline int foo(int *p)
{
	return p ? (*p = 42) : 0;
}

const volatile int i;

SEC("tc")
<<<<<<< HEAD
__failure __msg("Caller passes invalid args into func#1")
int global_func17(struct __sk_buff *skb)
=======
int test_cls(struct __sk_buff *skb)
>>>>>>> b7ba80a49124 (Commit)
{
	return foo((int *)&i);
}
