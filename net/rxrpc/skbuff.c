// SPDX-License-Identifier: GPL-2.0-or-later
<<<<<<< HEAD
/* Socket buffer accounting
=======
/* ar-skbuff.c: socket buffer destruction handling
>>>>>>> b7ba80a49124 (Commit)
 *
 * Copyright (C) 2007 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/net.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <net/af_rxrpc.h>
#include "ar-internal.h"

<<<<<<< HEAD
#define select_skb_count(skb) (&rxrpc_n_rx_skbs)
=======
#define is_tx_skb(skb) (rxrpc_skb(skb)->rx_flags & RXRPC_SKB_TX_BUFFER)
#define select_skb_count(skb) (is_tx_skb(skb) ? &rxrpc_n_tx_skbs : &rxrpc_n_rx_skbs)
>>>>>>> b7ba80a49124 (Commit)

/*
 * Note the allocation or reception of a socket buffer.
 */
<<<<<<< HEAD
void rxrpc_new_skb(struct sk_buff *skb, enum rxrpc_skb_trace why)
{
	int n = atomic_inc_return(select_skb_count(skb));
	trace_rxrpc_skb(skb, refcount_read(&skb->users), n, why);
=======
void rxrpc_new_skb(struct sk_buff *skb, enum rxrpc_skb_trace op)
{
	const void *here = __builtin_return_address(0);
	int n = atomic_inc_return(select_skb_count(skb));
	trace_rxrpc_skb(skb, op, refcount_read(&skb->users), n,
			rxrpc_skb(skb)->rx_flags, here);
>>>>>>> b7ba80a49124 (Commit)
}

/*
 * Note the re-emergence of a socket buffer from a queue or buffer.
 */
<<<<<<< HEAD
void rxrpc_see_skb(struct sk_buff *skb, enum rxrpc_skb_trace why)
{
	if (skb) {
		int n = atomic_read(select_skb_count(skb));
		trace_rxrpc_skb(skb, refcount_read(&skb->users), n, why);
=======
void rxrpc_see_skb(struct sk_buff *skb, enum rxrpc_skb_trace op)
{
	const void *here = __builtin_return_address(0);
	if (skb) {
		int n = atomic_read(select_skb_count(skb));
		trace_rxrpc_skb(skb, op, refcount_read(&skb->users), n,
				rxrpc_skb(skb)->rx_flags, here);
>>>>>>> b7ba80a49124 (Commit)
	}
}

/*
 * Note the addition of a ref on a socket buffer.
 */
<<<<<<< HEAD
void rxrpc_get_skb(struct sk_buff *skb, enum rxrpc_skb_trace why)
{
	int n = atomic_inc_return(select_skb_count(skb));
	trace_rxrpc_skb(skb, refcount_read(&skb->users), n, why);
=======
void rxrpc_get_skb(struct sk_buff *skb, enum rxrpc_skb_trace op)
{
	const void *here = __builtin_return_address(0);
	int n = atomic_inc_return(select_skb_count(skb));
	trace_rxrpc_skb(skb, op, refcount_read(&skb->users), n,
			rxrpc_skb(skb)->rx_flags, here);
>>>>>>> b7ba80a49124 (Commit)
	skb_get(skb);
}

/*
 * Note the dropping of a ref on a socket buffer by the core.
 */
<<<<<<< HEAD
void rxrpc_eaten_skb(struct sk_buff *skb, enum rxrpc_skb_trace why)
{
	int n = atomic_inc_return(&rxrpc_n_rx_skbs);
	trace_rxrpc_skb(skb, 0, n, why);
=======
void rxrpc_eaten_skb(struct sk_buff *skb, enum rxrpc_skb_trace op)
{
	const void *here = __builtin_return_address(0);
	int n = atomic_inc_return(&rxrpc_n_rx_skbs);
	trace_rxrpc_skb(skb, op, 0, n, 0, here);
>>>>>>> b7ba80a49124 (Commit)
}

/*
 * Note the destruction of a socket buffer.
 */
<<<<<<< HEAD
void rxrpc_free_skb(struct sk_buff *skb, enum rxrpc_skb_trace why)
{
	if (skb) {
		int n = atomic_dec_return(select_skb_count(skb));
		trace_rxrpc_skb(skb, refcount_read(&skb->users), n, why);
		consume_skb(skb);
=======
void rxrpc_free_skb(struct sk_buff *skb, enum rxrpc_skb_trace op)
{
	const void *here = __builtin_return_address(0);
	if (skb) {
		int n;
		n = atomic_dec_return(select_skb_count(skb));
		trace_rxrpc_skb(skb, op, refcount_read(&skb->users), n,
				rxrpc_skb(skb)->rx_flags, here);
		kfree_skb(skb);
>>>>>>> b7ba80a49124 (Commit)
	}
}

/*
 * Clear a queue of socket buffers.
 */
void rxrpc_purge_queue(struct sk_buff_head *list)
{
<<<<<<< HEAD
	struct sk_buff *skb;

	while ((skb = skb_dequeue((list))) != NULL) {
		int n = atomic_dec_return(select_skb_count(skb));
		trace_rxrpc_skb(skb, refcount_read(&skb->users), n,
				rxrpc_skb_put_purge);
		consume_skb(skb);
=======
	const void *here = __builtin_return_address(0);
	struct sk_buff *skb;
	while ((skb = skb_dequeue((list))) != NULL) {
		int n = atomic_dec_return(select_skb_count(skb));
		trace_rxrpc_skb(skb, rxrpc_skb_purged,
				refcount_read(&skb->users), n,
				rxrpc_skb(skb)->rx_flags, here);
		kfree_skb(skb);
>>>>>>> b7ba80a49124 (Commit)
	}
}
