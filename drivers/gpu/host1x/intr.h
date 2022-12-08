/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Tegra host1x Interrupt Management
 *
<<<<<<< HEAD
 * Copyright (c) 2010-2021, NVIDIA Corporation.
=======
 * Copyright (c) 2010-2013, NVIDIA Corporation.
>>>>>>> b7ba80a49124 (Commit)
 */

#ifndef __HOST1X_INTR_H
#define __HOST1X_INTR_H

<<<<<<< HEAD
struct host1x;
struct host1x_syncpt_fence;

/* Initialize host1x sync point interrupt */
int host1x_intr_init(struct host1x *host);
=======
#include <linux/interrupt.h>
#include <linux/workqueue.h>

struct host1x_syncpt;
struct host1x;

enum host1x_intr_action {
	/*
	 * Perform cleanup after a submit has completed.
	 * 'data' points to a channel
	 */
	HOST1X_INTR_ACTION_SUBMIT_COMPLETE = 0,

	/*
	 * Wake up a  task.
	 * 'data' points to a wait_queue_head_t
	 */
	HOST1X_INTR_ACTION_WAKEUP,

	/*
	 * Wake up a interruptible task.
	 * 'data' points to a wait_queue_head_t
	 */
	HOST1X_INTR_ACTION_WAKEUP_INTERRUPTIBLE,

	HOST1X_INTR_ACTION_SIGNAL_FENCE,

	HOST1X_INTR_ACTION_COUNT
};

struct host1x_syncpt_intr {
	spinlock_t lock;
	struct list_head wait_head;
	char thresh_irq_name[12];
	struct work_struct work;
};

struct host1x_waitlist {
	struct list_head list;
	struct kref refcount;
	u32 thresh;
	enum host1x_intr_action action;
	atomic_t state;
	void *data;
	int count;
};

/*
 * Schedule an action to be taken when a sync point reaches the given threshold.
 *
 * @id the sync point
 * @thresh the threshold
 * @action the action to take
 * @data a pointer to extra data depending on action, see above
 * @waiter waiter structure - assumes ownership
 * @ref must be passed if cancellation is possible, else NULL
 *
 * This is a non-blocking api.
 */
int host1x_intr_add_action(struct host1x *host, struct host1x_syncpt *syncpt,
			   u32 thresh, enum host1x_intr_action action,
			   void *data, struct host1x_waitlist *waiter,
			   void **ref);

/*
 * Unreference an action submitted to host1x_intr_add_action().
 * You must call this if you passed non-NULL as ref.
 * @ref the ref returned from host1x_intr_add_action()
 * @flush wait until any pending handlers have completed before returning.
 */
void host1x_intr_put_ref(struct host1x *host, unsigned int id, void *ref,
			 bool flush);

/* Initialize host1x sync point interrupt */
int host1x_intr_init(struct host1x *host, unsigned int irq_sync);
>>>>>>> b7ba80a49124 (Commit)

/* Deinitialize host1x sync point interrupt */
void host1x_intr_deinit(struct host1x *host);

/* Enable host1x sync point interrupt */
void host1x_intr_start(struct host1x *host);

/* Disable host1x sync point interrupt */
void host1x_intr_stop(struct host1x *host);

<<<<<<< HEAD
void host1x_intr_handle_interrupt(struct host1x *host, unsigned int id);

void host1x_intr_add_fence_locked(struct host1x *host, struct host1x_syncpt_fence *fence);

bool host1x_intr_remove_fence(struct host1x *host, struct host1x_syncpt_fence *fence);

=======
irqreturn_t host1x_syncpt_thresh_fn(void *dev_id);
>>>>>>> b7ba80a49124 (Commit)
#endif
