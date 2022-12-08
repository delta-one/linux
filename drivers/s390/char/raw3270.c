// SPDX-License-Identifier: GPL-2.0
/*
 * IBM/3270 Driver - core functions.
 *
 * Author(s):
 *   Original 3270 Code for 2.4 written by Richard Hitt (UTS Global)
 *   Rewritten for 2.5 by Martin Schwidefsky <schwidefsky@de.ibm.com>
 *     Copyright IBM Corp. 2003, 2009
 */

#include <linux/module.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/wait.h>

#include <asm/ccwdev.h>
#include <asm/cio.h>
#include <asm/ebcdic.h>
#include <asm/diag.h>

#include "raw3270.h"

#include <linux/major.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/mutex.h>

struct class *class3270;
<<<<<<< HEAD
EXPORT_SYMBOL(class3270);
=======
>>>>>>> b7ba80a49124 (Commit)

/* The main 3270 data structure. */
struct raw3270 {
	struct list_head list;
	struct ccw_device *cdev;
	int minor;

<<<<<<< HEAD
	int model, rows, cols;
	int old_model, old_rows, old_cols;
=======
	short model, rows, cols;
>>>>>>> b7ba80a49124 (Commit)
	unsigned int state;
	unsigned long flags;

	struct list_head req_queue;	/* Request queue. */
	struct list_head view_list;	/* List of available views. */
	struct raw3270_view *view;	/* Active view. */

	struct timer_list timer;	/* Device timer. */

	unsigned char *ascebc;		/* ascii -> ebcdic table */

	struct raw3270_view init_view;
	struct raw3270_request init_reset;
	struct raw3270_request init_readpart;
	struct raw3270_request init_readmod;
	unsigned char init_data[256];
<<<<<<< HEAD
	struct work_struct resize_work;
=======
>>>>>>> b7ba80a49124 (Commit)
};

/* raw3270->state */
#define RAW3270_STATE_INIT	0	/* Initial state */
#define RAW3270_STATE_RESET	1	/* Reset command is pending */
#define RAW3270_STATE_W4ATTN	2	/* Wait for attention interrupt */
#define RAW3270_STATE_READMOD	3	/* Read partition is pending */
#define RAW3270_STATE_READY	4	/* Device is usable by views */

/* raw3270->flags */
#define RAW3270_FLAGS_14BITADDR	0	/* 14-bit buffer addresses */
#define RAW3270_FLAGS_BUSY	1	/* Device busy, leave it alone */
#define RAW3270_FLAGS_CONSOLE	2	/* Device is the console. */

/* Semaphore to protect global data of raw3270 (devices, views, etc). */
static DEFINE_MUTEX(raw3270_mutex);

/* List of 3270 devices. */
static LIST_HEAD(raw3270_devices);

/*
 * Flag to indicate if the driver has been registered. Some operations
 * like waiting for the end of i/o need to be done differently as long
 * as the kernel is still starting up (console support).
 */
static int raw3270_registered;

/* Module parameters */
static bool tubxcorrect;
module_param(tubxcorrect, bool, 0);

/*
 * Wait queue for device init/delete, view delete.
 */
DECLARE_WAIT_QUEUE_HEAD(raw3270_wait_queue);
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_wait_queue);
=======
>>>>>>> b7ba80a49124 (Commit)

static void __raw3270_disconnect(struct raw3270 *rp);

/*
 * Encode array for 12 bit 3270 addresses.
 */
static unsigned char raw3270_ebcgraf[64] =	{
	0x40, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
	0xc8, 0xc9, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
	0x50, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
	0xd8, 0xd9, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
	0x60, 0x61, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
	0xe8, 0xe9, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
	0xf8, 0xf9, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f
};

static inline int raw3270_state_ready(struct raw3270 *rp)
{
	return rp->state == RAW3270_STATE_READY;
}

<<<<<<< HEAD
void raw3270_buffer_address(struct raw3270 *rp, char *cp, int x, int y)
{
	int addr;

	if (x < 0)
		x = max_t(int, 0, rp->view->cols + x);
	if (y < 0)
		y = max_t(int, 0, rp->view->rows + y);
	addr = (y * rp->view->cols) + x;
=======
static inline int raw3270_state_final(struct raw3270 *rp)
{
	return rp->state == RAW3270_STATE_INIT ||
		rp->state == RAW3270_STATE_READY;
}

void
raw3270_buffer_address(struct raw3270 *rp, char *cp, unsigned short addr)
{
>>>>>>> b7ba80a49124 (Commit)
	if (test_bit(RAW3270_FLAGS_14BITADDR, &rp->flags)) {
		cp[0] = (addr >> 8) & 0x3f;
		cp[1] = addr & 0xff;
	} else {
		cp[0] = raw3270_ebcgraf[(addr >> 6) & 0x3f];
		cp[1] = raw3270_ebcgraf[addr & 0x3f];
	}
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_buffer_address);
=======
>>>>>>> b7ba80a49124 (Commit)

/*
 * Allocate a new 3270 ccw request
 */
<<<<<<< HEAD
struct raw3270_request *raw3270_request_alloc(size_t size)
=======
struct raw3270_request *
raw3270_request_alloc(size_t size)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270_request *rq;

	/* Allocate request structure */
<<<<<<< HEAD
	rq = kzalloc(sizeof(*rq), GFP_KERNEL | GFP_DMA);
=======
	rq = kzalloc(sizeof(struct raw3270_request), GFP_KERNEL | GFP_DMA);
>>>>>>> b7ba80a49124 (Commit)
	if (!rq)
		return ERR_PTR(-ENOMEM);

	/* alloc output buffer. */
	if (size > 0) {
		rq->buffer = kmalloc(size, GFP_KERNEL | GFP_DMA);
		if (!rq->buffer) {
			kfree(rq);
			return ERR_PTR(-ENOMEM);
		}
	}
	rq->size = size;
	INIT_LIST_HEAD(&rq->list);

	/*
	 * Setup ccw.
	 */
	rq->ccw.cda = __pa(rq->buffer);
	rq->ccw.flags = CCW_FLAG_SLI;

	return rq;
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_request_alloc);
=======
>>>>>>> b7ba80a49124 (Commit)

/*
 * Free 3270 ccw request
 */
<<<<<<< HEAD
void raw3270_request_free(struct raw3270_request *rq)
=======
void
raw3270_request_free (struct raw3270_request *rq)
>>>>>>> b7ba80a49124 (Commit)
{
	kfree(rq->buffer);
	kfree(rq);
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_request_free);
=======
>>>>>>> b7ba80a49124 (Commit)

/*
 * Reset request to initial state.
 */
<<<<<<< HEAD
int raw3270_request_reset(struct raw3270_request *rq)
{
	if (WARN_ON_ONCE(!list_empty(&rq->list)))
		return -EBUSY;
=======
void
raw3270_request_reset(struct raw3270_request *rq)
{
	BUG_ON(!list_empty(&rq->list));
>>>>>>> b7ba80a49124 (Commit)
	rq->ccw.cmd_code = 0;
	rq->ccw.count = 0;
	rq->ccw.cda = __pa(rq->buffer);
	rq->ccw.flags = CCW_FLAG_SLI;
	rq->rescnt = 0;
	rq->rc = 0;
<<<<<<< HEAD
	return 0;
}
EXPORT_SYMBOL(raw3270_request_reset);
=======
}
>>>>>>> b7ba80a49124 (Commit)

/*
 * Set command code to ccw of a request.
 */
<<<<<<< HEAD
void raw3270_request_set_cmd(struct raw3270_request *rq, u8 cmd)
{
	rq->ccw.cmd_code = cmd;
}
EXPORT_SYMBOL(raw3270_request_set_cmd);
=======
void
raw3270_request_set_cmd(struct raw3270_request *rq, u8 cmd)
{
	rq->ccw.cmd_code = cmd;
}
>>>>>>> b7ba80a49124 (Commit)

/*
 * Add data fragment to output buffer.
 */
<<<<<<< HEAD
int raw3270_request_add_data(struct raw3270_request *rq, void *data, size_t size)
=======
int
raw3270_request_add_data(struct raw3270_request *rq, void *data, size_t size)
>>>>>>> b7ba80a49124 (Commit)
{
	if (size + rq->ccw.count > rq->size)
		return -E2BIG;
	memcpy(rq->buffer + rq->ccw.count, data, size);
	rq->ccw.count += size;
	return 0;
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_request_add_data);
=======
>>>>>>> b7ba80a49124 (Commit)

/*
 * Set address/length pair to ccw of a request.
 */
<<<<<<< HEAD
void raw3270_request_set_data(struct raw3270_request *rq, void *data, size_t size)
=======
void
raw3270_request_set_data(struct raw3270_request *rq, void *data, size_t size)
>>>>>>> b7ba80a49124 (Commit)
{
	rq->ccw.cda = __pa(data);
	rq->ccw.count = size;
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_request_set_data);
=======
>>>>>>> b7ba80a49124 (Commit)

/*
 * Set idal buffer to ccw of a request.
 */
<<<<<<< HEAD
void raw3270_request_set_idal(struct raw3270_request *rq, struct idal_buffer *ib)
=======
void
raw3270_request_set_idal(struct raw3270_request *rq, struct idal_buffer *ib)
>>>>>>> b7ba80a49124 (Commit)
{
	rq->ccw.cda = __pa(ib->data);
	rq->ccw.count = ib->size;
	rq->ccw.flags |= CCW_FLAG_IDA;
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_request_set_idal);
=======
>>>>>>> b7ba80a49124 (Commit)

/*
 * Add the request to the request queue, try to start it if the
 * 3270 device is idle. Return without waiting for end of i/o.
 */
<<<<<<< HEAD
static int __raw3270_start(struct raw3270 *rp, struct raw3270_view *view,
			   struct raw3270_request *rq)
=======
static int
__raw3270_start(struct raw3270 *rp, struct raw3270_view *view,
		struct raw3270_request *rq)
>>>>>>> b7ba80a49124 (Commit)
{
	rq->view = view;
	raw3270_get_view(view);
	if (list_empty(&rp->req_queue) &&
	    !test_bit(RAW3270_FLAGS_BUSY, &rp->flags)) {
		/* No other requests are on the queue. Start this one. */
		rq->rc = ccw_device_start(rp->cdev, &rq->ccw,
<<<<<<< HEAD
					  (unsigned long)rq, 0, 0);
=======
					       (unsigned long) rq, 0, 0);
>>>>>>> b7ba80a49124 (Commit)
		if (rq->rc) {
			raw3270_put_view(view);
			return rq->rc;
		}
	}
	list_add_tail(&rq->list, &rp->req_queue);
	return 0;
}

<<<<<<< HEAD
int raw3270_view_active(struct raw3270_view *view)
=======
int
raw3270_view_active(struct raw3270_view *view)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270 *rp = view->dev;

	return rp && rp->view == view;
}

<<<<<<< HEAD
int raw3270_start(struct raw3270_view *view, struct raw3270_request *rq)
=======
int
raw3270_start(struct raw3270_view *view, struct raw3270_request *rq)
>>>>>>> b7ba80a49124 (Commit)
{
	unsigned long flags;
	struct raw3270 *rp;
	int rc;

	spin_lock_irqsave(get_ccwdev_lock(view->dev->cdev), flags);
	rp = view->dev;
	if (!rp || rp->view != view)
		rc = -EACCES;
	else if (!raw3270_state_ready(rp))
		rc = -EBUSY;
	else
		rc =  __raw3270_start(rp, view, rq);
	spin_unlock_irqrestore(get_ccwdev_lock(view->dev->cdev), flags);
	return rc;
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_start);

int raw3270_start_request(struct raw3270_view *view, struct raw3270_request *rq,
			  int cmd, void *data, size_t len)
{
	int rc;

	rc = raw3270_request_reset(rq);
	if (rc)
		return rc;
	raw3270_request_set_cmd(rq, cmd);
	rc = raw3270_request_add_data(rq, data, len);
	if (rc)
		return rc;
	return raw3270_start(view, rq);
}
EXPORT_SYMBOL(raw3270_start_request);

int raw3270_start_locked(struct raw3270_view *view, struct raw3270_request *rq)
=======

int
raw3270_start_locked(struct raw3270_view *view, struct raw3270_request *rq)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270 *rp;
	int rc;

	rp = view->dev;
	if (!rp || rp->view != view)
		rc = -EACCES;
	else if (!raw3270_state_ready(rp))
		rc = -EBUSY;
	else
		rc =  __raw3270_start(rp, view, rq);
	return rc;
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_start_locked);

int raw3270_start_irq(struct raw3270_view *view, struct raw3270_request *rq)
=======

int
raw3270_start_irq(struct raw3270_view *view, struct raw3270_request *rq)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270 *rp;

	rp = view->dev;
	rq->view = view;
	raw3270_get_view(view);
	list_add_tail(&rq->list, &rp->req_queue);
	return 0;
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_start_irq);
=======
>>>>>>> b7ba80a49124 (Commit)

/*
 * 3270 interrupt routine, called from the ccw_device layer
 */
<<<<<<< HEAD
static void raw3270_irq(struct ccw_device *cdev, unsigned long intparm, struct irb *irb)
=======
static void
raw3270_irq (struct ccw_device *cdev, unsigned long intparm, struct irb *irb)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270 *rp;
	struct raw3270_view *view;
	struct raw3270_request *rq;

	rp = dev_get_drvdata(&cdev->dev);
	if (!rp)
		return;
<<<<<<< HEAD
	rq = (struct raw3270_request *)intparm;
=======
	rq = (struct raw3270_request *) intparm;
>>>>>>> b7ba80a49124 (Commit)
	view = rq ? rq->view : rp->view;

	if (!IS_ERR(irb)) {
		/* Handle CE-DE-UE and subsequent UDE */
		if (irb->scsw.cmd.dstat & DEV_STAT_DEV_END)
			clear_bit(RAW3270_FLAGS_BUSY, &rp->flags);
		if (irb->scsw.cmd.dstat == (DEV_STAT_CHN_END |
					    DEV_STAT_DEV_END |
					    DEV_STAT_UNIT_EXCEP))
			set_bit(RAW3270_FLAGS_BUSY, &rp->flags);
		/* Handle disconnected devices */
		if ((irb->scsw.cmd.dstat & DEV_STAT_UNIT_CHECK) &&
		    (irb->ecw[0] & SNS0_INTERVENTION_REQ)) {
			set_bit(RAW3270_FLAGS_BUSY, &rp->flags);
			if (rp->state > RAW3270_STATE_RESET)
				__raw3270_disconnect(rp);
		}
		/* Call interrupt handler of the view */
		if (view)
			view->fn->intv(view, rq, irb);
	}

	if (test_bit(RAW3270_FLAGS_BUSY, &rp->flags))
		/* Device busy, do not start I/O */
		return;

	if (rq && !list_empty(&rq->list)) {
		/* The request completed, remove from queue and do callback. */
		list_del_init(&rq->list);
		if (rq->callback)
			rq->callback(rq, rq->callback_data);
		/* Do put_device for get_device in raw3270_start. */
		raw3270_put_view(view);
	}

	/*
	 * Try to start each request on request queue until one is
	 * started successful.
	 */
	while (!list_empty(&rp->req_queue)) {
<<<<<<< HEAD
		rq = list_entry(rp->req_queue.next, struct raw3270_request, list);
		rq->rc = ccw_device_start(rp->cdev, &rq->ccw,
					  (unsigned long)rq, 0, 0);
=======
		rq = list_entry(rp->req_queue.next,struct raw3270_request,list);
		rq->rc = ccw_device_start(rp->cdev, &rq->ccw,
					  (unsigned long) rq, 0, 0);
>>>>>>> b7ba80a49124 (Commit)
		if (rq->rc == 0)
			break;
		/* Start failed. Remove request and do callback. */
		list_del_init(&rq->list);
		if (rq->callback)
			rq->callback(rq, rq->callback_data);
		/* Do put_device for get_device in raw3270_start. */
		raw3270_put_view(view);
	}
}

/*
 * To determine the size of the 3270 device we need to do:
 * 1) send a 'read partition' data stream to the device
 * 2) wait for the attn interrupt that precedes the query reply
 * 3) do a read modified to get the query reply
 * To make things worse we have to cope with intervention
 * required (3270 device switched to 'stand-by') and command
 * rejects (old devices that can't do 'read partition').
 */
struct raw3270_ua {	/* Query Reply structure for Usable Area */
	struct {	/* Usable Area Query Reply Base */
		short l;	/* Length of this structured field */
		char  sfid;	/* 0x81 if Query Reply */
		char  qcode;	/* 0x81 if Usable Area */
		char  flags0;
		char  flags1;
		short w;	/* Width of usable area */
		short h;	/* Heigth of usavle area */
		char  units;	/* 0x00:in; 0x01:mm */
		int   xr;
		int   yr;
		char  aw;
		char  ah;
		short buffsz;	/* Character buffer size, bytes */
		char  xmin;
		char  ymin;
		char  xmax;
		char  ymax;
<<<<<<< HEAD
	} __packed uab;
=======
	} __attribute__ ((packed)) uab;
>>>>>>> b7ba80a49124 (Commit)
	struct {	/* Alternate Usable Area Self-Defining Parameter */
		char  l;	/* Length of this Self-Defining Parm */
		char  sdpid;	/* 0x02 if Alternate Usable Area */
		char  res;
		char  auaid;	/* 0x01 is Id for the A U A */
		short wauai;	/* Width of AUAi */
		short hauai;	/* Height of AUAi */
		char  auaunits;	/* 0x00:in, 0x01:mm */
		int   auaxr;
		int   auayr;
		char  awauai;
		char  ahauai;
<<<<<<< HEAD
	} __packed aua;
} __packed;

static void raw3270_size_device_vm(struct raw3270 *rp)
=======
	} __attribute__ ((packed)) aua;
} __attribute__ ((packed));

static void
raw3270_size_device_vm(struct raw3270 *rp)
>>>>>>> b7ba80a49124 (Commit)
{
	int rc, model;
	struct ccw_dev_id dev_id;
	struct diag210 diag_data;
<<<<<<< HEAD
	struct diag8c diag8c_data;

	ccw_device_get_id(rp->cdev, &dev_id);
	rc = diag8c(&diag8c_data, &dev_id);
	if (!rc) {
		rp->model = 2;
		rp->rows = diag8c_data.height;
		rp->cols = diag8c_data.width;
		if (diag8c_data.flags & 1)
			set_bit(RAW3270_FLAGS_14BITADDR, &rp->flags);
		return;
	}

=======

	ccw_device_get_id(rp->cdev, &dev_id);
>>>>>>> b7ba80a49124 (Commit)
	diag_data.vrdcdvno = dev_id.devno;
	diag_data.vrdclen = sizeof(struct diag210);
	rc = diag210(&diag_data);
	model = diag_data.vrdccrmd;
	/* Use default model 2 if the size could not be detected */
	if (rc || model < 2 || model > 5)
		model = 2;
	switch (model) {
	case 2:
		rp->model = model;
		rp->rows = 24;
		rp->cols = 80;
		break;
	case 3:
		rp->model = model;
		rp->rows = 32;
		rp->cols = 80;
		break;
	case 4:
		rp->model = model;
		rp->rows = 43;
		rp->cols = 80;
		break;
	case 5:
		rp->model = model;
		rp->rows = 27;
		rp->cols = 132;
		break;
	}
}

<<<<<<< HEAD
static void raw3270_size_device(struct raw3270 *rp, char *init_data)
=======
static void
raw3270_size_device(struct raw3270 *rp)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270_ua *uap;

	/* Got a Query Reply */
<<<<<<< HEAD
	uap = (struct raw3270_ua *)(init_data + 1);
	/* Paranoia check. */
	if (init_data[0] != 0x88 || uap->uab.qcode != 0x81) {
=======
	uap = (struct raw3270_ua *) (rp->init_data + 1);
	/* Paranoia check. */
	if (rp->init_readmod.rc || rp->init_data[0] != 0x88 ||
	    uap->uab.qcode != 0x81) {
>>>>>>> b7ba80a49124 (Commit)
		/* Couldn't detect size. Use default model 2. */
		rp->model = 2;
		rp->rows = 24;
		rp->cols = 80;
		return;
	}
	/* Copy rows/columns of default Usable Area */
	rp->rows = uap->uab.h;
	rp->cols = uap->uab.w;
	/* Check for 14 bit addressing */
	if ((uap->uab.flags0 & 0x0d) == 0x01)
		set_bit(RAW3270_FLAGS_14BITADDR, &rp->flags);
	/* Check for Alternate Usable Area */
	if (uap->uab.l == sizeof(struct raw3270_ua) &&
	    uap->aua.sdpid == 0x02) {
		rp->rows = uap->aua.hauai;
		rp->cols = uap->aua.wauai;
	}
	/* Try to find a model. */
	rp->model = 0;
	if (rp->rows == 24 && rp->cols == 80)
		rp->model = 2;
	if (rp->rows == 32 && rp->cols == 80)
		rp->model = 3;
	if (rp->rows == 43 && rp->cols == 80)
		rp->model = 4;
	if (rp->rows == 27 && rp->cols == 132)
		rp->model = 5;
}

<<<<<<< HEAD
static void raw3270_resize_work(struct work_struct *work)
{
	struct raw3270 *rp = container_of(work, struct raw3270, resize_work);
	struct raw3270_view *view;

	/* Notify views about new size */
	list_for_each_entry(view, &rp->view_list, list) {
		if (view->fn->resize)
			view->fn->resize(view, rp->model, rp->rows, rp->cols,
					 rp->old_model, rp->old_rows, rp->old_cols);
	}
	rp->old_cols = rp->cols;
	rp->old_rows = rp->rows;
	rp->old_model = rp->model;
=======
static void
raw3270_size_device_done(struct raw3270 *rp)
{
	struct raw3270_view *view;

	rp->view = NULL;
	rp->state = RAW3270_STATE_READY;
	/* Notify views about new size */
	list_for_each_entry(view, &rp->view_list, list)
		if (view->fn->resize)
			view->fn->resize(view, rp->model, rp->rows, rp->cols);
>>>>>>> b7ba80a49124 (Commit)
	/* Setup processing done, now activate a view */
	list_for_each_entry(view, &rp->view_list, list) {
		rp->view = view;
		if (view->fn->activate(view) == 0)
			break;
		rp->view = NULL;
	}
}

<<<<<<< HEAD
static void raw3270_size_device_done(struct raw3270 *rp)
{
	rp->view = NULL;
	rp->state = RAW3270_STATE_READY;
	schedule_work(&rp->resize_work);
}

void raw3270_read_modified_cb(struct raw3270_request *rq, void *data)
{
	struct raw3270 *rp = rq->view->dev;

	raw3270_size_device(rp, data);
	raw3270_size_device_done(rp);
}
EXPORT_SYMBOL(raw3270_read_modified_cb);

static void raw3270_read_modified(struct raw3270 *rp)
=======
static void
raw3270_read_modified_cb(struct raw3270_request *rq, void *data)
{
	struct raw3270 *rp = rq->view->dev;

	raw3270_size_device(rp);
	raw3270_size_device_done(rp);
}

static void
raw3270_read_modified(struct raw3270 *rp)
>>>>>>> b7ba80a49124 (Commit)
{
	if (rp->state != RAW3270_STATE_W4ATTN)
		return;
	/* Use 'read modified' to get the result of a read partition. */
	memset(&rp->init_readmod, 0, sizeof(rp->init_readmod));
	memset(&rp->init_data, 0, sizeof(rp->init_data));
	rp->init_readmod.ccw.cmd_code = TC_READMOD;
	rp->init_readmod.ccw.flags = CCW_FLAG_SLI;
	rp->init_readmod.ccw.count = sizeof(rp->init_data);
<<<<<<< HEAD
	rp->init_readmod.ccw.cda = (__u32)__pa(rp->init_data);
	rp->init_readmod.callback = raw3270_read_modified_cb;
	rp->init_readmod.callback_data = rp->init_data;
=======
	rp->init_readmod.ccw.cda = (__u32) __pa(rp->init_data);
	rp->init_readmod.callback = raw3270_read_modified_cb;
>>>>>>> b7ba80a49124 (Commit)
	rp->state = RAW3270_STATE_READMOD;
	raw3270_start_irq(&rp->init_view, &rp->init_readmod);
}

<<<<<<< HEAD
static void raw3270_writesf_readpart(struct raw3270 *rp)
{
	static const unsigned char wbuf[] = {
		0x00, 0x07, 0x01, 0xff, 0x03, 0x00, 0x81
	};
=======
static void
raw3270_writesf_readpart(struct raw3270 *rp)
{
	static const unsigned char wbuf[] =
		{ 0x00, 0x07, 0x01, 0xff, 0x03, 0x00, 0x81 };
>>>>>>> b7ba80a49124 (Commit)

	/* Store 'read partition' data stream to init_data */
	memset(&rp->init_readpart, 0, sizeof(rp->init_readpart));
	memset(&rp->init_data, 0, sizeof(rp->init_data));
	memcpy(&rp->init_data, wbuf, sizeof(wbuf));
	rp->init_readpart.ccw.cmd_code = TC_WRITESF;
	rp->init_readpart.ccw.flags = CCW_FLAG_SLI;
	rp->init_readpart.ccw.count = sizeof(wbuf);
<<<<<<< HEAD
	rp->init_readpart.ccw.cda = (__u32)__pa(&rp->init_data);
=======
	rp->init_readpart.ccw.cda = (__u32) __pa(&rp->init_data);
>>>>>>> b7ba80a49124 (Commit)
	rp->state = RAW3270_STATE_W4ATTN;
	raw3270_start_irq(&rp->init_view, &rp->init_readpart);
}

/*
 * Device reset
 */
<<<<<<< HEAD
static void raw3270_reset_device_cb(struct raw3270_request *rq, void *data)
=======
static void
raw3270_reset_device_cb(struct raw3270_request *rq, void *data)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270 *rp = rq->view->dev;

	if (rp->state != RAW3270_STATE_RESET)
		return;
	if (rq->rc) {
		/* Reset command failed. */
		rp->state = RAW3270_STATE_INIT;
	} else if (MACHINE_IS_VM) {
		raw3270_size_device_vm(rp);
		raw3270_size_device_done(rp);
<<<<<<< HEAD
	} else {
		raw3270_writesf_readpart(rp);
	}
	memset(&rp->init_reset, 0, sizeof(rp->init_reset));
}

static int __raw3270_reset_device(struct raw3270 *rp)
=======
	} else
		raw3270_writesf_readpart(rp);
	memset(&rp->init_reset, 0, sizeof(rp->init_reset));
}

static int
__raw3270_reset_device(struct raw3270 *rp)
>>>>>>> b7ba80a49124 (Commit)
{
	int rc;

	/* Check if reset is already pending */
	if (rp->init_reset.view)
		return -EBUSY;
	/* Store reset data stream to init_data/init_reset */
	rp->init_data[0] = TW_KR;
	rp->init_reset.ccw.cmd_code = TC_EWRITEA;
	rp->init_reset.ccw.flags = CCW_FLAG_SLI;
	rp->init_reset.ccw.count = 1;
<<<<<<< HEAD
	rp->init_reset.ccw.cda = (__u32)__pa(rp->init_data);
=======
	rp->init_reset.ccw.cda = (__u32) __pa(rp->init_data);
>>>>>>> b7ba80a49124 (Commit)
	rp->init_reset.callback = raw3270_reset_device_cb;
	rc = __raw3270_start(rp, &rp->init_view, &rp->init_reset);
	if (rc == 0 && rp->state == RAW3270_STATE_INIT)
		rp->state = RAW3270_STATE_RESET;
	return rc;
}

<<<<<<< HEAD
static int raw3270_reset_device(struct raw3270 *rp)
=======
static int
raw3270_reset_device(struct raw3270 *rp)
>>>>>>> b7ba80a49124 (Commit)
{
	unsigned long flags;
	int rc;

	spin_lock_irqsave(get_ccwdev_lock(rp->cdev), flags);
	rc = __raw3270_reset_device(rp);
	spin_unlock_irqrestore(get_ccwdev_lock(rp->cdev), flags);
	return rc;
}

<<<<<<< HEAD
int raw3270_reset(struct raw3270_view *view)
=======
int
raw3270_reset(struct raw3270_view *view)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270 *rp;
	int rc;

	rp = view->dev;
	if (!rp || rp->view != view)
		rc = -EACCES;
	else if (!raw3270_state_ready(rp))
		rc = -EBUSY;
	else
		rc = raw3270_reset_device(view->dev);
	return rc;
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_reset);

static void __raw3270_disconnect(struct raw3270 *rp)
=======

static void
__raw3270_disconnect(struct raw3270 *rp)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270_request *rq;
	struct raw3270_view *view;

	rp->state = RAW3270_STATE_INIT;
	rp->view = &rp->init_view;
	/* Cancel all queued requests */
	while (!list_empty(&rp->req_queue)) {
<<<<<<< HEAD
		rq = list_entry(rp->req_queue.next, struct raw3270_request, list);
=======
		rq = list_entry(rp->req_queue.next,struct raw3270_request,list);
>>>>>>> b7ba80a49124 (Commit)
		view = rq->view;
		rq->rc = -EACCES;
		list_del_init(&rq->list);
		if (rq->callback)
			rq->callback(rq, rq->callback_data);
		raw3270_put_view(view);
	}
	/* Start from scratch */
	__raw3270_reset_device(rp);
}

<<<<<<< HEAD
static void raw3270_init_irq(struct raw3270_view *view, struct raw3270_request *rq,
			     struct irb *irb)
=======
static void
raw3270_init_irq(struct raw3270_view *view, struct raw3270_request *rq,
		 struct irb *irb)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270 *rp;

	if (rq) {
		if (irb->scsw.cmd.dstat & DEV_STAT_UNIT_CHECK) {
			if (irb->ecw[0] & SNS0_CMD_REJECT)
				rq->rc = -EOPNOTSUPP;
			else
				rq->rc = -EIO;
		}
	}
	if (irb->scsw.cmd.dstat & DEV_STAT_ATTENTION) {
		/* Queue read modified after attention interrupt */
		rp = view->dev;
		raw3270_read_modified(rp);
	}
}

static struct raw3270_fn raw3270_init_fn = {
	.intv = raw3270_init_irq
};

/*
 * Setup new 3270 device.
 */
<<<<<<< HEAD
static int raw3270_setup_device(struct ccw_device *cdev, struct raw3270 *rp,
				char *ascebc)
=======
static int
raw3270_setup_device(struct ccw_device *cdev, struct raw3270 *rp, char *ascebc)
>>>>>>> b7ba80a49124 (Commit)
{
	struct list_head *l;
	struct raw3270 *tmp;
	int minor;

	memset(rp, 0, sizeof(struct raw3270));
	/* Copy ebcdic -> ascii translation table. */
	memcpy(ascebc, _ascebc, 256);
	if (tubxcorrect) {
		/* correct brackets and circumflex */
		ascebc['['] = 0xad;
		ascebc[']'] = 0xbd;
		ascebc['^'] = 0xb0;
	}
	rp->ascebc = ascebc;

	/* Set defaults. */
	rp->rows = 24;
	rp->cols = 80;
<<<<<<< HEAD
	rp->old_rows = rp->rows;
	rp->old_cols = rp->cols;
=======
>>>>>>> b7ba80a49124 (Commit)

	INIT_LIST_HEAD(&rp->req_queue);
	INIT_LIST_HEAD(&rp->view_list);

	rp->init_view.dev = rp;
	rp->init_view.fn = &raw3270_init_fn;
	rp->view = &rp->init_view;
<<<<<<< HEAD
	INIT_WORK(&rp->resize_work, raw3270_resize_work);
=======
>>>>>>> b7ba80a49124 (Commit)

	/*
	 * Add device to list and find the smallest unused minor
	 * number for it. Note: there is no device with minor 0,
	 * see special case for fs3270.c:fs3270_open().
	 */
	mutex_lock(&raw3270_mutex);
	/* Keep the list sorted. */
	minor = RAW3270_FIRSTMINOR;
	rp->minor = -1;
	list_for_each(l, &raw3270_devices) {
		tmp = list_entry(l, struct raw3270, list);
		if (tmp->minor > minor) {
			rp->minor = minor;
			__list_add(&rp->list, l->prev, l);
			break;
		}
		minor++;
	}
	if (rp->minor == -1 && minor < RAW3270_MAXDEVS + RAW3270_FIRSTMINOR) {
		rp->minor = minor;
		list_add_tail(&rp->list, &raw3270_devices);
	}
	mutex_unlock(&raw3270_mutex);
	/* No free minor number? Then give up. */
	if (rp->minor == -1)
		return -EUSERS;
	rp->cdev = cdev;
	dev_set_drvdata(&cdev->dev, rp);
	cdev->handler = raw3270_irq;
	return 0;
}

#ifdef CONFIG_TN3270_CONSOLE
/* Tentative definition - see below for actual definition. */
static struct ccw_driver raw3270_ccw_driver;

<<<<<<< HEAD
static inline int raw3270_state_final(struct raw3270 *rp)
{
	return rp->state == RAW3270_STATE_INIT ||
		rp->state == RAW3270_STATE_READY;
}

=======
>>>>>>> b7ba80a49124 (Commit)
/*
 * Setup 3270 device configured as console.
 */
struct raw3270 __init *raw3270_setup_console(void)
{
	struct ccw_device *cdev;
	unsigned long flags;
	struct raw3270 *rp;
	char *ascebc;
	int rc;

	cdev = ccw_device_create_console(&raw3270_ccw_driver);
	if (IS_ERR(cdev))
		return ERR_CAST(cdev);

<<<<<<< HEAD
	rp = kzalloc(sizeof(*rp), GFP_KERNEL | GFP_DMA);
=======
	rp = kzalloc(sizeof(struct raw3270), GFP_KERNEL | GFP_DMA);
>>>>>>> b7ba80a49124 (Commit)
	ascebc = kzalloc(256, GFP_KERNEL);
	rc = raw3270_setup_device(cdev, rp, ascebc);
	if (rc)
		return ERR_PTR(rc);
	set_bit(RAW3270_FLAGS_CONSOLE, &rp->flags);

	rc = ccw_device_enable_console(cdev);
	if (rc) {
		ccw_device_destroy_console(cdev);
		return ERR_PTR(rc);
	}

	spin_lock_irqsave(get_ccwdev_lock(rp->cdev), flags);
	do {
		__raw3270_reset_device(rp);
		while (!raw3270_state_final(rp)) {
			ccw_device_wait_idle(rp->cdev);
			barrier();
		}
	} while (rp->state != RAW3270_STATE_READY);
	spin_unlock_irqrestore(get_ccwdev_lock(rp->cdev), flags);
	return rp;
}

<<<<<<< HEAD
void raw3270_wait_cons_dev(struct raw3270 *rp)
=======
void
raw3270_wait_cons_dev(struct raw3270 *rp)
>>>>>>> b7ba80a49124 (Commit)
{
	unsigned long flags;

	spin_lock_irqsave(get_ccwdev_lock(rp->cdev), flags);
	ccw_device_wait_idle(rp->cdev);
	spin_unlock_irqrestore(get_ccwdev_lock(rp->cdev), flags);
}

#endif

/*
 * Create a 3270 device structure.
 */
<<<<<<< HEAD
static struct raw3270 *raw3270_create_device(struct ccw_device *cdev)
=======
static struct raw3270 *
raw3270_create_device(struct ccw_device *cdev)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270 *rp;
	char *ascebc;
	int rc;

<<<<<<< HEAD
	rp = kzalloc(sizeof(*rp), GFP_KERNEL | GFP_DMA);
=======
	rp = kzalloc(sizeof(struct raw3270), GFP_KERNEL | GFP_DMA);
>>>>>>> b7ba80a49124 (Commit)
	if (!rp)
		return ERR_PTR(-ENOMEM);
	ascebc = kmalloc(256, GFP_KERNEL);
	if (!ascebc) {
		kfree(rp);
		return ERR_PTR(-ENOMEM);
	}
	rc = raw3270_setup_device(cdev, rp, ascebc);
	if (rc) {
		kfree(rp->ascebc);
		kfree(rp);
		rp = ERR_PTR(rc);
	}
	/* Get reference to ccw_device structure. */
	get_device(&cdev->dev);
	return rp;
}

/*
 * This helper just validates that it is safe to activate a
 * view in the panic() context, due to locking restrictions.
 */
int raw3270_view_lock_unavailable(struct raw3270_view *view)
{
	struct raw3270 *rp = view->dev;

	if (!rp)
		return -ENODEV;
	if (spin_is_locked(get_ccwdev_lock(rp->cdev)))
		return -EBUSY;
	return 0;
}

<<<<<<< HEAD
static int raw3270_assign_activate_view(struct raw3270 *rp, struct raw3270_view *view)
{
	rp->view = view;
	return view->fn->activate(view);
}

static int __raw3270_activate_view(struct raw3270 *rp, struct raw3270_view *view)
{
	struct raw3270_view *oldview = NULL, *nv;
	int rc;

	if (rp->view == view)
		return 0;

	if (!raw3270_state_ready(rp))
		return -EBUSY;

	if (rp->view && rp->view->fn->deactivate) {
		oldview = rp->view;
		oldview->fn->deactivate(oldview);
	}

	rc = raw3270_assign_activate_view(rp, view);
	if (!rc)
		return 0;

	/* Didn't work. Try to reactivate the old view. */
	if (oldview) {
		rc = raw3270_assign_activate_view(rp, oldview);
		if (!rc)
			return 0;
	}

	/* Didn't work as well. Try any other view. */
	list_for_each_entry(nv, &rp->view_list, list) {
		if (nv == view || nv == oldview)
			continue;
		rc = raw3270_assign_activate_view(rp, nv);
		if (!rc)
			break;
		rp->view = NULL;
	}
	return rc;
}

/*
 * Activate a view.
 */
int raw3270_activate_view(struct raw3270_view *view)
{
	struct raw3270 *rp;
=======
/*
 * Activate a view.
 */
int
raw3270_activate_view(struct raw3270_view *view)
{
	struct raw3270 *rp;
	struct raw3270_view *oldview, *nv;
>>>>>>> b7ba80a49124 (Commit)
	unsigned long flags;
	int rc;

	rp = view->dev;
	if (!rp)
		return -ENODEV;
	spin_lock_irqsave(get_ccwdev_lock(rp->cdev), flags);
<<<<<<< HEAD
	rc = __raw3270_activate_view(rp, view);
	spin_unlock_irqrestore(get_ccwdev_lock(rp->cdev), flags);
	return rc;
}
EXPORT_SYMBOL(raw3270_activate_view);
=======
	if (rp->view == view)
		rc = 0;
	else if (!raw3270_state_ready(rp))
		rc = -EBUSY;
	else {
		oldview = NULL;
		if (rp->view && rp->view->fn->deactivate) {
			oldview = rp->view;
			oldview->fn->deactivate(oldview);
		}
		rp->view = view;
		rc = view->fn->activate(view);
		if (rc) {
			/* Didn't work. Try to reactivate the old view. */
			rp->view = oldview;
			if (!oldview || oldview->fn->activate(oldview) != 0) {
				/* Didn't work as well. Try any other view. */
				list_for_each_entry(nv, &rp->view_list, list)
					if (nv != view && nv != oldview) {
						rp->view = nv;
						if (nv->fn->activate(nv) == 0)
							break;
						rp->view = NULL;
					}
			}
		}
	}
	spin_unlock_irqrestore(get_ccwdev_lock(rp->cdev), flags);
	return rc;
}
>>>>>>> b7ba80a49124 (Commit)

/*
 * Deactivate current view.
 */
<<<<<<< HEAD
void raw3270_deactivate_view(struct raw3270_view *view)
=======
void
raw3270_deactivate_view(struct raw3270_view *view)
>>>>>>> b7ba80a49124 (Commit)
{
	unsigned long flags;
	struct raw3270 *rp;

	rp = view->dev;
	if (!rp)
		return;
	spin_lock_irqsave(get_ccwdev_lock(rp->cdev), flags);
	if (rp->view == view) {
		view->fn->deactivate(view);
		rp->view = NULL;
		/* Move deactivated view to end of list. */
		list_del_init(&view->list);
		list_add_tail(&view->list, &rp->view_list);
		/* Try to activate another view. */
		if (raw3270_state_ready(rp)) {
			list_for_each_entry(view, &rp->view_list, list) {
				rp->view = view;
				if (view->fn->activate(view) == 0)
					break;
				rp->view = NULL;
			}
		}
	}
	spin_unlock_irqrestore(get_ccwdev_lock(rp->cdev), flags);
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_deactivate_view);
=======
>>>>>>> b7ba80a49124 (Commit)

/*
 * Add view to device with minor "minor".
 */
<<<<<<< HEAD
int raw3270_add_view(struct raw3270_view *view, struct raw3270_fn *fn,
		     int minor, int subclass)
=======
int
raw3270_add_view(struct raw3270_view *view, struct raw3270_fn *fn, int minor, int subclass)
>>>>>>> b7ba80a49124 (Commit)
{
	unsigned long flags;
	struct raw3270 *rp;
	int rc;

	if (minor <= 0)
		return -ENODEV;
	mutex_lock(&raw3270_mutex);
	rc = -ENODEV;
	list_for_each_entry(rp, &raw3270_devices, list) {
		if (rp->minor != minor)
			continue;
		spin_lock_irqsave(get_ccwdev_lock(rp->cdev), flags);
		atomic_set(&view->ref_count, 2);
		view->dev = rp;
		view->fn = fn;
		view->model = rp->model;
		view->rows = rp->rows;
		view->cols = rp->cols;
		view->ascebc = rp->ascebc;
		spin_lock_init(&view->lock);
		lockdep_set_subclass(&view->lock, subclass);
		list_add(&view->list, &rp->view_list);
		rc = 0;
		spin_unlock_irqrestore(get_ccwdev_lock(rp->cdev), flags);
		break;
	}
	mutex_unlock(&raw3270_mutex);
	return rc;
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_add_view);
=======
>>>>>>> b7ba80a49124 (Commit)

/*
 * Find specific view of device with minor "minor".
 */
<<<<<<< HEAD
struct raw3270_view *raw3270_find_view(struct raw3270_fn *fn, int minor)
=======
struct raw3270_view *
raw3270_find_view(struct raw3270_fn *fn, int minor)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270 *rp;
	struct raw3270_view *view, *tmp;
	unsigned long flags;

	mutex_lock(&raw3270_mutex);
	view = ERR_PTR(-ENODEV);
	list_for_each_entry(rp, &raw3270_devices, list) {
		if (rp->minor != minor)
			continue;
		spin_lock_irqsave(get_ccwdev_lock(rp->cdev), flags);
		list_for_each_entry(tmp, &rp->view_list, list) {
			if (tmp->fn == fn) {
				raw3270_get_view(tmp);
				view = tmp;
				break;
			}
		}
		spin_unlock_irqrestore(get_ccwdev_lock(rp->cdev), flags);
		break;
	}
	mutex_unlock(&raw3270_mutex);
	return view;
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_find_view);
=======
>>>>>>> b7ba80a49124 (Commit)

/*
 * Remove view from device and free view structure via call to view->fn->free.
 */
<<<<<<< HEAD
void raw3270_del_view(struct raw3270_view *view)
=======
void
raw3270_del_view(struct raw3270_view *view)
>>>>>>> b7ba80a49124 (Commit)
{
	unsigned long flags;
	struct raw3270 *rp;
	struct raw3270_view *nv;

	rp = view->dev;
	spin_lock_irqsave(get_ccwdev_lock(rp->cdev), flags);
	if (rp->view == view) {
		view->fn->deactivate(view);
		rp->view = NULL;
	}
	list_del_init(&view->list);
	if (!rp->view && raw3270_state_ready(rp)) {
		/* Try to activate another view. */
		list_for_each_entry(nv, &rp->view_list, list) {
			if (nv->fn->activate(nv) == 0) {
				rp->view = nv;
				break;
			}
		}
	}
	spin_unlock_irqrestore(get_ccwdev_lock(rp->cdev), flags);
	/* Wait for reference counter to drop to zero. */
	atomic_dec(&view->ref_count);
	wait_event(raw3270_wait_queue, atomic_read(&view->ref_count) == 0);
	if (view->fn->free)
		view->fn->free(view);
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_del_view);
=======
>>>>>>> b7ba80a49124 (Commit)

/*
 * Remove a 3270 device structure.
 */
<<<<<<< HEAD
static void raw3270_delete_device(struct raw3270 *rp)
=======
static void
raw3270_delete_device(struct raw3270 *rp)
>>>>>>> b7ba80a49124 (Commit)
{
	struct ccw_device *cdev;

	/* Remove from device chain. */
	mutex_lock(&raw3270_mutex);
	list_del_init(&rp->list);
	mutex_unlock(&raw3270_mutex);

	/* Disconnect from ccw_device. */
	cdev = rp->cdev;
	rp->cdev = NULL;
	dev_set_drvdata(&cdev->dev, NULL);
	cdev->handler = NULL;

	/* Put ccw_device structure. */
	put_device(&cdev->dev);

	/* Now free raw3270 structure. */
	kfree(rp->ascebc);
	kfree(rp);
}

<<<<<<< HEAD
static int raw3270_probe(struct ccw_device *cdev)
=======
static int
raw3270_probe (struct ccw_device *cdev)
>>>>>>> b7ba80a49124 (Commit)
{
	return 0;
}

/*
 * Additional attributes for a 3270 device
 */
<<<<<<< HEAD
static ssize_t model_show(struct device *dev, struct device_attribute *attr,
			  char *buf)
=======
static ssize_t
raw3270_model_show(struct device *dev, struct device_attribute *attr, char *buf)
>>>>>>> b7ba80a49124 (Commit)
{
	return sysfs_emit(buf, "%i\n",
			  ((struct raw3270 *)dev_get_drvdata(dev))->model);
}
<<<<<<< HEAD
static DEVICE_ATTR_RO(model);

static ssize_t rows_show(struct device *dev, struct device_attribute *attr,
			 char *buf)
=======
static DEVICE_ATTR(model, 0444, raw3270_model_show, NULL);

static ssize_t
raw3270_rows_show(struct device *dev, struct device_attribute *attr, char *buf)
>>>>>>> b7ba80a49124 (Commit)
{
	return sysfs_emit(buf, "%i\n",
			  ((struct raw3270 *)dev_get_drvdata(dev))->rows);
}
<<<<<<< HEAD
static DEVICE_ATTR_RO(rows);

static ssize_t
columns_show(struct device *dev, struct device_attribute *attr,
	     char *buf)
=======
static DEVICE_ATTR(rows, 0444, raw3270_rows_show, NULL);

static ssize_t
raw3270_columns_show(struct device *dev, struct device_attribute *attr, char *buf)
>>>>>>> b7ba80a49124 (Commit)
{
	return sysfs_emit(buf, "%i\n",
			  ((struct raw3270 *)dev_get_drvdata(dev))->cols);
}
<<<<<<< HEAD
static DEVICE_ATTR_RO(columns);

static struct attribute *raw3270_attrs[] = {
=======
static DEVICE_ATTR(columns, 0444, raw3270_columns_show, NULL);

static struct attribute * raw3270_attrs[] = {
>>>>>>> b7ba80a49124 (Commit)
	&dev_attr_model.attr,
	&dev_attr_rows.attr,
	&dev_attr_columns.attr,
	NULL,
};

static const struct attribute_group raw3270_attr_group = {
	.attrs = raw3270_attrs,
};

static int raw3270_create_attributes(struct raw3270 *rp)
{
	return sysfs_create_group(&rp->cdev->dev.kobj, &raw3270_attr_group);
}

/*
 * Notifier for device addition/removal
 */
static LIST_HEAD(raw3270_notifier);

int raw3270_register_notifier(struct raw3270_notifier *notifier)
{
	struct raw3270 *rp;

	mutex_lock(&raw3270_mutex);
	list_add_tail(&notifier->list, &raw3270_notifier);
	list_for_each_entry(rp, &raw3270_devices, list)
		notifier->create(rp->minor);
	mutex_unlock(&raw3270_mutex);
	return 0;
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_register_notifier);
=======
>>>>>>> b7ba80a49124 (Commit)

void raw3270_unregister_notifier(struct raw3270_notifier *notifier)
{
	struct raw3270 *rp;

	mutex_lock(&raw3270_mutex);
	list_for_each_entry(rp, &raw3270_devices, list)
		notifier->destroy(rp->minor);
	list_del(&notifier->list);
	mutex_unlock(&raw3270_mutex);
}
<<<<<<< HEAD
EXPORT_SYMBOL(raw3270_unregister_notifier);
=======
>>>>>>> b7ba80a49124 (Commit)

/*
 * Set 3270 device online.
 */
<<<<<<< HEAD
static int raw3270_set_online(struct ccw_device *cdev)
=======
static int
raw3270_set_online (struct ccw_device *cdev)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270_notifier *np;
	struct raw3270 *rp;
	int rc;

	rp = raw3270_create_device(cdev);
	if (IS_ERR(rp))
		return PTR_ERR(rp);
	rc = raw3270_create_attributes(rp);
	if (rc)
		goto failure;
	raw3270_reset_device(rp);
	mutex_lock(&raw3270_mutex);
	list_for_each_entry(np, &raw3270_notifier, list)
		np->create(rp->minor);
	mutex_unlock(&raw3270_mutex);
	return 0;

failure:
	raw3270_delete_device(rp);
	return rc;
}

/*
 * Remove 3270 device structure.
 */
<<<<<<< HEAD
static void raw3270_remove(struct ccw_device *cdev)
=======
static void
raw3270_remove (struct ccw_device *cdev)
>>>>>>> b7ba80a49124 (Commit)
{
	unsigned long flags;
	struct raw3270 *rp;
	struct raw3270_view *v;
	struct raw3270_notifier *np;

	rp = dev_get_drvdata(&cdev->dev);
	/*
	 * _remove is the opposite of _probe; it's probe that
	 * should set up rp.  raw3270_remove gets entered for
	 * devices even if they haven't been varied online.
	 * Thus, rp may validly be NULL here.
	 */
<<<<<<< HEAD
	if (!rp)
=======
	if (rp == NULL)
>>>>>>> b7ba80a49124 (Commit)
		return;

	sysfs_remove_group(&cdev->dev.kobj, &raw3270_attr_group);

	/* Deactivate current view and remove all views. */
	spin_lock_irqsave(get_ccwdev_lock(cdev), flags);
	if (rp->view) {
		if (rp->view->fn->deactivate)
			rp->view->fn->deactivate(rp->view);
		rp->view = NULL;
	}
	while (!list_empty(&rp->view_list)) {
		v = list_entry(rp->view_list.next, struct raw3270_view, list);
		if (v->fn->release)
			v->fn->release(v);
		spin_unlock_irqrestore(get_ccwdev_lock(cdev), flags);
		raw3270_del_view(v);
		spin_lock_irqsave(get_ccwdev_lock(cdev), flags);
	}
	spin_unlock_irqrestore(get_ccwdev_lock(cdev), flags);

	mutex_lock(&raw3270_mutex);
	list_for_each_entry(np, &raw3270_notifier, list)
		np->destroy(rp->minor);
	mutex_unlock(&raw3270_mutex);

	/* Reset 3270 device. */
	raw3270_reset_device(rp);
	/* And finally remove it. */
	raw3270_delete_device(rp);
}

/*
 * Set 3270 device offline.
 */
<<<<<<< HEAD
static int raw3270_set_offline(struct ccw_device *cdev)
=======
static int
raw3270_set_offline (struct ccw_device *cdev)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270 *rp;

	rp = dev_get_drvdata(&cdev->dev);
	if (test_bit(RAW3270_FLAGS_CONSOLE, &rp->flags))
		return -EBUSY;
	raw3270_remove(cdev);
	return 0;
}

static struct ccw_device_id raw3270_id[] = {
	{ CCW_DEVICE(0x3270, 0) },
	{ CCW_DEVICE(0x3271, 0) },
	{ CCW_DEVICE(0x3272, 0) },
	{ CCW_DEVICE(0x3273, 0) },
	{ CCW_DEVICE(0x3274, 0) },
	{ CCW_DEVICE(0x3275, 0) },
	{ CCW_DEVICE(0x3276, 0) },
	{ CCW_DEVICE(0x3277, 0) },
	{ CCW_DEVICE(0x3278, 0) },
	{ CCW_DEVICE(0x3279, 0) },
	{ CCW_DEVICE(0x3174, 0) },
	{ /* end of list */ },
};

static struct ccw_driver raw3270_ccw_driver = {
	.driver = {
		.name	= "3270",
		.owner	= THIS_MODULE,
	},
	.ids		= raw3270_id,
	.probe		= &raw3270_probe,
	.remove		= &raw3270_remove,
	.set_online	= &raw3270_set_online,
	.set_offline	= &raw3270_set_offline,
	.int_class	= IRQIO_C70,
};

<<<<<<< HEAD
static int raw3270_init(void)
=======
static int
raw3270_init(void)
>>>>>>> b7ba80a49124 (Commit)
{
	struct raw3270 *rp;
	int rc;

	if (raw3270_registered)
		return 0;
	raw3270_registered = 1;
	rc = ccw_driver_register(&raw3270_ccw_driver);
	if (rc == 0) {
		/* Create attributes for early (= console) device. */
		mutex_lock(&raw3270_mutex);
<<<<<<< HEAD
		class3270 = class_create("3270");
=======
		class3270 = class_create(THIS_MODULE, "3270");
>>>>>>> b7ba80a49124 (Commit)
		list_for_each_entry(rp, &raw3270_devices, list) {
			get_device(&rp->cdev->dev);
			raw3270_create_attributes(rp);
		}
		mutex_unlock(&raw3270_mutex);
	}
	return rc;
}

<<<<<<< HEAD
static void raw3270_exit(void)
=======
static void
raw3270_exit(void)
>>>>>>> b7ba80a49124 (Commit)
{
	ccw_driver_unregister(&raw3270_ccw_driver);
	class_destroy(class3270);
}

MODULE_LICENSE("GPL");

module_init(raw3270_init);
module_exit(raw3270_exit);
<<<<<<< HEAD
=======

EXPORT_SYMBOL(class3270);
EXPORT_SYMBOL(raw3270_request_alloc);
EXPORT_SYMBOL(raw3270_request_free);
EXPORT_SYMBOL(raw3270_request_reset);
EXPORT_SYMBOL(raw3270_request_set_cmd);
EXPORT_SYMBOL(raw3270_request_add_data);
EXPORT_SYMBOL(raw3270_request_set_data);
EXPORT_SYMBOL(raw3270_request_set_idal);
EXPORT_SYMBOL(raw3270_buffer_address);
EXPORT_SYMBOL(raw3270_add_view);
EXPORT_SYMBOL(raw3270_del_view);
EXPORT_SYMBOL(raw3270_find_view);
EXPORT_SYMBOL(raw3270_activate_view);
EXPORT_SYMBOL(raw3270_deactivate_view);
EXPORT_SYMBOL(raw3270_start);
EXPORT_SYMBOL(raw3270_start_locked);
EXPORT_SYMBOL(raw3270_start_irq);
EXPORT_SYMBOL(raw3270_reset);
EXPORT_SYMBOL(raw3270_register_notifier);
EXPORT_SYMBOL(raw3270_unregister_notifier);
EXPORT_SYMBOL(raw3270_wait_queue);
>>>>>>> b7ba80a49124 (Commit)
