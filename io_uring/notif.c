#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/file.h>
#include <linux/slab.h>
#include <linux/net.h>
#include <linux/io_uring.h>

#include "io_uring.h"
#include "notif.h"
#include "rsrc.h"

<<<<<<< HEAD
static void io_notif_complete_tw_ext(struct io_kiocb *notif, bool *locked)
=======
static void __io_notif_complete_tw(struct io_kiocb *notif, bool *locked)
>>>>>>> b7ba80a49124 (Commit)
{
	struct io_notif_data *nd = io_notif_to_data(notif);
	struct io_ring_ctx *ctx = notif->ctx;

<<<<<<< HEAD
	if (nd->zc_report && (nd->zc_copied || !nd->zc_used))
		notif->cqe.res |= IORING_NOTIF_USAGE_ZC_COPIED;

=======
>>>>>>> b7ba80a49124 (Commit)
	if (nd->account_pages && ctx->user) {
		__io_unaccount_mem(ctx->user, nd->account_pages);
		nd->account_pages = 0;
	}
	io_req_task_complete(notif, locked);
}

<<<<<<< HEAD
static void io_tx_ubuf_callback(struct sk_buff *skb, struct ubuf_info *uarg,
				bool success)
=======
static void io_uring_tx_zerocopy_callback(struct sk_buff *skb,
					  struct ubuf_info *uarg,
					  bool success)
>>>>>>> b7ba80a49124 (Commit)
{
	struct io_notif_data *nd = container_of(uarg, struct io_notif_data, uarg);
	struct io_kiocb *notif = cmd_to_io_kiocb(nd);

<<<<<<< HEAD
	if (refcount_dec_and_test(&uarg->refcnt))
		io_req_task_work_add(notif);
}

static void io_tx_ubuf_callback_ext(struct sk_buff *skb, struct ubuf_info *uarg,
			     bool success)
{
	struct io_notif_data *nd = container_of(uarg, struct io_notif_data, uarg);

	if (nd->zc_report) {
		if (success && !nd->zc_used && skb)
			WRITE_ONCE(nd->zc_used, true);
		else if (!success && !nd->zc_copied)
			WRITE_ONCE(nd->zc_copied, true);
	}
	io_tx_ubuf_callback(skb, uarg, success);
}

void io_notif_set_extended(struct io_kiocb *notif)
{
	struct io_notif_data *nd = io_notif_to_data(notif);

	if (nd->uarg.callback != io_tx_ubuf_callback_ext) {
		nd->account_pages = 0;
		nd->zc_report = false;
		nd->zc_used = false;
		nd->zc_copied = false;
		nd->uarg.callback = io_tx_ubuf_callback_ext;
		notif->io_task_work.func = io_notif_complete_tw_ext;
=======
	if (refcount_dec_and_test(&uarg->refcnt)) {
		notif->io_task_work.func = __io_notif_complete_tw;
		io_req_task_work_add(notif);
>>>>>>> b7ba80a49124 (Commit)
	}
}

struct io_kiocb *io_alloc_notif(struct io_ring_ctx *ctx)
	__must_hold(&ctx->uring_lock)
{
	struct io_kiocb *notif;
	struct io_notif_data *nd;

<<<<<<< HEAD
	if (unlikely(!io_alloc_req(ctx, &notif)))
		return NULL;
=======
	if (unlikely(!io_alloc_req_refill(ctx)))
		return NULL;
	notif = io_alloc_req(ctx);
>>>>>>> b7ba80a49124 (Commit)
	notif->opcode = IORING_OP_NOP;
	notif->flags = 0;
	notif->file = NULL;
	notif->task = current;
	io_get_task_refs(1);
	notif->rsrc_node = NULL;
<<<<<<< HEAD
	notif->io_task_work.func = io_req_task_complete;

	nd = io_notif_to_data(notif);
	nd->uarg.flags = SKBFL_ZEROCOPY_FRAG | SKBFL_DONT_ORPHAN;
	nd->uarg.callback = io_tx_ubuf_callback;
	refcount_set(&nd->uarg.refcnt, 1);
	return notif;
}
=======
	io_req_set_rsrc_node(notif, ctx, 0);

	nd = io_notif_to_data(notif);
	nd->account_pages = 0;
	nd->uarg.flags = SKBFL_ZEROCOPY_FRAG | SKBFL_DONT_ORPHAN;
	nd->uarg.callback = io_uring_tx_zerocopy_callback;
	refcount_set(&nd->uarg.refcnt, 1);
	return notif;
}

void io_notif_flush(struct io_kiocb *notif)
	__must_hold(&slot->notif->ctx->uring_lock)
{
	struct io_notif_data *nd = io_notif_to_data(notif);

	/* drop slot's master ref */
	if (refcount_dec_and_test(&nd->uarg.refcnt)) {
		notif->io_task_work.func = __io_notif_complete_tw;
		io_req_task_work_add(notif);
	}
}
>>>>>>> b7ba80a49124 (Commit)
