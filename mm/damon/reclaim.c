// SPDX-License-Identifier: GPL-2.0
/*
 * DAMON-based page reclamation
 *
 * Author: SeongJae Park <sj@kernel.org>
 */

#define pr_fmt(fmt) "damon-reclaim: " fmt

#include <linux/damon.h>
<<<<<<< HEAD
#include <linux/kstrtox.h>
#include <linux/module.h>
=======
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
>>>>>>> b7ba80a49124 (Commit)

#include "modules-common.h"

#ifdef MODULE_PARAM_PREFIX
#undef MODULE_PARAM_PREFIX
#endif
#define MODULE_PARAM_PREFIX "damon_reclaim."

/*
 * Enable or disable DAMON_RECLAIM.
 *
 * You can enable DAMON_RCLAIM by setting the value of this parameter as ``Y``.
 * Setting it as ``N`` disables DAMON_RECLAIM.  Note that DAMON_RECLAIM could
 * do no real monitoring and reclamation due to the watermarks-based activation
 * condition.  Refer to below descriptions for the watermarks parameter for
 * this.
 */
static bool enabled __read_mostly;

/*
 * Make DAMON_RECLAIM reads the input parameters again, except ``enabled``.
 *
 * Input parameters that updated while DAMON_RECLAIM is running are not applied
 * by default.  Once this parameter is set as ``Y``, DAMON_RECLAIM reads values
 * of parametrs except ``enabled`` again.  Once the re-reading is done, this
 * parameter is set as ``N``.  If invalid parameters are found while the
 * re-reading, DAMON_RECLAIM will be disabled.
 */
static bool commit_inputs __read_mostly;
module_param(commit_inputs, bool, 0600);

/*
 * Time threshold for cold memory regions identification in microseconds.
 *
 * If a memory region is not accessed for this or longer time, DAMON_RECLAIM
 * identifies the region as cold, and reclaims.  120 seconds by default.
 */
static unsigned long min_age __read_mostly = 120000000;
module_param(min_age, ulong, 0600);

static struct damos_quota damon_reclaim_quota = {
	/* use up to 10 ms time, reclaim up to 128 MiB per 1 sec by default */
	.ms = 10,
	.sz = 128 * 1024 * 1024,
	.reset_interval = 1000,
	/* Within the quota, page out older regions first. */
	.weight_sz = 0,
	.weight_nr_accesses = 0,
	.weight_age = 1
};
DEFINE_DAMON_MODULES_DAMOS_QUOTAS(damon_reclaim_quota);

static struct damos_watermarks damon_reclaim_wmarks = {
	.metric = DAMOS_WMARK_FREE_MEM_RATE,
	.interval = 5000000,	/* 5 seconds */
	.high = 500,		/* 50 percent */
	.mid = 400,		/* 40 percent */
	.low = 200,		/* 20 percent */
};
DEFINE_DAMON_MODULES_WMARKS_PARAMS(damon_reclaim_wmarks);

static struct damon_attrs damon_reclaim_mon_attrs = {
	.sample_interval = 5000,	/* 5 ms */
	.aggr_interval = 100000,	/* 100 ms */
	.ops_update_interval = 0,
	.min_nr_regions = 10,
	.max_nr_regions = 1000,
};
DEFINE_DAMON_MODULES_MON_ATTRS_PARAMS(damon_reclaim_mon_attrs);

/*
 * Start of the target memory region in physical address.
 *
 * The start physical address of memory region that DAMON_RECLAIM will do work
 * against.  By default, biggest System RAM is used as the region.
 */
static unsigned long monitor_region_start __read_mostly;
module_param(monitor_region_start, ulong, 0600);

/*
 * End of the target memory region in physical address.
 *
 * The end physical address of memory region that DAMON_RECLAIM will do work
 * against.  By default, biggest System RAM is used as the region.
 */
static unsigned long monitor_region_end __read_mostly;
module_param(monitor_region_end, ulong, 0600);

/*
<<<<<<< HEAD
 * Skip anonymous pages reclamation.
 *
 * If this parameter is set as ``Y``, DAMON_RECLAIM does not reclaim anonymous
 * pages.  By default, ``N``.
 */
static bool skip_anon __read_mostly;
module_param(skip_anon, bool, 0600);

/*
=======
>>>>>>> b7ba80a49124 (Commit)
 * PID of the DAMON thread
 *
 * If DAMON_RECLAIM is enabled, this becomes the PID of the worker thread.
 * Else, -1.
 */
static int kdamond_pid __read_mostly = -1;
module_param(kdamond_pid, int, 0400);

static struct damos_stat damon_reclaim_stat;
DEFINE_DAMON_MODULES_DAMOS_STATS_PARAMS(damon_reclaim_stat,
		reclaim_tried_regions, reclaimed_regions, quota_exceeds);

static struct damon_ctx *ctx;
static struct damon_target *target;

static struct damos *damon_reclaim_new_scheme(void)
{
	struct damos_access_pattern pattern = {
		/* Find regions having PAGE_SIZE or larger size */
		.min_sz_region = PAGE_SIZE,
		.max_sz_region = ULONG_MAX,
		/* and not accessed at all */
		.min_nr_accesses = 0,
		.max_nr_accesses = 0,
		/* for min_age or more micro-seconds */
		.min_age_region = min_age /
			damon_reclaim_mon_attrs.aggr_interval,
		.max_age_region = UINT_MAX,
	};

	return damon_new_scheme(
			&pattern,
			/* page out those, as soon as found */
			DAMOS_PAGEOUT,
			/* under the quota. */
			&damon_reclaim_quota,
			/* (De)activate this according to the watermarks. */
			&damon_reclaim_wmarks);
}

static int damon_reclaim_apply_parameters(void)
{
	struct damos *scheme;
<<<<<<< HEAD
	struct damos_filter *filter;
=======
>>>>>>> b7ba80a49124 (Commit)
	int err = 0;

	err = damon_set_attrs(ctx, &damon_reclaim_mon_attrs);
	if (err)
		return err;

	/* Will be freed by next 'damon_set_schemes()' below */
	scheme = damon_reclaim_new_scheme();
	if (!scheme)
		return -ENOMEM;
<<<<<<< HEAD
	if (skip_anon) {
		filter = damos_new_filter(DAMOS_FILTER_TYPE_ANON, true);
		if (!filter) {
			/* Will be freed by next 'damon_set_schemes()' below */
			damon_destroy_scheme(scheme);
			return -ENOMEM;
		}
		damos_add_filter(scheme, filter);
	}
=======
>>>>>>> b7ba80a49124 (Commit)
	damon_set_schemes(ctx, &scheme, 1);

	return damon_set_region_biggest_system_ram_default(target,
					&monitor_region_start,
					&monitor_region_end);
}

static int damon_reclaim_turn(bool on)
{
	int err;

	if (!on) {
		err = damon_stop(&ctx, 1);
		if (!err)
			kdamond_pid = -1;
		return err;
	}

	err = damon_reclaim_apply_parameters();
	if (err)
		return err;

	err = damon_start(&ctx, 1, true);
	if (err)
		return err;
	kdamond_pid = ctx->kdamond->pid;
	return 0;
}

<<<<<<< HEAD
static int damon_reclaim_enabled_store(const char *val,
		const struct kernel_param *kp)
{
	bool is_enabled = enabled;
	bool enable;
	int err;

	err = kstrtobool(val, &enable);
	if (err)
		return err;

	if (is_enabled == enable)
		return 0;

	/* Called before init function.  The function will handle this. */
	if (!ctx)
		goto set_param_out;

	err = damon_reclaim_turn(enable);
	if (err)
		return err;

set_param_out:
	enabled = enable;
	return err;
=======
static struct delayed_work damon_reclaim_timer;
static void damon_reclaim_timer_fn(struct work_struct *work)
{
	static bool last_enabled;
	bool now_enabled;

	now_enabled = enabled;
	if (last_enabled != now_enabled) {
		if (!damon_reclaim_turn(now_enabled))
			last_enabled = now_enabled;
		else
			enabled = last_enabled;
	}
}
static DECLARE_DELAYED_WORK(damon_reclaim_timer, damon_reclaim_timer_fn);

static bool damon_reclaim_initialized;

static int damon_reclaim_enabled_store(const char *val,
		const struct kernel_param *kp)
{
	int rc = param_set_bool(val, kp);

	if (rc < 0)
		return rc;

	/* system_wq might not initialized yet */
	if (!damon_reclaim_initialized)
		return rc;

	schedule_delayed_work(&damon_reclaim_timer, 0);
	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

static const struct kernel_param_ops enabled_param_ops = {
	.set = damon_reclaim_enabled_store,
	.get = param_get_bool,
};

module_param_cb(enabled, &enabled_param_ops, &enabled, 0600);
MODULE_PARM_DESC(enabled,
	"Enable or disable DAMON_RECLAIM (default: disabled)");

static int damon_reclaim_handle_commit_inputs(void)
{
	int err;

	if (!commit_inputs)
		return 0;

	err = damon_reclaim_apply_parameters();
	commit_inputs = false;
	return err;
}

static int damon_reclaim_after_aggregation(struct damon_ctx *c)
{
	struct damos *s;

	/* update the stats parameter */
	damon_for_each_scheme(s, c)
		damon_reclaim_stat = s->stat;

	return damon_reclaim_handle_commit_inputs();
}

static int damon_reclaim_after_wmarks_check(struct damon_ctx *c)
{
	return damon_reclaim_handle_commit_inputs();
}

static int __init damon_reclaim_init(void)
{
<<<<<<< HEAD
	int err = damon_modules_new_paddr_ctx_target(&ctx, &target);

	if (err)
		return err;
=======
	ctx = damon_new_ctx();
	if (!ctx)
		return -ENOMEM;

	if (damon_select_ops(ctx, DAMON_OPS_PADDR)) {
		damon_destroy_ctx(ctx);
		return -EINVAL;
	}
>>>>>>> b7ba80a49124 (Commit)

	ctx->callback.after_wmarks_check = damon_reclaim_after_wmarks_check;
	ctx->callback.after_aggregation = damon_reclaim_after_aggregation;

<<<<<<< HEAD
	/* 'enabled' has set before this function, probably via command line */
	if (enabled)
		err = damon_reclaim_turn(true);

	return err;
=======
	target = damon_new_target();
	if (!target) {
		damon_destroy_ctx(ctx);
		return -ENOMEM;
	}
	damon_add_target(ctx, target);

	schedule_delayed_work(&damon_reclaim_timer, 0);

	damon_reclaim_initialized = true;
	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

module_init(damon_reclaim_init);
