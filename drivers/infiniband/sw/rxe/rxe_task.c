// SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB
/*
 * Copyright (c) 2016 Mellanox Technologies Ltd. All rights reserved.
 * Copyright (c) 2015 System Fabric Works, Inc. All rights reserved.
 */

<<<<<<< HEAD
=======
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/hardirq.h>

>>>>>>> b7ba80a49124 (Commit)
#include "rxe.h"

int __rxe_do_task(struct rxe_task *task)

{
	int ret;

	while ((ret = task->func(task->arg)) == 0)
		;

	task->ret = ret;

	return ret;
}

/*
 * this locking is due to a potential race where
 * a second caller finds the task already running
 * but looks just after the last call to func
 */
<<<<<<< HEAD
static void do_task(struct tasklet_struct *t)
=======
void rxe_do_task(struct tasklet_struct *t)
>>>>>>> b7ba80a49124 (Commit)
{
	int cont;
	int ret;
	struct rxe_task *task = from_tasklet(task, t, tasklet);
<<<<<<< HEAD
	struct rxe_qp *qp = (struct rxe_qp *)task->arg;
	unsigned int iterations = RXE_MAX_ITERATIONS;

	spin_lock_bh(&task->lock);
	switch (task->state) {
	case TASK_STATE_START:
		task->state = TASK_STATE_BUSY;
		spin_unlock_bh(&task->lock);
=======
	unsigned int iterations = RXE_MAX_ITERATIONS;

	spin_lock_bh(&task->state_lock);
	switch (task->state) {
	case TASK_STATE_START:
		task->state = TASK_STATE_BUSY;
		spin_unlock_bh(&task->state_lock);
>>>>>>> b7ba80a49124 (Commit)
		break;

	case TASK_STATE_BUSY:
		task->state = TASK_STATE_ARMED;
		fallthrough;
	case TASK_STATE_ARMED:
<<<<<<< HEAD
		spin_unlock_bh(&task->lock);
		return;

	default:
		spin_unlock_bh(&task->lock);
		rxe_dbg_qp(qp, "failed with bad state %d\n", task->state);
=======
		spin_unlock_bh(&task->state_lock);
		return;

	default:
		spin_unlock_bh(&task->state_lock);
		pr_warn("%s failed with bad state %d\n", __func__, task->state);
>>>>>>> b7ba80a49124 (Commit)
		return;
	}

	do {
		cont = 0;
		ret = task->func(task->arg);

<<<<<<< HEAD
		spin_lock_bh(&task->lock);
=======
		spin_lock_bh(&task->state_lock);
>>>>>>> b7ba80a49124 (Commit)
		switch (task->state) {
		case TASK_STATE_BUSY:
			if (ret) {
				task->state = TASK_STATE_START;
			} else if (iterations--) {
				cont = 1;
			} else {
				/* reschedule the tasklet and exit
				 * the loop to give up the cpu
				 */
				tasklet_schedule(&task->tasklet);
				task->state = TASK_STATE_START;
			}
			break;

		/* someone tried to run the task since the last time we called
		 * func, so we will call one more time regardless of the
		 * return value
		 */
		case TASK_STATE_ARMED:
			task->state = TASK_STATE_BUSY;
			cont = 1;
			break;

		default:
<<<<<<< HEAD
			rxe_dbg_qp(qp, "failed with bad state %d\n",
					task->state);
		}
		spin_unlock_bh(&task->lock);
=======
			pr_warn("%s failed with bad state %d\n", __func__,
				task->state);
		}
		spin_unlock_bh(&task->state_lock);
>>>>>>> b7ba80a49124 (Commit)
	} while (cont);

	task->ret = ret;
}

<<<<<<< HEAD
int rxe_init_task(struct rxe_task *task, void *arg, int (*func)(void *))
{
	task->arg	= arg;
	task->func	= func;
	task->destroyed	= false;

	tasklet_setup(&task->tasklet, do_task);

	task->state = TASK_STATE_START;
	spin_lock_init(&task->lock);
=======
int rxe_init_task(struct rxe_task *task,
		  void *arg, int (*func)(void *), char *name)
{
	task->arg	= arg;
	task->func	= func;
	snprintf(task->name, sizeof(task->name), "%s", name);
	task->destroyed	= false;

	tasklet_setup(&task->tasklet, rxe_do_task);

	task->state = TASK_STATE_START;
	spin_lock_init(&task->state_lock);
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

void rxe_cleanup_task(struct rxe_task *task)
{
	bool idle;

	/*
	 * Mark the task, then wait for it to finish. It might be
	 * running in a non-tasklet (direct call) context.
	 */
	task->destroyed = true;

	do {
<<<<<<< HEAD
		spin_lock_bh(&task->lock);
		idle = (task->state == TASK_STATE_START);
		spin_unlock_bh(&task->lock);
=======
		spin_lock_bh(&task->state_lock);
		idle = (task->state == TASK_STATE_START);
		spin_unlock_bh(&task->state_lock);
>>>>>>> b7ba80a49124 (Commit)
	} while (!idle);

	tasklet_kill(&task->tasklet);
}

<<<<<<< HEAD
void rxe_run_task(struct rxe_task *task)
=======
void rxe_run_task(struct rxe_task *task, int sched)
>>>>>>> b7ba80a49124 (Commit)
{
	if (task->destroyed)
		return;

<<<<<<< HEAD
	do_task(&task->tasklet);
}

void rxe_sched_task(struct rxe_task *task)
{
	if (task->destroyed)
		return;

	tasklet_schedule(&task->tasklet);
=======
	if (sched)
		tasklet_schedule(&task->tasklet);
	else
		rxe_do_task(&task->tasklet);
>>>>>>> b7ba80a49124 (Commit)
}

void rxe_disable_task(struct rxe_task *task)
{
	tasklet_disable(&task->tasklet);
}

void rxe_enable_task(struct rxe_task *task)
{
	tasklet_enable(&task->tasklet);
}
