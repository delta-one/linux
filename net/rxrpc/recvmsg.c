// SPDX-License-Identifier: GPL-2.0-or-later
/* RxRPC recvmsg() implementation
 *
 * Copyright (C) 2007 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/net.h>
#include <linux/skbuff.h>
#include <linux/export.h>
#include <linux/sched/signal.h>

#include <net/sock.h>
#include <net/af_rxrpc.h>
#include "ar-internal.h"

/*
 * Post a call for attention by the socket or kernel service.  Further
 * notifications are suppressed by putting recvmsg_link on a dummy queue.
 */
void rxrpc_notify_socket(struct rxrpc_call *call)
{
	struct rxrpc_sock *rx;
	struct sock *sk;

	_enter("%d", call->debug_id);

	if (!list_empty(&call->recvmsg_link))
		return;

	rcu_read_lock();

	rx = rcu_dereference(call->socket);
	sk = &rx->sk;
	if (rx && sk->sk_state < RXRPC_CLOSE) {
		if (call->notify_rx) {
<<<<<<< HEAD
			spin_lock(&call->notify_lock);
			call->notify_rx(sk, call, call->user_call_ID);
			spin_unlock(&call->notify_lock);
		} else {
			spin_lock(&rx->recvmsg_lock);
			if (list_empty(&call->recvmsg_link)) {
				rxrpc_get_call(call, rxrpc_call_get_notify_socket);
				list_add_tail(&call->recvmsg_link, &rx->recvmsg_q);
			}
			spin_unlock(&rx->recvmsg_lock);
=======
			spin_lock_bh(&call->notify_lock);
			call->notify_rx(sk, call, call->user_call_ID);
			spin_unlock_bh(&call->notify_lock);
		} else {
			write_lock_bh(&rx->recvmsg_lock);
			if (list_empty(&call->recvmsg_link)) {
				rxrpc_get_call(call, rxrpc_call_got);
				list_add_tail(&call->recvmsg_link, &rx->recvmsg_q);
			}
			write_unlock_bh(&rx->recvmsg_lock);
>>>>>>> b7ba80a49124 (Commit)

			if (!sock_flag(sk, SOCK_DEAD)) {
				_debug("call %ps", sk->sk_data_ready);
				sk->sk_data_ready(sk);
			}
		}
	}

	rcu_read_unlock();
	_leave("");
}

/*
<<<<<<< HEAD
=======
 * Transition a call to the complete state.
 */
bool __rxrpc_set_call_completion(struct rxrpc_call *call,
				 enum rxrpc_call_completion compl,
				 u32 abort_code,
				 int error)
{
	if (call->state < RXRPC_CALL_COMPLETE) {
		call->abort_code = abort_code;
		call->error = error;
		call->completion = compl;
		call->state = RXRPC_CALL_COMPLETE;
		trace_rxrpc_call_complete(call);
		wake_up(&call->waitq);
		rxrpc_notify_socket(call);
		return true;
	}
	return false;
}

bool rxrpc_set_call_completion(struct rxrpc_call *call,
			       enum rxrpc_call_completion compl,
			       u32 abort_code,
			       int error)
{
	bool ret = false;

	if (call->state < RXRPC_CALL_COMPLETE) {
		write_lock_bh(&call->state_lock);
		ret = __rxrpc_set_call_completion(call, compl, abort_code, error);
		write_unlock_bh(&call->state_lock);
	}
	return ret;
}

/*
 * Record that a call successfully completed.
 */
bool __rxrpc_call_completed(struct rxrpc_call *call)
{
	return __rxrpc_set_call_completion(call, RXRPC_CALL_SUCCEEDED, 0, 0);
}

bool rxrpc_call_completed(struct rxrpc_call *call)
{
	bool ret = false;

	if (call->state < RXRPC_CALL_COMPLETE) {
		write_lock_bh(&call->state_lock);
		ret = __rxrpc_call_completed(call);
		write_unlock_bh(&call->state_lock);
	}
	return ret;
}

/*
 * Record that a call is locally aborted.
 */
bool __rxrpc_abort_call(const char *why, struct rxrpc_call *call,
			rxrpc_seq_t seq, u32 abort_code, int error)
{
	trace_rxrpc_abort(call->debug_id, why, call->cid, call->call_id, seq,
			  abort_code, error);
	return __rxrpc_set_call_completion(call, RXRPC_CALL_LOCALLY_ABORTED,
					   abort_code, error);
}

bool rxrpc_abort_call(const char *why, struct rxrpc_call *call,
		      rxrpc_seq_t seq, u32 abort_code, int error)
{
	bool ret;

	write_lock_bh(&call->state_lock);
	ret = __rxrpc_abort_call(why, call, seq, abort_code, error);
	write_unlock_bh(&call->state_lock);
	return ret;
}

/*
>>>>>>> b7ba80a49124 (Commit)
 * Pass a call terminating message to userspace.
 */
static int rxrpc_recvmsg_term(struct rxrpc_call *call, struct msghdr *msg)
{
	u32 tmp = 0;
	int ret;

	switch (call->completion) {
	case RXRPC_CALL_SUCCEEDED:
		ret = 0;
		if (rxrpc_is_service_call(call))
			ret = put_cmsg(msg, SOL_RXRPC, RXRPC_ACK, 0, &tmp);
		break;
	case RXRPC_CALL_REMOTELY_ABORTED:
		tmp = call->abort_code;
		ret = put_cmsg(msg, SOL_RXRPC, RXRPC_ABORT, 4, &tmp);
		break;
	case RXRPC_CALL_LOCALLY_ABORTED:
		tmp = call->abort_code;
		ret = put_cmsg(msg, SOL_RXRPC, RXRPC_ABORT, 4, &tmp);
		break;
	case RXRPC_CALL_NETWORK_ERROR:
		tmp = -call->error;
		ret = put_cmsg(msg, SOL_RXRPC, RXRPC_NET_ERROR, 4, &tmp);
		break;
	case RXRPC_CALL_LOCAL_ERROR:
		tmp = -call->error;
		ret = put_cmsg(msg, SOL_RXRPC, RXRPC_LOCAL_ERROR, 4, &tmp);
		break;
	default:
<<<<<<< HEAD
		pr_err("Invalid terminal call state %u\n", call->completion);
=======
		pr_err("Invalid terminal call state %u\n", call->state);
>>>>>>> b7ba80a49124 (Commit)
		BUG();
		break;
	}

<<<<<<< HEAD
	trace_rxrpc_recvdata(call, rxrpc_recvmsg_terminal,
			     call->ackr_window - 1,
			     call->rx_pkt_offset, call->rx_pkt_len, ret);
=======
	trace_rxrpc_recvmsg(call, rxrpc_recvmsg_terminal, call->rx_hard_ack,
			    call->rx_pkt_offset, call->rx_pkt_len, ret);
>>>>>>> b7ba80a49124 (Commit)
	return ret;
}

/*
<<<<<<< HEAD
=======
 * End the packet reception phase.
 */
static void rxrpc_end_rx_phase(struct rxrpc_call *call, rxrpc_serial_t serial)
{
	_enter("%d,%s", call->debug_id, rxrpc_call_states[call->state]);

	trace_rxrpc_receive(call, rxrpc_receive_end, 0, call->rx_top);
	ASSERTCMP(call->rx_hard_ack, ==, call->rx_top);

	if (call->state == RXRPC_CALL_CLIENT_RECV_REPLY) {
		rxrpc_propose_ACK(call, RXRPC_ACK_IDLE, serial, false, true,
				  rxrpc_propose_ack_terminal_ack);
		//rxrpc_send_ack_packet(call, false, NULL);
	}

	write_lock_bh(&call->state_lock);

	switch (call->state) {
	case RXRPC_CALL_CLIENT_RECV_REPLY:
		__rxrpc_call_completed(call);
		write_unlock_bh(&call->state_lock);
		break;

	case RXRPC_CALL_SERVER_RECV_REQUEST:
		call->tx_phase = true;
		call->state = RXRPC_CALL_SERVER_ACK_REQUEST;
		call->expect_req_by = jiffies + MAX_JIFFY_OFFSET;
		write_unlock_bh(&call->state_lock);
		rxrpc_propose_ACK(call, RXRPC_ACK_DELAY, serial, false, true,
				  rxrpc_propose_ack_processing_op);
		break;
	default:
		write_unlock_bh(&call->state_lock);
		break;
	}
}

/*
>>>>>>> b7ba80a49124 (Commit)
 * Discard a packet we've used up and advance the Rx window by one.
 */
static void rxrpc_rotate_rx_window(struct rxrpc_call *call)
{
	struct rxrpc_skb_priv *sp;
	struct sk_buff *skb;
	rxrpc_serial_t serial;
<<<<<<< HEAD
	rxrpc_seq_t old_consumed = call->rx_consumed, tseq;
	bool last;
	int acked;

	_enter("%d", call->debug_id);

	skb = skb_dequeue(&call->recvmsg_queue);
	rxrpc_see_skb(skb, rxrpc_skb_see_rotate);

	sp = rxrpc_skb(skb);
	tseq   = sp->hdr.seq;
	serial = sp->hdr.serial;
	last   = sp->hdr.flags & RXRPC_LAST_PACKET;

	/* Barrier against rxrpc_input_data(). */
	if (after(tseq, call->rx_consumed))
		smp_store_release(&call->rx_consumed, tseq);

	rxrpc_free_skb(skb, rxrpc_skb_put_rotate);

	trace_rxrpc_receive(call, last ? rxrpc_receive_rotate_last : rxrpc_receive_rotate,
			    serial, call->rx_consumed);

	if (last)
		set_bit(RXRPC_CALL_RECVMSG_READ_ALL, &call->flags);

	/* Check to see if there's an ACK that needs sending. */
	acked = atomic_add_return(call->rx_consumed - old_consumed,
				  &call->ackr_nr_consumed);
	if (acked > 8 &&
	    !test_and_set_bit(RXRPC_CALL_RX_IS_IDLE, &call->flags))
		rxrpc_poke_call(call, rxrpc_call_poke_idle);
}

/*
 * Decrypt and verify a DATA packet.
 */
static int rxrpc_verify_data(struct rxrpc_call *call, struct sk_buff *skb)
{
	struct rxrpc_skb_priv *sp = rxrpc_skb(skb);

	if (sp->flags & RXRPC_RX_VERIFIED)
		return 0;
	return call->security->verify_packet(call, skb);
=======
	rxrpc_seq_t hard_ack, top;
	bool last = false;
	u8 subpacket;
	int ix;

	_enter("%d", call->debug_id);

	hard_ack = call->rx_hard_ack;
	top = smp_load_acquire(&call->rx_top);
	ASSERT(before(hard_ack, top));

	hard_ack++;
	ix = hard_ack & RXRPC_RXTX_BUFF_MASK;
	skb = call->rxtx_buffer[ix];
	rxrpc_see_skb(skb, rxrpc_skb_rotated);
	sp = rxrpc_skb(skb);

	subpacket = call->rxtx_annotations[ix] & RXRPC_RX_ANNO_SUBPACKET;
	serial = sp->hdr.serial + subpacket;

	if (subpacket == sp->nr_subpackets - 1 &&
	    sp->rx_flags & RXRPC_SKB_INCL_LAST)
		last = true;

	call->rxtx_buffer[ix] = NULL;
	call->rxtx_annotations[ix] = 0;
	/* Barrier against rxrpc_input_data(). */
	smp_store_release(&call->rx_hard_ack, hard_ack);

	rxrpc_free_skb(skb, rxrpc_skb_freed);

	trace_rxrpc_receive(call, rxrpc_receive_rotate, serial, hard_ack);
	if (last) {
		rxrpc_end_rx_phase(call, serial);
	} else {
		/* Check to see if there's an ACK that needs sending. */
		if (atomic_inc_return(&call->ackr_nr_consumed) > 2)
			rxrpc_propose_ACK(call, RXRPC_ACK_IDLE, serial,
					  true, false,
					  rxrpc_propose_ack_rotate_rx);
		if (call->ackr_reason && call->ackr_reason != RXRPC_ACK_DELAY)
			rxrpc_send_ack_packet(call, false, NULL);
	}
}

/*
 * Decrypt and verify a (sub)packet.  The packet's length may be changed due to
 * padding, but if this is the case, the packet length will be resident in the
 * socket buffer.  Note that we can't modify the master skb info as the skb may
 * be the home to multiple subpackets.
 */
static int rxrpc_verify_packet(struct rxrpc_call *call, struct sk_buff *skb,
			       u8 annotation,
			       unsigned int offset, unsigned int len)
{
	struct rxrpc_skb_priv *sp = rxrpc_skb(skb);
	rxrpc_seq_t seq = sp->hdr.seq;
	u16 cksum = sp->hdr.cksum;
	u8 subpacket = annotation & RXRPC_RX_ANNO_SUBPACKET;

	_enter("");

	/* For all but the head jumbo subpacket, the security checksum is in a
	 * jumbo header immediately prior to the data.
	 */
	if (subpacket > 0) {
		__be16 tmp;
		if (skb_copy_bits(skb, offset - 2, &tmp, 2) < 0)
			BUG();
		cksum = ntohs(tmp);
		seq += subpacket;
	}

	return call->security->verify_packet(call, skb, offset, len,
					     seq, cksum);
}

/*
 * Locate the data within a packet.  This is complicated by:
 *
 * (1) An skb may contain a jumbo packet - so we have to find the appropriate
 *     subpacket.
 *
 * (2) The (sub)packets may be encrypted and, if so, the encrypted portion
 *     contains an extra header which includes the true length of the data,
 *     excluding any encrypted padding.
 */
static int rxrpc_locate_data(struct rxrpc_call *call, struct sk_buff *skb,
			     u8 *_annotation,
			     unsigned int *_offset, unsigned int *_len,
			     bool *_last)
{
	struct rxrpc_skb_priv *sp = rxrpc_skb(skb);
	unsigned int offset = sizeof(struct rxrpc_wire_header);
	unsigned int len;
	bool last = false;
	int ret;
	u8 annotation = *_annotation;
	u8 subpacket = annotation & RXRPC_RX_ANNO_SUBPACKET;

	/* Locate the subpacket */
	offset += subpacket * RXRPC_JUMBO_SUBPKTLEN;
	len = skb->len - offset;
	if (subpacket < sp->nr_subpackets - 1)
		len = RXRPC_JUMBO_DATALEN;
	else if (sp->rx_flags & RXRPC_SKB_INCL_LAST)
		last = true;

	if (!(annotation & RXRPC_RX_ANNO_VERIFIED)) {
		ret = rxrpc_verify_packet(call, skb, annotation, offset, len);
		if (ret < 0)
			return ret;
		*_annotation |= RXRPC_RX_ANNO_VERIFIED;
	}

	*_offset = offset;
	*_len = len;
	*_last = last;
	call->security->locate_data(call, skb, _offset, _len);
	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

/*
 * Deliver messages to a call.  This keeps processing packets until the buffer
 * is filled and we find either more DATA (returns 0) or the end of the DATA
<<<<<<< HEAD
 * (returns 1).  If more packets are required, it returns -EAGAIN and if the
 * call has failed it returns -EIO.
=======
 * (returns 1).  If more packets are required, it returns -EAGAIN.
>>>>>>> b7ba80a49124 (Commit)
 */
static int rxrpc_recvmsg_data(struct socket *sock, struct rxrpc_call *call,
			      struct msghdr *msg, struct iov_iter *iter,
			      size_t len, int flags, size_t *_offset)
{
	struct rxrpc_skb_priv *sp;
	struct sk_buff *skb;
<<<<<<< HEAD
	rxrpc_seq_t seq = 0;
	size_t remain;
	unsigned int rx_pkt_offset, rx_pkt_len;
	int copy, ret = -EAGAIN, ret2;

	rx_pkt_offset = call->rx_pkt_offset;
	rx_pkt_len = call->rx_pkt_len;

	if (rxrpc_call_has_failed(call)) {
		seq = call->ackr_window - 1;
		ret = -EIO;
		goto done;
	}

	if (test_bit(RXRPC_CALL_RECVMSG_READ_ALL, &call->flags)) {
		seq = call->ackr_window - 1;
=======
	rxrpc_serial_t serial;
	rxrpc_seq_t hard_ack, top, seq;
	size_t remain;
	bool rx_pkt_last;
	unsigned int rx_pkt_offset, rx_pkt_len;
	int ix, copy, ret = -EAGAIN, ret2;

	if (test_and_clear_bit(RXRPC_CALL_RX_UNDERRUN, &call->flags) &&
	    call->ackr_reason)
		rxrpc_send_ack_packet(call, false, NULL);

	rx_pkt_offset = call->rx_pkt_offset;
	rx_pkt_len = call->rx_pkt_len;
	rx_pkt_last = call->rx_pkt_last;

	if (call->state >= RXRPC_CALL_SERVER_ACK_REQUEST) {
		seq = call->rx_hard_ack;
>>>>>>> b7ba80a49124 (Commit)
		ret = 1;
		goto done;
	}

<<<<<<< HEAD
	/* No one else can be removing stuff from the queue, so we shouldn't
	 * need the Rx lock to walk it.
	 */
	skb = skb_peek(&call->recvmsg_queue);
	while (skb) {
		rxrpc_see_skb(skb, rxrpc_skb_see_recvmsg);
		sp = rxrpc_skb(skb);
		seq = sp->hdr.seq;

		if (!(flags & MSG_PEEK))
			trace_rxrpc_receive(call, rxrpc_receive_front,
					    sp->hdr.serial, seq);
=======
	/* Barriers against rxrpc_input_data(). */
	hard_ack = call->rx_hard_ack;
	seq = hard_ack + 1;

	while (top = smp_load_acquire(&call->rx_top),
	       before_eq(seq, top)
	       ) {
		ix = seq & RXRPC_RXTX_BUFF_MASK;
		skb = call->rxtx_buffer[ix];
		if (!skb) {
			trace_rxrpc_recvmsg(call, rxrpc_recvmsg_hole, seq,
					    rx_pkt_offset, rx_pkt_len, 0);
			break;
		}
		smp_rmb();
		rxrpc_see_skb(skb, rxrpc_skb_seen);
		sp = rxrpc_skb(skb);

		if (!(flags & MSG_PEEK)) {
			serial = sp->hdr.serial;
			serial += call->rxtx_annotations[ix] & RXRPC_RX_ANNO_SUBPACKET;
			trace_rxrpc_receive(call, rxrpc_receive_front,
					    serial, seq);
		}
>>>>>>> b7ba80a49124 (Commit)

		if (msg)
			sock_recv_timestamp(msg, sock->sk, skb);

		if (rx_pkt_offset == 0) {
<<<<<<< HEAD
			ret2 = rxrpc_verify_data(call, skb);
			trace_rxrpc_recvdata(call, rxrpc_recvmsg_next, seq,
					     sp->offset, sp->len, ret2);
			if (ret2 < 0) {
				kdebug("verify = %d", ret2);
				ret = ret2;
				goto out;
			}
			rx_pkt_offset = sp->offset;
			rx_pkt_len = sp->len;
		} else {
			trace_rxrpc_recvdata(call, rxrpc_recvmsg_cont, seq,
					     rx_pkt_offset, rx_pkt_len, 0);
=======
			ret2 = rxrpc_locate_data(call, skb,
						 &call->rxtx_annotations[ix],
						 &rx_pkt_offset, &rx_pkt_len,
						 &rx_pkt_last);
			trace_rxrpc_recvmsg(call, rxrpc_recvmsg_next, seq,
					    rx_pkt_offset, rx_pkt_len, ret2);
			if (ret2 < 0) {
				ret = ret2;
				goto out;
			}
		} else {
			trace_rxrpc_recvmsg(call, rxrpc_recvmsg_cont, seq,
					    rx_pkt_offset, rx_pkt_len, 0);
>>>>>>> b7ba80a49124 (Commit)
		}

		/* We have to handle short, empty and used-up DATA packets. */
		remain = len - *_offset;
		copy = rx_pkt_len;
		if (copy > remain)
			copy = remain;
		if (copy > 0) {
			ret2 = skb_copy_datagram_iter(skb, rx_pkt_offset, iter,
						      copy);
			if (ret2 < 0) {
				ret = ret2;
				goto out;
			}

			/* handle piecemeal consumption of data packets */
			rx_pkt_offset += copy;
			rx_pkt_len -= copy;
			*_offset += copy;
		}

		if (rx_pkt_len > 0) {
<<<<<<< HEAD
			trace_rxrpc_recvdata(call, rxrpc_recvmsg_full, seq,
					     rx_pkt_offset, rx_pkt_len, 0);
=======
			trace_rxrpc_recvmsg(call, rxrpc_recvmsg_full, seq,
					    rx_pkt_offset, rx_pkt_len, 0);
>>>>>>> b7ba80a49124 (Commit)
			ASSERTCMP(*_offset, ==, len);
			ret = 0;
			break;
		}

		/* The whole packet has been transferred. */
<<<<<<< HEAD
		if (sp->hdr.flags & RXRPC_LAST_PACKET)
			ret = 1;
		rx_pkt_offset = 0;
		rx_pkt_len = 0;

		skb = skb_peek_next(skb, &call->recvmsg_queue);

		if (!(flags & MSG_PEEK))
			rxrpc_rotate_rx_window(call);
=======
		if (!(flags & MSG_PEEK))
			rxrpc_rotate_rx_window(call);
		rx_pkt_offset = 0;
		rx_pkt_len = 0;

		if (rx_pkt_last) {
			ASSERTCMP(seq, ==, READ_ONCE(call->rx_top));
			ret = 1;
			goto out;
		}

		seq++;
>>>>>>> b7ba80a49124 (Commit)
	}

out:
	if (!(flags & MSG_PEEK)) {
		call->rx_pkt_offset = rx_pkt_offset;
		call->rx_pkt_len = rx_pkt_len;
<<<<<<< HEAD
	}
done:
	trace_rxrpc_recvdata(call, rxrpc_recvmsg_data_return, seq,
			     rx_pkt_offset, rx_pkt_len, ret);
	if (ret == -EAGAIN)
		set_bit(RXRPC_CALL_RX_IS_IDLE, &call->flags);
=======
		call->rx_pkt_last = rx_pkt_last;
	}
done:
	trace_rxrpc_recvmsg(call, rxrpc_recvmsg_data_return, seq,
			    rx_pkt_offset, rx_pkt_len, ret);
	if (ret == -EAGAIN)
		set_bit(RXRPC_CALL_RX_UNDERRUN, &call->flags);
>>>>>>> b7ba80a49124 (Commit)
	return ret;
}

/*
 * Receive a message from an RxRPC socket
 * - we need to be careful about two or more threads calling recvmsg
 *   simultaneously
 */
int rxrpc_recvmsg(struct socket *sock, struct msghdr *msg, size_t len,
		  int flags)
{
	struct rxrpc_call *call;
	struct rxrpc_sock *rx = rxrpc_sk(sock->sk);
	struct list_head *l;
<<<<<<< HEAD
	unsigned int call_debug_id = 0;
=======
>>>>>>> b7ba80a49124 (Commit)
	size_t copied = 0;
	long timeo;
	int ret;

	DEFINE_WAIT(wait);

<<<<<<< HEAD
	trace_rxrpc_recvmsg(0, rxrpc_recvmsg_enter, 0);
=======
	trace_rxrpc_recvmsg(NULL, rxrpc_recvmsg_enter, 0, 0, 0, 0);
>>>>>>> b7ba80a49124 (Commit)

	if (flags & (MSG_OOB | MSG_TRUNC))
		return -EOPNOTSUPP;

	timeo = sock_rcvtimeo(&rx->sk, flags & MSG_DONTWAIT);

try_again:
	lock_sock(&rx->sk);

	/* Return immediately if a client socket has no outstanding calls */
	if (RB_EMPTY_ROOT(&rx->calls) &&
	    list_empty(&rx->recvmsg_q) &&
	    rx->sk.sk_state != RXRPC_SERVER_LISTENING) {
		release_sock(&rx->sk);
		return -EAGAIN;
	}

	if (list_empty(&rx->recvmsg_q)) {
		ret = -EWOULDBLOCK;
		if (timeo == 0) {
			call = NULL;
			goto error_no_call;
		}

		release_sock(&rx->sk);

		/* Wait for something to happen */
		prepare_to_wait_exclusive(sk_sleep(&rx->sk), &wait,
					  TASK_INTERRUPTIBLE);
		ret = sock_error(&rx->sk);
		if (ret)
			goto wait_error;

		if (list_empty(&rx->recvmsg_q)) {
			if (signal_pending(current))
				goto wait_interrupted;
<<<<<<< HEAD
			trace_rxrpc_recvmsg(0, rxrpc_recvmsg_wait, 0);
=======
			trace_rxrpc_recvmsg(NULL, rxrpc_recvmsg_wait,
					    0, 0, 0, 0);
>>>>>>> b7ba80a49124 (Commit)
			timeo = schedule_timeout(timeo);
		}
		finish_wait(sk_sleep(&rx->sk), &wait);
		goto try_again;
	}

	/* Find the next call and dequeue it if we're not just peeking.  If we
	 * do dequeue it, that comes with a ref that we will need to release.
<<<<<<< HEAD
	 * We also want to weed out calls that got requeued whilst we were
	 * shovelling data out.
	 */
	spin_lock(&rx->recvmsg_lock);
	l = rx->recvmsg_q.next;
	call = list_entry(l, struct rxrpc_call, recvmsg_link);

	if (!rxrpc_call_is_complete(call) &&
	    skb_queue_empty(&call->recvmsg_queue)) {
		list_del_init(&call->recvmsg_link);
		spin_unlock(&rx->recvmsg_lock);
		release_sock(&rx->sk);
		trace_rxrpc_recvmsg(call->debug_id, rxrpc_recvmsg_unqueue, 0);
		rxrpc_put_call(call, rxrpc_call_put_recvmsg);
		goto try_again;
	}

	if (!(flags & MSG_PEEK))
		list_del_init(&call->recvmsg_link);
	else
		rxrpc_get_call(call, rxrpc_call_get_recvmsg);
	spin_unlock(&rx->recvmsg_lock);

	call_debug_id = call->debug_id;
	trace_rxrpc_recvmsg(call_debug_id, rxrpc_recvmsg_dequeue, 0);
=======
	 */
	write_lock_bh(&rx->recvmsg_lock);
	l = rx->recvmsg_q.next;
	call = list_entry(l, struct rxrpc_call, recvmsg_link);
	if (!(flags & MSG_PEEK))
		list_del_init(&call->recvmsg_link);
	else
		rxrpc_get_call(call, rxrpc_call_got);
	write_unlock_bh(&rx->recvmsg_lock);

	trace_rxrpc_recvmsg(call, rxrpc_recvmsg_dequeue, 0, 0, 0, 0);
>>>>>>> b7ba80a49124 (Commit)

	/* We're going to drop the socket lock, so we need to lock the call
	 * against interference by sendmsg.
	 */
	if (!mutex_trylock(&call->user_mutex)) {
		ret = -EWOULDBLOCK;
		if (flags & MSG_DONTWAIT)
			goto error_requeue_call;
		ret = -ERESTARTSYS;
		if (mutex_lock_interruptible(&call->user_mutex) < 0)
			goto error_requeue_call;
	}

	release_sock(&rx->sk);

	if (test_bit(RXRPC_CALL_RELEASED, &call->flags))
		BUG();

	if (test_bit(RXRPC_CALL_HAS_USERID, &call->flags)) {
		if (flags & MSG_CMSG_COMPAT) {
			unsigned int id32 = call->user_call_ID;

			ret = put_cmsg(msg, SOL_RXRPC, RXRPC_USER_CALL_ID,
				       sizeof(unsigned int), &id32);
		} else {
			unsigned long idl = call->user_call_ID;

			ret = put_cmsg(msg, SOL_RXRPC, RXRPC_USER_CALL_ID,
				       sizeof(unsigned long), &idl);
		}
		if (ret < 0)
			goto error_unlock_call;
	}

	if (msg->msg_name && call->peer) {
<<<<<<< HEAD
		size_t len = sizeof(call->dest_srx);

		memcpy(msg->msg_name, &call->dest_srx, len);
		msg->msg_namelen = len;
	}

	ret = rxrpc_recvmsg_data(sock, call, msg, &msg->msg_iter, len,
				 flags, &copied);
	if (ret == -EAGAIN)
		ret = 0;
	if (ret == -EIO)
		goto call_failed;
	if (ret < 0)
		goto error_unlock_call;

	if (rxrpc_call_is_complete(call) &&
	    skb_queue_empty(&call->recvmsg_queue))
		goto call_complete;
	if (rxrpc_call_has_failed(call))
		goto call_failed;

	if (!skb_queue_empty(&call->recvmsg_queue))
		rxrpc_notify_socket(call);
	goto not_yet_complete;

call_failed:
	rxrpc_purge_queue(&call->recvmsg_queue);
call_complete:
	ret = rxrpc_recvmsg_term(call, msg);
	if (ret < 0)
		goto error_unlock_call;
	if (!(flags & MSG_PEEK))
		rxrpc_release_call(rx, call);
	msg->msg_flags |= MSG_EOR;
	ret = 1;

not_yet_complete:
=======
		struct sockaddr_rxrpc *srx = msg->msg_name;
		size_t len = sizeof(call->peer->srx);

		memcpy(msg->msg_name, &call->peer->srx, len);
		srx->srx_service = call->service_id;
		msg->msg_namelen = len;
	}

	switch (READ_ONCE(call->state)) {
	case RXRPC_CALL_CLIENT_RECV_REPLY:
	case RXRPC_CALL_SERVER_RECV_REQUEST:
	case RXRPC_CALL_SERVER_ACK_REQUEST:
		ret = rxrpc_recvmsg_data(sock, call, msg, &msg->msg_iter, len,
					 flags, &copied);
		if (ret == -EAGAIN)
			ret = 0;

		if (after(call->rx_top, call->rx_hard_ack) &&
		    call->rxtx_buffer[(call->rx_hard_ack + 1) & RXRPC_RXTX_BUFF_MASK])
			rxrpc_notify_socket(call);
		break;
	default:
		ret = 0;
		break;
	}

	if (ret < 0)
		goto error_unlock_call;

	if (call->state == RXRPC_CALL_COMPLETE) {
		ret = rxrpc_recvmsg_term(call, msg);
		if (ret < 0)
			goto error_unlock_call;
		if (!(flags & MSG_PEEK))
			rxrpc_release_call(rx, call);
		msg->msg_flags |= MSG_EOR;
		ret = 1;
	}

>>>>>>> b7ba80a49124 (Commit)
	if (ret == 0)
		msg->msg_flags |= MSG_MORE;
	else
		msg->msg_flags &= ~MSG_MORE;
	ret = copied;

error_unlock_call:
	mutex_unlock(&call->user_mutex);
<<<<<<< HEAD
	rxrpc_put_call(call, rxrpc_call_put_recvmsg);
	trace_rxrpc_recvmsg(call_debug_id, rxrpc_recvmsg_return, ret);
=======
	rxrpc_put_call(call, rxrpc_call_put);
	trace_rxrpc_recvmsg(call, rxrpc_recvmsg_return, 0, 0, 0, ret);
>>>>>>> b7ba80a49124 (Commit)
	return ret;

error_requeue_call:
	if (!(flags & MSG_PEEK)) {
<<<<<<< HEAD
		spin_lock(&rx->recvmsg_lock);
		list_add(&call->recvmsg_link, &rx->recvmsg_q);
		spin_unlock(&rx->recvmsg_lock);
		trace_rxrpc_recvmsg(call_debug_id, rxrpc_recvmsg_requeue, 0);
	} else {
		rxrpc_put_call(call, rxrpc_call_put_recvmsg);
=======
		write_lock_bh(&rx->recvmsg_lock);
		list_add(&call->recvmsg_link, &rx->recvmsg_q);
		write_unlock_bh(&rx->recvmsg_lock);
		trace_rxrpc_recvmsg(call, rxrpc_recvmsg_requeue, 0, 0, 0, 0);
	} else {
		rxrpc_put_call(call, rxrpc_call_put);
>>>>>>> b7ba80a49124 (Commit)
	}
error_no_call:
	release_sock(&rx->sk);
error_trace:
<<<<<<< HEAD
	trace_rxrpc_recvmsg(call_debug_id, rxrpc_recvmsg_return, ret);
=======
	trace_rxrpc_recvmsg(call, rxrpc_recvmsg_return, 0, 0, 0, ret);
>>>>>>> b7ba80a49124 (Commit)
	return ret;

wait_interrupted:
	ret = sock_intr_errno(timeo);
wait_error:
	finish_wait(sk_sleep(&rx->sk), &wait);
	call = NULL;
	goto error_trace;
}

/**
 * rxrpc_kernel_recv_data - Allow a kernel service to receive data/info
 * @sock: The socket that the call exists on
 * @call: The call to send data through
 * @iter: The buffer to receive into
 * @_len: The amount of data we want to receive (decreased on return)
 * @want_more: True if more data is expected to be read
 * @_abort: Where the abort code is stored if -ECONNABORTED is returned
 * @_service: Where to store the actual service ID (may be upgraded)
 *
 * Allow a kernel service to receive data and pick up information about the
 * state of a call.  Returns 0 if got what was asked for and there's more
 * available, 1 if we got what was asked for and we're at the end of the data
 * and -EAGAIN if we need more data.
 *
 * Note that we may return -EAGAIN to drain empty packets at the end of the
 * data, even if we've already copied over the requested data.
 *
 * *_abort should also be initialised to 0.
 */
int rxrpc_kernel_recv_data(struct socket *sock, struct rxrpc_call *call,
			   struct iov_iter *iter, size_t *_len,
			   bool want_more, u32 *_abort, u16 *_service)
{
	size_t offset = 0;
	int ret;

<<<<<<< HEAD
	_enter("{%d},%zu,%d", call->debug_id, *_len, want_more);

	mutex_lock(&call->user_mutex);

	ret = rxrpc_recvmsg_data(sock, call, NULL, iter, *_len, 0, &offset);
	*_len -= offset;
	if (ret == -EIO)
		goto call_failed;
	if (ret < 0)
		goto out;

	/* We can only reach here with a partially full buffer if we have
	 * reached the end of the data.  We must otherwise have a full buffer
	 * or have been given -EAGAIN.
	 */
	if (ret == 1) {
		if (iov_iter_count(iter) > 0)
			goto short_data;
		if (!want_more)
			goto read_phase_complete;
		ret = 0;
		goto out;
	}

	if (!want_more)
		goto excess_data;
	goto out;

read_phase_complete:
	ret = 1;
out:
	if (_service)
		*_service = call->dest_srx.srx_service;
=======
	_enter("{%d,%s},%zu,%d",
	       call->debug_id, rxrpc_call_states[call->state],
	       *_len, want_more);

	ASSERTCMP(call->state, !=, RXRPC_CALL_SERVER_SECURING);

	mutex_lock(&call->user_mutex);

	switch (READ_ONCE(call->state)) {
	case RXRPC_CALL_CLIENT_RECV_REPLY:
	case RXRPC_CALL_SERVER_RECV_REQUEST:
	case RXRPC_CALL_SERVER_ACK_REQUEST:
		ret = rxrpc_recvmsg_data(sock, call, NULL, iter,
					 *_len, 0, &offset);
		*_len -= offset;
		if (ret < 0)
			goto out;

		/* We can only reach here with a partially full buffer if we
		 * have reached the end of the data.  We must otherwise have a
		 * full buffer or have been given -EAGAIN.
		 */
		if (ret == 1) {
			if (iov_iter_count(iter) > 0)
				goto short_data;
			if (!want_more)
				goto read_phase_complete;
			ret = 0;
			goto out;
		}

		if (!want_more)
			goto excess_data;
		goto out;

	case RXRPC_CALL_COMPLETE:
		goto call_complete;

	default:
		ret = -EINPROGRESS;
		goto out;
	}

read_phase_complete:
	ret = 1;
out:
	switch (call->ackr_reason) {
	case RXRPC_ACK_IDLE:
		break;
	case RXRPC_ACK_DELAY:
		if (ret != -EAGAIN)
			break;
		fallthrough;
	default:
		rxrpc_send_ack_packet(call, false, NULL);
	}

	if (_service)
		*_service = call->service_id;
>>>>>>> b7ba80a49124 (Commit)
	mutex_unlock(&call->user_mutex);
	_leave(" = %d [%zu,%d]", ret, iov_iter_count(iter), *_abort);
	return ret;

short_data:
<<<<<<< HEAD
	trace_rxrpc_abort(call->debug_id, rxrpc_recvmsg_short_data,
			  call->cid, call->call_id, call->rx_consumed,
			  0, -EBADMSG);
	ret = -EBADMSG;
	goto out;
excess_data:
	trace_rxrpc_abort(call->debug_id, rxrpc_recvmsg_excess_data,
			  call->cid, call->call_id, call->rx_consumed,
			  0, -EMSGSIZE);
	ret = -EMSGSIZE;
	goto out;
call_failed:
=======
	trace_rxrpc_rx_eproto(call, 0, tracepoint_string("short_data"));
	ret = -EBADMSG;
	goto out;
excess_data:
	trace_rxrpc_rx_eproto(call, 0, tracepoint_string("excess_data"));
	ret = -EMSGSIZE;
	goto out;
call_complete:
>>>>>>> b7ba80a49124 (Commit)
	*_abort = call->abort_code;
	ret = call->error;
	if (call->completion == RXRPC_CALL_SUCCEEDED) {
		ret = 1;
		if (iov_iter_count(iter) > 0)
			ret = -ECONNRESET;
	}
	goto out;
}
EXPORT_SYMBOL(rxrpc_kernel_recv_data);
