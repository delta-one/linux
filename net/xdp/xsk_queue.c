// SPDX-License-Identifier: GPL-2.0
/* XDP user-space ring structure
 * Copyright(c) 2018 Intel Corporation.
 */

#include <linux/log2.h>
#include <linux/slab.h>
#include <linux/overflow.h>
<<<<<<< HEAD
#include <linux/vmalloc.h>
=======
>>>>>>> b7ba80a49124 (Commit)
#include <net/xdp_sock_drv.h>

#include "xsk_queue.h"

static size_t xskq_get_ring_size(struct xsk_queue *q, bool umem_queue)
{
	struct xdp_umem_ring *umem_ring;
	struct xdp_rxtx_ring *rxtx_ring;

	if (umem_queue)
		return struct_size(umem_ring, desc, q->nentries);
	return struct_size(rxtx_ring, desc, q->nentries);
}

struct xsk_queue *xskq_create(u32 nentries, bool umem_queue)
{
	struct xsk_queue *q;
<<<<<<< HEAD
=======
	gfp_t gfp_flags;
>>>>>>> b7ba80a49124 (Commit)
	size_t size;

	q = kzalloc(sizeof(*q), GFP_KERNEL);
	if (!q)
		return NULL;

	q->nentries = nentries;
	q->ring_mask = nentries - 1;

<<<<<<< HEAD
	size = xskq_get_ring_size(q, umem_queue);
	size = PAGE_ALIGN(size);

	q->ring = vmalloc_user(size);
=======
	gfp_flags = GFP_KERNEL | __GFP_ZERO | __GFP_NOWARN |
		    __GFP_COMP  | __GFP_NORETRY;
	size = xskq_get_ring_size(q, umem_queue);

	q->ring = (struct xdp_ring *)__get_free_pages(gfp_flags,
						      get_order(size));
>>>>>>> b7ba80a49124 (Commit)
	if (!q->ring) {
		kfree(q);
		return NULL;
	}

<<<<<<< HEAD
	q->ring_vmalloc_size = size;
=======
>>>>>>> b7ba80a49124 (Commit)
	return q;
}

void xskq_destroy(struct xsk_queue *q)
{
	if (!q)
		return;

<<<<<<< HEAD
	vfree(q->ring);
=======
	page_frag_free(q->ring);
>>>>>>> b7ba80a49124 (Commit)
	kfree(q);
}
