// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2020 Facebook */
#include <stddef.h>
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
<<<<<<< HEAD
#include "bpf_misc.h"

#define MAX_STACK (512 - 3 * 32 + 8)
=======

#ifndef MAX_STACK
#define MAX_STACK (512 - 3 * 32 + 8)
#endif
>>>>>>> b7ba80a49124 (Commit)

static __attribute__ ((noinline))
int f0(int var, struct __sk_buff *skb)
{
	return skb->len;
}

__attribute__ ((noinline))
int f1(struct __sk_buff *skb)
{
	volatile char buf[MAX_STACK] = {};

<<<<<<< HEAD
	__sink(buf[MAX_STACK - 1]);

=======
>>>>>>> b7ba80a49124 (Commit)
	return f0(0, skb) + skb->len;
}

int f3(int, struct __sk_buff *skb, int);

__attribute__ ((noinline))
int f2(int val, struct __sk_buff *skb)
{
	return f1(skb) + f3(val, skb, 1);
}

__attribute__ ((noinline))
int f3(int val, struct __sk_buff *skb, int var)
{
	volatile char buf[MAX_STACK] = {};

<<<<<<< HEAD
	__sink(buf[MAX_STACK - 1]);

=======
>>>>>>> b7ba80a49124 (Commit)
	return skb->ifindex * val * var;
}

SEC("tc")
<<<<<<< HEAD
__failure __msg("combined stack size of 4 calls is 544")
int global_func1(struct __sk_buff *skb)
=======
int test_cls(struct __sk_buff *skb)
>>>>>>> b7ba80a49124 (Commit)
{
	return f0(1, skb) + f1(skb) + f2(2, skb) + f3(3, skb, 4);
}
