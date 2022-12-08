// SPDX-License-Identifier: GPL-2.0-only
/*
 * intel_powerclamp.c - package c-state idle injection
 *
<<<<<<< HEAD
 * Copyright (c) 2012-2023, Intel Corporation.
=======
 * Copyright (c) 2012, Intel Corporation.
>>>>>>> b7ba80a49124 (Commit)
 *
 * Authors:
 *     Arjan van de Ven <arjan@linux.intel.com>
 *     Jacob Pan <jacob.jun.pan@linux.intel.com>
 *
 *	TODO:
 *           1. better handle wakeup from external interrupts, currently a fixed
 *              compensation is added to clamping duration when excessive amount
 *              of wakeups are observed during idle time. the reason is that in
 *              case of external interrupts without need for ack, clamping down
 *              cpu in non-irq context does not reduce irq. for majority of the
 *              cases, clamping down cpu does help reduce irq as well, we should
 *              be able to differentiate the two cases and give a quantitative
 *              solution for the irqs that we can control. perhaps based on
 *              get_cpu_iowait_time_us()
 *
 *	     2. synchronization with other hw blocks
 */

#define pr_fmt(fmt)	KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
<<<<<<< HEAD
#include <linux/cpu.h>
#include <linux/thermal.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/idle_inject.h>

#include <asm/msr.h>
#include <asm/mwait.h>
#include <asm/cpu_device_id.h>

#define MAX_TARGET_RATIO (100U)
=======
#include <linux/kthread.h>
#include <linux/cpu.h>
#include <linux/thermal.h>
#include <linux/slab.h>
#include <linux/tick.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/sched/rt.h>
#include <uapi/linux/sched/types.h>

#include <asm/nmi.h>
#include <asm/msr.h>
#include <asm/mwait.h>
#include <asm/cpu_device_id.h>
#include <asm/hardirq.h>

#define MAX_TARGET_RATIO (50U)
>>>>>>> b7ba80a49124 (Commit)
/* For each undisturbed clamping period (no extra wake ups during idle time),
 * we increment the confidence counter for the given target ratio.
 * CONFIDENCE_OK defines the level where runtime calibration results are
 * valid.
 */
#define CONFIDENCE_OK (3)
/* Default idle injection duration, driver adjust sleep time to meet target
 * idle ratio. Similar to frequency modulation.
 */
#define DEFAULT_DURATION_JIFFIES (6)

static unsigned int target_mwait;
static struct dentry *debug_dir;
<<<<<<< HEAD
static bool poll_pkg_cstate_enable;

/* Idle ratio observed using package C-state counters */
static unsigned int current_ratio;

/* Skip the idle injection till set to true */
static bool should_skip;

struct powerclamp_data {
=======

/* user selected target */
static unsigned int set_target_ratio;
static unsigned int current_ratio;
static bool should_skip;
static bool reduce_irq;
static atomic_t idle_wakeup_counter;
static unsigned int control_cpu; /* The cpu assigned to collect stat and update
				  * control parameters. default to BSP but BSP
				  * can be offlined.
				  */
static bool clamping;

struct powerclamp_worker_data {
	struct kthread_worker *worker;
	struct kthread_work balancing_work;
	struct kthread_delayed_work idle_injection_work;
>>>>>>> b7ba80a49124 (Commit)
	unsigned int cpu;
	unsigned int count;
	unsigned int guard;
	unsigned int window_size_now;
	unsigned int target_ratio;
<<<<<<< HEAD
	bool clamping;
};

static struct powerclamp_data powerclamp_data;

static struct thermal_cooling_device *cooling_dev;

static DEFINE_MUTEX(powerclamp_lock);

/* This duration is in microseconds */
=======
	unsigned int duration_jiffies;
	bool clamping;
};

static struct powerclamp_worker_data __percpu *worker_data;
static struct thermal_cooling_device *cooling_dev;
static unsigned long *cpu_clamping_mask;  /* bit map for tracking per cpu
					   * clamping kthread worker
					   */

>>>>>>> b7ba80a49124 (Commit)
static unsigned int duration;
static unsigned int pkg_cstate_ratio_cur;
static unsigned int window_size;

static int duration_set(const char *arg, const struct kernel_param *kp)
{
	int ret = 0;
	unsigned long new_duration;

	ret = kstrtoul(arg, 10, &new_duration);
	if (ret)
		goto exit;
	if (new_duration > 25 || new_duration < 6) {
		pr_err("Out of recommended range %lu, between 6-25ms\n",
			new_duration);
		ret = -EINVAL;
<<<<<<< HEAD
		goto exit;
	}

	mutex_lock(&powerclamp_lock);
	duration = clamp(new_duration, 6ul, 25ul) * 1000;
	mutex_unlock(&powerclamp_lock);
=======
	}

	duration = clamp(new_duration, 6ul, 25ul);
	smp_mb();

>>>>>>> b7ba80a49124 (Commit)
exit:

	return ret;
}

<<<<<<< HEAD
static int duration_get(char *buf, const struct kernel_param *kp)
{
	int ret;

	mutex_lock(&powerclamp_lock);
	ret = sysfs_emit(buf, "%d\n", duration / 1000);
	mutex_unlock(&powerclamp_lock);

	return ret;
}

static const struct kernel_param_ops duration_ops = {
	.set = duration_set,
	.get = duration_get,
};

module_param_cb(duration, &duration_ops, NULL, 0644);
MODULE_PARM_DESC(duration, "forced idle time for each attempt in msec.");

#define DEFAULT_MAX_IDLE	50
#define MAX_ALL_CPU_IDLE	75

static u8 max_idle = DEFAULT_MAX_IDLE;

static cpumask_var_t idle_injection_cpu_mask;

static int allocate_copy_idle_injection_mask(const struct cpumask *copy_mask)
{
	if (cpumask_available(idle_injection_cpu_mask))
		goto copy_mask;

	/* This mask is allocated only one time and freed during module exit */
	if (!alloc_cpumask_var(&idle_injection_cpu_mask, GFP_KERNEL))
		return -ENOMEM;

copy_mask:
	cpumask_copy(idle_injection_cpu_mask, copy_mask);

	return 0;
}

/* Return true if the cpumask and idle percent combination is invalid */
static bool check_invalid(cpumask_var_t mask, u8 idle)
{
	if (cpumask_equal(cpu_present_mask, mask) && idle > MAX_ALL_CPU_IDLE)
		return true;

	return false;
}

static int cpumask_set(const char *arg, const struct kernel_param *kp)
{
	cpumask_var_t new_mask;
	int ret;

	mutex_lock(&powerclamp_lock);

	/* Can't set mask when cooling device is in use */
	if (powerclamp_data.clamping) {
		ret = -EAGAIN;
		goto skip_cpumask_set;
	}

	ret = alloc_cpumask_var(&new_mask, GFP_KERNEL);
	if (!ret)
		goto skip_cpumask_set;

	ret = bitmap_parse(arg, strlen(arg), cpumask_bits(new_mask),
			   nr_cpumask_bits);
	if (ret)
		goto free_cpumask_set;

	if (cpumask_empty(new_mask) || check_invalid(new_mask, max_idle)) {
		ret = -EINVAL;
		goto free_cpumask_set;
	}

	/*
	 * When module parameters are passed from kernel command line
	 * during insmod, the module parameter callback is called
	 * before powerclamp_init(), so we can't assume that some
	 * cpumask can be allocated and copied before here. Also
	 * in this case this cpumask is used as the default mask.
	 */
	ret = allocate_copy_idle_injection_mask(new_mask);

free_cpumask_set:
	free_cpumask_var(new_mask);
skip_cpumask_set:
	mutex_unlock(&powerclamp_lock);

	return ret;
}

static int cpumask_get(char *buf, const struct kernel_param *kp)
{
	if (!cpumask_available(idle_injection_cpu_mask))
		return -ENODEV;

	return bitmap_print_to_pagebuf(false, buf, cpumask_bits(idle_injection_cpu_mask),
				       nr_cpumask_bits);
}

static const struct kernel_param_ops cpumask_ops = {
	.set = cpumask_set,
	.get = cpumask_get,
};

module_param_cb(cpumask, &cpumask_ops, NULL, 0644);
MODULE_PARM_DESC(cpumask, "Mask of CPUs to use for idle injection.");

static int max_idle_set(const char *arg, const struct kernel_param *kp)
{
	u8 new_max_idle;
	int ret = 0;

	mutex_lock(&powerclamp_lock);

	/* Can't set mask when cooling device is in use */
	if (powerclamp_data.clamping) {
		ret = -EAGAIN;
		goto skip_limit_set;
	}

	ret = kstrtou8(arg, 10, &new_max_idle);
	if (ret)
		goto skip_limit_set;

	if (new_max_idle > MAX_TARGET_RATIO) {
		ret = -EINVAL;
		goto skip_limit_set;
	}

	if (check_invalid(idle_injection_cpu_mask, new_max_idle)) {
		ret = -EINVAL;
		goto skip_limit_set;
	}

	max_idle = new_max_idle;

skip_limit_set:
	mutex_unlock(&powerclamp_lock);

	return ret;
}

static const struct kernel_param_ops max_idle_ops = {
	.set = max_idle_set,
	.get = param_get_int,
};

module_param_cb(max_idle, &max_idle_ops, &max_idle, 0644);
MODULE_PARM_DESC(max_idle, "maximum injected idle time to the total CPU time ratio in percent range:1-100");
=======
static const struct kernel_param_ops duration_ops = {
	.set = duration_set,
	.get = param_get_int,
};


module_param_cb(duration, &duration_ops, &duration, 0644);
MODULE_PARM_DESC(duration, "forced idle time for each attempt in msec.");
>>>>>>> b7ba80a49124 (Commit)

struct powerclamp_calibration_data {
	unsigned long confidence;  /* used for calibration, basically a counter
				    * gets incremented each time a clamping
				    * period is completed without extra wakeups
				    * once that counter is reached given level,
				    * compensation is deemed usable.
				    */
	unsigned long steady_comp; /* steady state compensation used when
				    * no extra wakeups occurred.
				    */
	unsigned long dynamic_comp; /* compensate excessive wakeup from idle
				     * mostly from external interrupts.
				     */
};

static struct powerclamp_calibration_data cal_data[MAX_TARGET_RATIO];

static int window_size_set(const char *arg, const struct kernel_param *kp)
{
	int ret = 0;
	unsigned long new_window_size;

	ret = kstrtoul(arg, 10, &new_window_size);
	if (ret)
		goto exit_win;
	if (new_window_size > 10 || new_window_size < 2) {
		pr_err("Out of recommended window size %lu, between 2-10\n",
			new_window_size);
		ret = -EINVAL;
	}

	window_size = clamp(new_window_size, 2ul, 10ul);
	smp_mb();

exit_win:

	return ret;
}

static const struct kernel_param_ops window_size_ops = {
	.set = window_size_set,
	.get = param_get_int,
};

module_param_cb(window_size, &window_size_ops, &window_size, 0644);
MODULE_PARM_DESC(window_size, "sliding window in number of clamping cycles\n"
	"\tpowerclamp controls idle ratio within this window. larger\n"
	"\twindow size results in slower response time but more smooth\n"
	"\tclamping results. default to 2.");

static void find_target_mwait(void)
{
	unsigned int eax, ebx, ecx, edx;
	unsigned int highest_cstate = 0;
	unsigned int highest_subcstate = 0;
	int i;

	if (boot_cpu_data.cpuid_level < CPUID_MWAIT_LEAF)
		return;

	cpuid(CPUID_MWAIT_LEAF, &eax, &ebx, &ecx, &edx);

	if (!(ecx & CPUID5_ECX_EXTENSIONS_SUPPORTED) ||
	    !(ecx & CPUID5_ECX_INTERRUPT_BREAK))
		return;

	edx >>= MWAIT_SUBSTATE_SIZE;
	for (i = 0; i < 7 && edx; i++, edx >>= MWAIT_SUBSTATE_SIZE) {
		if (edx & MWAIT_SUBSTATE_MASK) {
			highest_cstate = i;
			highest_subcstate = edx & MWAIT_SUBSTATE_MASK;
		}
	}
	target_mwait = (highest_cstate << MWAIT_SUBSTATE_SIZE) |
		(highest_subcstate - 1);

}

struct pkg_cstate_info {
	bool skip;
	int msr_index;
	int cstate_id;
};

#define PKG_CSTATE_INIT(id) {				\
		.msr_index = MSR_PKG_C##id##_RESIDENCY, \
		.cstate_id = id				\
			}

static struct pkg_cstate_info pkg_cstates[] = {
	PKG_CSTATE_INIT(2),
	PKG_CSTATE_INIT(3),
	PKG_CSTATE_INIT(6),
	PKG_CSTATE_INIT(7),
	PKG_CSTATE_INIT(8),
	PKG_CSTATE_INIT(9),
	PKG_CSTATE_INIT(10),
	{NULL},
};

static bool has_pkg_state_counter(void)
{
	u64 val;
	struct pkg_cstate_info *info = pkg_cstates;

	/* check if any one of the counter msrs exists */
	while (info->msr_index) {
		if (!rdmsrl_safe(info->msr_index, &val))
			return true;
		info++;
	}

	return false;
}

static u64 pkg_state_counter(void)
{
	u64 val;
	u64 count = 0;
	struct pkg_cstate_info *info = pkg_cstates;

	while (info->msr_index) {
		if (!info->skip) {
			if (!rdmsrl_safe(info->msr_index, &val))
				count += val;
			else
				info->skip = true;
		}
		info++;
	}

	return count;
}

static unsigned int get_compensation(int ratio)
{
	unsigned int comp = 0;

<<<<<<< HEAD
	if (!poll_pkg_cstate_enable)
		return 0;

=======
>>>>>>> b7ba80a49124 (Commit)
	/* we only use compensation if all adjacent ones are good */
	if (ratio == 1 &&
		cal_data[ratio].confidence >= CONFIDENCE_OK &&
		cal_data[ratio + 1].confidence >= CONFIDENCE_OK &&
		cal_data[ratio + 2].confidence >= CONFIDENCE_OK) {
		comp = (cal_data[ratio].steady_comp +
			cal_data[ratio + 1].steady_comp +
			cal_data[ratio + 2].steady_comp) / 3;
	} else if (ratio == MAX_TARGET_RATIO - 1 &&
		cal_data[ratio].confidence >= CONFIDENCE_OK &&
		cal_data[ratio - 1].confidence >= CONFIDENCE_OK &&
		cal_data[ratio - 2].confidence >= CONFIDENCE_OK) {
		comp = (cal_data[ratio].steady_comp +
			cal_data[ratio - 1].steady_comp +
			cal_data[ratio - 2].steady_comp) / 3;
	} else if (cal_data[ratio].confidence >= CONFIDENCE_OK &&
		cal_data[ratio - 1].confidence >= CONFIDENCE_OK &&
		cal_data[ratio + 1].confidence >= CONFIDENCE_OK) {
		comp = (cal_data[ratio].steady_comp +
			cal_data[ratio - 1].steady_comp +
			cal_data[ratio + 1].steady_comp) / 3;
	}

<<<<<<< HEAD
=======
	/* REVISIT: simple penalty of double idle injection */
	if (reduce_irq)
		comp = ratio;
>>>>>>> b7ba80a49124 (Commit)
	/* do not exceed limit */
	if (comp + ratio >= MAX_TARGET_RATIO)
		comp = MAX_TARGET_RATIO - ratio - 1;

	return comp;
}

static void adjust_compensation(int target_ratio, unsigned int win)
{
	int delta;
	struct powerclamp_calibration_data *d = &cal_data[target_ratio];

	/*
<<<<<<< HEAD
	 * adjust compensations if confidence level has not been reached.
	 */
	if (d->confidence >= CONFIDENCE_OK)
		return;

	delta = powerclamp_data.target_ratio - current_ratio;
=======
	 * adjust compensations if confidence level has not been reached or
	 * there are too many wakeups during the last idle injection period, we
	 * cannot trust the data for compensation.
	 */
	if (d->confidence >= CONFIDENCE_OK ||
		atomic_read(&idle_wakeup_counter) >
		win * num_online_cpus())
		return;

	delta = set_target_ratio - current_ratio;
>>>>>>> b7ba80a49124 (Commit)
	/* filter out bad data */
	if (delta >= 0 && delta <= (1+target_ratio/10)) {
		if (d->steady_comp)
			d->steady_comp =
				roundup(delta+d->steady_comp, 2)/2;
		else
			d->steady_comp = delta;
		d->confidence++;
	}
}

static bool powerclamp_adjust_controls(unsigned int target_ratio,
				unsigned int guard, unsigned int win)
{
	static u64 msr_last, tsc_last;
	u64 msr_now, tsc_now;
	u64 val64;

	/* check result for the last window */
	msr_now = pkg_state_counter();
	tsc_now = rdtsc();

	/* calculate pkg cstate vs tsc ratio */
	if (!msr_last || !tsc_last)
		current_ratio = 1;
	else if (tsc_now-tsc_last) {
		val64 = 100*(msr_now-msr_last);
		do_div(val64, (tsc_now-tsc_last));
		current_ratio = val64;
	}

	/* update record */
	msr_last = msr_now;
	tsc_last = tsc_now;

	adjust_compensation(target_ratio, win);
<<<<<<< HEAD

	/* if we are above target+guard, skip */
	return powerclamp_data.target_ratio + guard <= current_ratio;
}

/*
 * This function calculates runtime from the current target ratio.
 * This function gets called under powerclamp_lock.
 */
static unsigned int get_run_time(void)
{
	unsigned int compensated_ratio;
	unsigned int runtime;
=======
	/*
	 * too many external interrupts, set flag such
	 * that we can take measure later.
	 */
	reduce_irq = atomic_read(&idle_wakeup_counter) >=
		2 * win * num_online_cpus();

	atomic_set(&idle_wakeup_counter, 0);
	/* if we are above target+guard, skip */
	return set_target_ratio + guard <= current_ratio;
}

static void clamp_balancing_func(struct kthread_work *work)
{
	struct powerclamp_worker_data *w_data;
	int sleeptime;
	unsigned long target_jiffies;
	unsigned int compensated_ratio;
	int interval; /* jiffies to sleep for each attempt */

	w_data = container_of(work, struct powerclamp_worker_data,
			      balancing_work);
>>>>>>> b7ba80a49124 (Commit)

	/*
	 * make sure user selected ratio does not take effect until
	 * the next round. adjust target_ratio if user has changed
	 * target such that we can converge quickly.
	 */
<<<<<<< HEAD
	powerclamp_data.guard = 1 + powerclamp_data.target_ratio / 20;
	powerclamp_data.window_size_now = window_size;
=======
	w_data->target_ratio = READ_ONCE(set_target_ratio);
	w_data->guard = 1 + w_data->target_ratio / 20;
	w_data->window_size_now = window_size;
	w_data->duration_jiffies = msecs_to_jiffies(duration);
	w_data->count++;
>>>>>>> b7ba80a49124 (Commit)

	/*
	 * systems may have different ability to enter package level
	 * c-states, thus we need to compensate the injected idle ratio
	 * to achieve the actual target reported by the HW.
	 */
<<<<<<< HEAD
	compensated_ratio = powerclamp_data.target_ratio +
		get_compensation(powerclamp_data.target_ratio);
	if (compensated_ratio <= 0)
		compensated_ratio = 1;

	runtime = duration * 100 / compensated_ratio - duration;

	return runtime;
=======
	compensated_ratio = w_data->target_ratio +
		get_compensation(w_data->target_ratio);
	if (compensated_ratio <= 0)
		compensated_ratio = 1;
	interval = w_data->duration_jiffies * 100 / compensated_ratio;

	/* align idle time */
	target_jiffies = roundup(jiffies, interval);
	sleeptime = target_jiffies - jiffies;
	if (sleeptime <= 0)
		sleeptime = 1;

	if (clamping && w_data->clamping && cpu_online(w_data->cpu))
		kthread_queue_delayed_work(w_data->worker,
					   &w_data->idle_injection_work,
					   sleeptime);
}

static void clamp_idle_injection_func(struct kthread_work *work)
{
	struct powerclamp_worker_data *w_data;

	w_data = container_of(work, struct powerclamp_worker_data,
			      idle_injection_work.work);

	/*
	 * only elected controlling cpu can collect stats and update
	 * control parameters.
	 */
	if (w_data->cpu == control_cpu &&
	    !(w_data->count % w_data->window_size_now)) {
		should_skip =
			powerclamp_adjust_controls(w_data->target_ratio,
						   w_data->guard,
						   w_data->window_size_now);
		smp_mb();
	}

	if (should_skip)
		goto balance;

	play_idle(jiffies_to_usecs(w_data->duration_jiffies));

balance:
	if (clamping && w_data->clamping && cpu_online(w_data->cpu))
		kthread_queue_work(w_data->worker, &w_data->balancing_work);
>>>>>>> b7ba80a49124 (Commit)
}

/*
 * 1 HZ polling while clamping is active, useful for userspace
 * to monitor actual idle ratio.
 */
static void poll_pkg_cstate(struct work_struct *dummy);
static DECLARE_DELAYED_WORK(poll_pkg_cstate_work, poll_pkg_cstate);
static void poll_pkg_cstate(struct work_struct *dummy)
{
	static u64 msr_last;
	static u64 tsc_last;

	u64 msr_now;
	u64 tsc_now;
	u64 val64;

	msr_now = pkg_state_counter();
	tsc_now = rdtsc();

	/* calculate pkg cstate vs tsc ratio */
	if (!msr_last || !tsc_last)
		pkg_cstate_ratio_cur = 1;
	else {
		if (tsc_now - tsc_last) {
			val64 = 100 * (msr_now - msr_last);
			do_div(val64, (tsc_now - tsc_last));
			pkg_cstate_ratio_cur = val64;
		}
	}

	/* update record */
	msr_last = msr_now;
	tsc_last = tsc_now;

<<<<<<< HEAD
	mutex_lock(&powerclamp_lock);
	if (powerclamp_data.clamping)
		schedule_delayed_work(&poll_pkg_cstate_work, HZ);
	mutex_unlock(&powerclamp_lock);
}

static struct idle_inject_device *ii_dev;

/*
 * This function is called from idle injection core on timer expiry
 * for the run duration. This allows powerclamp to readjust or skip
 * injecting idle for this cycle.
 */
static bool idle_inject_update(void)
{
	bool update = false;

	/* We can't sleep in this callback */
	if (!mutex_trylock(&powerclamp_lock))
		return true;

	if (!(powerclamp_data.count % powerclamp_data.window_size_now)) {

		should_skip = powerclamp_adjust_controls(powerclamp_data.target_ratio,
							 powerclamp_data.guard,
							 powerclamp_data.window_size_now);
		update = true;
	}

	if (update) {
		unsigned int runtime = get_run_time();

		idle_inject_set_duration(ii_dev, runtime, duration);
	}

	powerclamp_data.count++;

	mutex_unlock(&powerclamp_lock);

	if (should_skip)
		return false;

	return true;
}

/* This function starts idle injection by calling idle_inject_start() */
static void trigger_idle_injection(void)
{
	unsigned int runtime = get_run_time();

	idle_inject_set_duration(ii_dev, runtime, duration);
	idle_inject_start(ii_dev);
	powerclamp_data.clamping = true;
}

/*
 * This function is called from start_power_clamp() to register
 * CPUS with powercap idle injection register and set default
 * idle duration and latency.
 */
static int powerclamp_idle_injection_register(void)
{
	poll_pkg_cstate_enable = false;
	if (cpumask_equal(cpu_present_mask, idle_injection_cpu_mask)) {
		ii_dev = idle_inject_register_full(idle_injection_cpu_mask, idle_inject_update);
		if (topology_max_packages() == 1 && topology_max_die_per_package() == 1)
			poll_pkg_cstate_enable = true;
	} else {
		ii_dev = idle_inject_register(idle_injection_cpu_mask);
	}

	if (!ii_dev) {
		pr_err("powerclamp: idle_inject_register failed\n");
		return -EAGAIN;
	}

	idle_inject_set_duration(ii_dev, TICK_USEC, duration);
	idle_inject_set_latency(ii_dev, UINT_MAX);
=======
	if (true == clamping)
		schedule_delayed_work(&poll_pkg_cstate_work, HZ);
}

static void start_power_clamp_worker(unsigned long cpu)
{
	struct powerclamp_worker_data *w_data = per_cpu_ptr(worker_data, cpu);
	struct kthread_worker *worker;

	worker = kthread_create_worker_on_cpu(cpu, 0, "kidle_inj/%ld", cpu);
	if (IS_ERR(worker))
		return;

	w_data->worker = worker;
	w_data->count = 0;
	w_data->cpu = cpu;
	w_data->clamping = true;
	set_bit(cpu, cpu_clamping_mask);
	sched_set_fifo(worker->task);
	kthread_init_work(&w_data->balancing_work, clamp_balancing_func);
	kthread_init_delayed_work(&w_data->idle_injection_work,
				  clamp_idle_injection_func);
	kthread_queue_work(w_data->worker, &w_data->balancing_work);
}

static void stop_power_clamp_worker(unsigned long cpu)
{
	struct powerclamp_worker_data *w_data = per_cpu_ptr(worker_data, cpu);

	if (!w_data->worker)
		return;

	w_data->clamping = false;
	/*
	 * Make sure that all works that get queued after this point see
	 * the clamping disabled. The counter part is not needed because
	 * there is an implicit memory barrier when the queued work
	 * is proceed.
	 */
	smp_wmb();
	kthread_cancel_work_sync(&w_data->balancing_work);
	kthread_cancel_delayed_work_sync(&w_data->idle_injection_work);
	/*
	 * The balancing work still might be queued here because
	 * the handling of the "clapming" variable, cancel, and queue
	 * operations are not synchronized via a lock. But it is not
	 * a big deal. The balancing work is fast and destroy kthread
	 * will wait for it.
	 */
	clear_bit(w_data->cpu, cpu_clamping_mask);
	kthread_destroy_worker(w_data->worker);

	w_data->worker = NULL;
}

static int start_power_clamp(void)
{
	unsigned long cpu;

	set_target_ratio = clamp(set_target_ratio, 0U, MAX_TARGET_RATIO - 1);
	/* prevent cpu hotplug */
	cpus_read_lock();

	/* prefer BSP */
	control_cpu = 0;
	if (!cpu_online(control_cpu))
		control_cpu = smp_processor_id();

	clamping = true;
	schedule_delayed_work(&poll_pkg_cstate_work, 0);

	/* start one kthread worker per online cpu */
	for_each_online_cpu(cpu) {
		start_power_clamp_worker(cpu);
	}
	cpus_read_unlock();
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

<<<<<<< HEAD
/*
 * This function is called from end_power_clamp() to stop idle injection
 * and unregister CPUS from powercap idle injection core.
 */
static void remove_idle_injection(void)
{
	if (!powerclamp_data.clamping)
		return;

	powerclamp_data.clamping = false;
	idle_inject_stop(ii_dev);
}

/*
 * This function is called when user change the cooling device
 * state from zero to some other value.
 */
static int start_power_clamp(void)
{
	int ret;

	ret = powerclamp_idle_injection_register();
	if (!ret) {
		trigger_idle_injection();
		if (poll_pkg_cstate_enable)
			schedule_delayed_work(&poll_pkg_cstate_work, 0);
	}

	return ret;
}

/*
 * This function is called when user change the cooling device
 * state from non zero value zero.
 */
static void end_power_clamp(void)
{
	if (powerclamp_data.clamping) {
		remove_idle_injection();
		idle_inject_unregister(ii_dev);
	}
}

=======
static void end_power_clamp(void)
{
	int i;

	/*
	 * Block requeuing in all the kthread workers. They will flush and
	 * stop faster.
	 */
	clamping = false;
	for_each_set_bit(i, cpu_clamping_mask, num_possible_cpus()) {
		pr_debug("clamping worker for cpu %d alive, destroy\n", i);
		stop_power_clamp_worker(i);
	}
}

static int powerclamp_cpu_online(unsigned int cpu)
{
	if (clamping == false)
		return 0;
	start_power_clamp_worker(cpu);
	/* prefer BSP as controlling CPU */
	if (cpu == 0) {
		control_cpu = 0;
		smp_mb();
	}
	return 0;
}

static int powerclamp_cpu_predown(unsigned int cpu)
{
	if (clamping == false)
		return 0;

	stop_power_clamp_worker(cpu);
	if (cpu != control_cpu)
		return 0;

	control_cpu = cpumask_first(cpu_online_mask);
	if (control_cpu == cpu)
		control_cpu = cpumask_next(cpu, cpu_online_mask);
	smp_mb();
	return 0;
}

>>>>>>> b7ba80a49124 (Commit)
static int powerclamp_get_max_state(struct thermal_cooling_device *cdev,
				 unsigned long *state)
{
	*state = MAX_TARGET_RATIO;

	return 0;
}

static int powerclamp_get_cur_state(struct thermal_cooling_device *cdev,
				 unsigned long *state)
{
<<<<<<< HEAD
	mutex_lock(&powerclamp_lock);
	*state = powerclamp_data.target_ratio;
	mutex_unlock(&powerclamp_lock);
=======
	if (true == clamping)
		*state = pkg_cstate_ratio_cur;
	else
		/* to save power, do not poll idle ratio while not clamping */
		*state = -1; /* indicates invalid state */
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

static int powerclamp_set_cur_state(struct thermal_cooling_device *cdev,
				 unsigned long new_target_ratio)
{
	int ret = 0;

<<<<<<< HEAD
	mutex_lock(&powerclamp_lock);

	new_target_ratio = clamp(new_target_ratio, 0UL,
				(unsigned long) (max_idle - 1));
	if (!powerclamp_data.target_ratio && new_target_ratio > 0) {
		pr_info("Start idle injection to reduce power\n");
		powerclamp_data.target_ratio = new_target_ratio;
		ret = start_power_clamp();
		if (ret)
			powerclamp_data.target_ratio = 0;
		goto exit_set;
	} else	if (powerclamp_data.target_ratio > 0 && new_target_ratio == 0) {
		pr_info("Stop forced idle injection\n");
		end_power_clamp();
		powerclamp_data.target_ratio = 0;
	} else	/* adjust currently running */ {
		unsigned int runtime;

		powerclamp_data.target_ratio = new_target_ratio;
		runtime = get_run_time();
		idle_inject_set_duration(ii_dev, runtime, duration);
	}

exit_set:
	mutex_unlock(&powerclamp_lock);

=======
	new_target_ratio = clamp(new_target_ratio, 0UL,
				(unsigned long) (MAX_TARGET_RATIO-1));
	if (set_target_ratio == 0 && new_target_ratio > 0) {
		pr_info("Start idle injection to reduce power\n");
		set_target_ratio = new_target_ratio;
		ret = start_power_clamp();
		goto exit_set;
	} else	if (set_target_ratio > 0 && new_target_ratio == 0) {
		pr_info("Stop forced idle injection\n");
		end_power_clamp();
		set_target_ratio = 0;
	} else	/* adjust currently running */ {
		set_target_ratio = new_target_ratio;
		/* make new set_target_ratio visible to other cpus */
		smp_mb();
	}

exit_set:
>>>>>>> b7ba80a49124 (Commit)
	return ret;
}

/* bind to generic thermal layer as cooling device*/
static const struct thermal_cooling_device_ops powerclamp_cooling_ops = {
	.get_max_state = powerclamp_get_max_state,
	.get_cur_state = powerclamp_get_cur_state,
	.set_cur_state = powerclamp_set_cur_state,
};

static const struct x86_cpu_id __initconst intel_powerclamp_ids[] = {
	X86_MATCH_VENDOR_FEATURE(INTEL, X86_FEATURE_MWAIT, NULL),
	{}
};
MODULE_DEVICE_TABLE(x86cpu, intel_powerclamp_ids);

static int __init powerclamp_probe(void)
{

	if (!x86_match_cpu(intel_powerclamp_ids)) {
		pr_err("CPU does not support MWAIT\n");
		return -ENODEV;
	}

	/* The goal for idle time alignment is to achieve package cstate. */
	if (!has_pkg_state_counter()) {
		pr_info("No package C-state available\n");
		return -ENODEV;
	}

	/* find the deepest mwait value */
	find_target_mwait();

	return 0;
}

static int powerclamp_debug_show(struct seq_file *m, void *unused)
{
	int i = 0;

<<<<<<< HEAD
=======
	seq_printf(m, "controlling cpu: %d\n", control_cpu);
>>>>>>> b7ba80a49124 (Commit)
	seq_printf(m, "pct confidence steady dynamic (compensation)\n");
	for (i = 0; i < MAX_TARGET_RATIO; i++) {
		seq_printf(m, "%d\t%lu\t%lu\t%lu\n",
			i,
			cal_data[i].confidence,
			cal_data[i].steady_comp,
			cal_data[i].dynamic_comp);
	}

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(powerclamp_debug);

static inline void powerclamp_create_debug_files(void)
{
	debug_dir = debugfs_create_dir("intel_powerclamp", NULL);

	debugfs_create_file("powerclamp_calib", S_IRUGO, debug_dir, cal_data,
			    &powerclamp_debug_fops);
}

<<<<<<< HEAD
=======
static enum cpuhp_state hp_state;

>>>>>>> b7ba80a49124 (Commit)
static int __init powerclamp_init(void)
{
	int retval;

<<<<<<< HEAD
	/* probe cpu features and ids here */
	retval = powerclamp_probe();
	if (retval)
		return retval;

	mutex_lock(&powerclamp_lock);
	retval = allocate_copy_idle_injection_mask(cpu_present_mask);
	mutex_unlock(&powerclamp_lock);

	if (retval)
		return retval;

	/* set default limit, maybe adjusted during runtime based on feedback */
	window_size = 2;

	cooling_dev = thermal_cooling_device_register("intel_powerclamp", NULL,
						      &powerclamp_cooling_ops);
	if (IS_ERR(cooling_dev))
		return -ENODEV;

	if (!duration)
		duration = jiffies_to_usecs(DEFAULT_DURATION_JIFFIES);
=======
	cpu_clamping_mask = bitmap_zalloc(num_possible_cpus(), GFP_KERNEL);
	if (!cpu_clamping_mask)
		return -ENOMEM;

	/* probe cpu features and ids here */
	retval = powerclamp_probe();
	if (retval)
		goto exit_free;

	/* set default limit, maybe adjusted during runtime based on feedback */
	window_size = 2;
	retval = cpuhp_setup_state_nocalls(CPUHP_AP_ONLINE_DYN,
					   "thermal/intel_powerclamp:online",
					   powerclamp_cpu_online,
					   powerclamp_cpu_predown);
	if (retval < 0)
		goto exit_free;

	hp_state = retval;

	worker_data = alloc_percpu(struct powerclamp_worker_data);
	if (!worker_data) {
		retval = -ENOMEM;
		goto exit_unregister;
	}

	cooling_dev = thermal_cooling_device_register("intel_powerclamp", NULL,
						&powerclamp_cooling_ops);
	if (IS_ERR(cooling_dev)) {
		retval = -ENODEV;
		goto exit_free_thread;
	}

	if (!duration)
		duration = jiffies_to_msecs(DEFAULT_DURATION_JIFFIES);
>>>>>>> b7ba80a49124 (Commit)

	powerclamp_create_debug_files();

	return 0;
<<<<<<< HEAD
=======

exit_free_thread:
	free_percpu(worker_data);
exit_unregister:
	cpuhp_remove_state_nocalls(hp_state);
exit_free:
	bitmap_free(cpu_clamping_mask);
	return retval;
>>>>>>> b7ba80a49124 (Commit)
}
module_init(powerclamp_init);

static void __exit powerclamp_exit(void)
{
<<<<<<< HEAD
	mutex_lock(&powerclamp_lock);
	end_power_clamp();
	mutex_unlock(&powerclamp_lock);

	thermal_cooling_device_unregister(cooling_dev);

	cancel_delayed_work_sync(&poll_pkg_cstate_work);
	debugfs_remove_recursive(debug_dir);

	if (cpumask_available(idle_injection_cpu_mask))
		free_cpumask_var(idle_injection_cpu_mask);
}
module_exit(powerclamp_exit);

MODULE_IMPORT_NS(IDLE_INJECT);

=======
	end_power_clamp();
	cpuhp_remove_state_nocalls(hp_state);
	free_percpu(worker_data);
	thermal_cooling_device_unregister(cooling_dev);
	bitmap_free(cpu_clamping_mask);

	cancel_delayed_work_sync(&poll_pkg_cstate_work);
	debugfs_remove_recursive(debug_dir);
}
module_exit(powerclamp_exit);

>>>>>>> b7ba80a49124 (Commit)
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arjan van de Ven <arjan@linux.intel.com>");
MODULE_AUTHOR("Jacob Pan <jacob.jun.pan@linux.intel.com>");
MODULE_DESCRIPTION("Package Level C-state Idle Injection for Intel CPUs");
