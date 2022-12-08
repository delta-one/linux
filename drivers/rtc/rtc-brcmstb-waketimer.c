// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright © 2014-2017 Broadcom
 */

#define pr_fmt(fmt)	KBUILD_MODNAME ": " fmt

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irqreturn.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/pm_wakeup.h>
#include <linux/reboot.h>
#include <linux/rtc.h>
#include <linux/stat.h>
#include <linux/suspend.h>

struct brcmstb_waketmr {
	struct rtc_device *rtc;
	struct device *dev;
	void __iomem *base;
<<<<<<< HEAD
	unsigned int wake_irq;
	unsigned int alarm_irq;
	struct notifier_block reboot_notifier;
	struct clk *clk;
	u32 rate;
	unsigned long rtc_alarm;
	bool alarm_en;
};

#define BRCMSTB_WKTMR_EVENT		0x00
#define  WKTMR_ALARM_EVENT		BIT(0)
=======
	int irq;
	struct notifier_block reboot_notifier;
	struct clk *clk;
	u32 rate;
};

#define BRCMSTB_WKTMR_EVENT		0x00
>>>>>>> b7ba80a49124 (Commit)
#define BRCMSTB_WKTMR_COUNTER		0x04
#define BRCMSTB_WKTMR_ALARM		0x08
#define BRCMSTB_WKTMR_PRESCALER		0x0C
#define BRCMSTB_WKTMR_PRESCALER_VAL	0x10

#define BRCMSTB_WKTMR_DEFAULT_FREQ	27000000

<<<<<<< HEAD
static inline bool brcmstb_waketmr_is_pending(struct brcmstb_waketmr *timer)
{
	u32 reg;

	reg = readl_relaxed(timer->base + BRCMSTB_WKTMR_EVENT);
	return !!(reg & WKTMR_ALARM_EVENT);
}

static inline void brcmstb_waketmr_clear_alarm(struct brcmstb_waketmr *timer)
{
	u32 reg;

	if (timer->alarm_en && timer->alarm_irq)
		disable_irq(timer->alarm_irq);
	timer->alarm_en = false;
	reg = readl_relaxed(timer->base + BRCMSTB_WKTMR_COUNTER);
	writel_relaxed(reg - 1, timer->base + BRCMSTB_WKTMR_ALARM);
	writel_relaxed(WKTMR_ALARM_EVENT, timer->base + BRCMSTB_WKTMR_EVENT);
=======
static inline void brcmstb_waketmr_clear_alarm(struct brcmstb_waketmr *timer)
{
	writel_relaxed(1, timer->base + BRCMSTB_WKTMR_EVENT);
>>>>>>> b7ba80a49124 (Commit)
	(void)readl_relaxed(timer->base + BRCMSTB_WKTMR_EVENT);
}

static void brcmstb_waketmr_set_alarm(struct brcmstb_waketmr *timer,
				      unsigned int secs)
{
<<<<<<< HEAD
	unsigned int now;

=======
>>>>>>> b7ba80a49124 (Commit)
	brcmstb_waketmr_clear_alarm(timer);

	/* Make sure we are actually counting in seconds */
	writel_relaxed(timer->rate, timer->base + BRCMSTB_WKTMR_PRESCALER);

<<<<<<< HEAD
	writel_relaxed(secs, timer->base + BRCMSTB_WKTMR_ALARM);
	now = readl_relaxed(timer->base + BRCMSTB_WKTMR_COUNTER);

	while ((int)(secs - now) <= 0 &&
		!brcmstb_waketmr_is_pending(timer)) {
		secs = now + 1;
		writel_relaxed(secs, timer->base + BRCMSTB_WKTMR_ALARM);
		now = readl_relaxed(timer->base + BRCMSTB_WKTMR_COUNTER);
	}
=======
	writel_relaxed(secs + 1, timer->base + BRCMSTB_WKTMR_ALARM);
>>>>>>> b7ba80a49124 (Commit)
}

static irqreturn_t brcmstb_waketmr_irq(int irq, void *data)
{
	struct brcmstb_waketmr *timer = data;

<<<<<<< HEAD
	if (!timer->alarm_irq)
		pm_wakeup_event(timer->dev, 0);
	return IRQ_HANDLED;
}

static irqreturn_t brcmstb_alarm_irq(int irq, void *data)
{
	struct brcmstb_waketmr *timer = data;

	/* Ignore spurious interrupts */
	if (!brcmstb_waketmr_is_pending(timer))
		return IRQ_HANDLED;

	if (timer->alarm_en) {
		if (!device_may_wakeup(timer->dev))
			writel_relaxed(WKTMR_ALARM_EVENT,
				       timer->base + BRCMSTB_WKTMR_EVENT);
		rtc_update_irq(timer->rtc, 1, RTC_IRQF | RTC_AF);
	}
=======
	pm_wakeup_event(timer->dev, 0);
>>>>>>> b7ba80a49124 (Commit)

	return IRQ_HANDLED;
}

struct wktmr_time {
	u32 sec;
	u32 pre;
};

static void wktmr_read(struct brcmstb_waketmr *timer,
		       struct wktmr_time *t)
{
	u32 tmp;

	do {
		t->sec = readl_relaxed(timer->base + BRCMSTB_WKTMR_COUNTER);
		tmp = readl_relaxed(timer->base + BRCMSTB_WKTMR_PRESCALER_VAL);
	} while (tmp >= timer->rate);

	t->pre = timer->rate - tmp;
}

static int brcmstb_waketmr_prepare_suspend(struct brcmstb_waketmr *timer)
{
	struct device *dev = timer->dev;
<<<<<<< HEAD
	int ret;

	if (device_may_wakeup(dev)) {
		ret = enable_irq_wake(timer->wake_irq);
=======
	int ret = 0;

	if (device_may_wakeup(dev)) {
		ret = enable_irq_wake(timer->irq);
>>>>>>> b7ba80a49124 (Commit)
		if (ret) {
			dev_err(dev, "failed to enable wake-up interrupt\n");
			return ret;
		}
<<<<<<< HEAD
		if (timer->alarm_en && timer->alarm_irq) {
			ret = enable_irq_wake(timer->alarm_irq);
			if (ret) {
				dev_err(dev, "failed to enable rtc interrupt\n");
				disable_irq_wake(timer->wake_irq);
				return ret;
			}
		}
	}

	return 0;
=======
	}

	return ret;
>>>>>>> b7ba80a49124 (Commit)
}

/* If enabled as a wakeup-source, arm the timer when powering off */
static int brcmstb_waketmr_reboot(struct notifier_block *nb,
		unsigned long action, void *data)
{
	struct brcmstb_waketmr *timer;

	timer = container_of(nb, struct brcmstb_waketmr, reboot_notifier);

	/* Set timer for cold boot */
	if (action == SYS_POWER_OFF)
		brcmstb_waketmr_prepare_suspend(timer);

	return NOTIFY_DONE;
}

static int brcmstb_waketmr_gettime(struct device *dev,
				   struct rtc_time *tm)
{
	struct brcmstb_waketmr *timer = dev_get_drvdata(dev);
	struct wktmr_time now;

	wktmr_read(timer, &now);

	rtc_time64_to_tm(now.sec, tm);

	return 0;
}

static int brcmstb_waketmr_settime(struct device *dev,
				   struct rtc_time *tm)
{
	struct brcmstb_waketmr *timer = dev_get_drvdata(dev);
	time64_t sec;

	sec = rtc_tm_to_time64(tm);

	writel_relaxed(sec, timer->base + BRCMSTB_WKTMR_COUNTER);

	return 0;
}

static int brcmstb_waketmr_getalarm(struct device *dev,
				    struct rtc_wkalrm *alarm)
{
	struct brcmstb_waketmr *timer = dev_get_drvdata(dev);
<<<<<<< HEAD

	alarm->enabled = timer->alarm_en;
	rtc_time64_to_tm(timer->rtc_alarm, &alarm->time);

	alarm->pending = brcmstb_waketmr_is_pending(timer);

	return 0;
}

static int brcmstb_waketmr_alarm_enable(struct device *dev,
					unsigned int enabled)
{
	struct brcmstb_waketmr *timer = dev_get_drvdata(dev);

	if (enabled && !timer->alarm_en) {
		if ((int)(readl_relaxed(timer->base + BRCMSTB_WKTMR_COUNTER) -
		    readl_relaxed(timer->base + BRCMSTB_WKTMR_ALARM)) >= 0 &&
		    !brcmstb_waketmr_is_pending(timer))
			return -EINVAL;
		timer->alarm_en = true;
		if (timer->alarm_irq)
			enable_irq(timer->alarm_irq);
	} else if (!enabled && timer->alarm_en) {
		if (timer->alarm_irq)
			disable_irq(timer->alarm_irq);
		timer->alarm_en = false;
	}

=======
	time64_t sec;
	u32 reg;

	sec = readl_relaxed(timer->base + BRCMSTB_WKTMR_ALARM);
	if (sec != 0) {
		/* Alarm is enabled */
		alarm->enabled = 1;
		rtc_time64_to_tm(sec, &alarm->time);
	}

	reg = readl_relaxed(timer->base + BRCMSTB_WKTMR_EVENT);
	alarm->pending = !!(reg & 1);

>>>>>>> b7ba80a49124 (Commit)
	return 0;
}

static int brcmstb_waketmr_setalarm(struct device *dev,
				     struct rtc_wkalrm *alarm)
{
	struct brcmstb_waketmr *timer = dev_get_drvdata(dev);
<<<<<<< HEAD

	timer->rtc_alarm = rtc_tm_to_time64(&alarm->time);

	brcmstb_waketmr_set_alarm(timer, timer->rtc_alarm);

	return brcmstb_waketmr_alarm_enable(dev, alarm->enabled);
=======
	time64_t sec;

	if (alarm->enabled)
		sec = rtc_tm_to_time64(&alarm->time);
	else
		sec = 0;

	brcmstb_waketmr_set_alarm(timer, sec);

	return 0;
}

/*
 * Does not do much but keep the RTC class happy. We always support
 * alarms.
 */
static int brcmstb_waketmr_alarm_enable(struct device *dev,
					unsigned int enabled)
{
	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

static const struct rtc_class_ops brcmstb_waketmr_ops = {
	.read_time	= brcmstb_waketmr_gettime,
	.set_time	= brcmstb_waketmr_settime,
	.read_alarm	= brcmstb_waketmr_getalarm,
	.set_alarm	= brcmstb_waketmr_setalarm,
	.alarm_irq_enable = brcmstb_waketmr_alarm_enable,
};

static int brcmstb_waketmr_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct brcmstb_waketmr *timer;
	int ret;

	timer = devm_kzalloc(dev, sizeof(*timer), GFP_KERNEL);
	if (!timer)
		return -ENOMEM;

	platform_set_drvdata(pdev, timer);
	timer->dev = dev;

	timer->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(timer->base))
		return PTR_ERR(timer->base);

	timer->rtc = devm_rtc_allocate_device(dev);
	if (IS_ERR(timer->rtc))
		return PTR_ERR(timer->rtc);

	/*
	 * Set wakeup capability before requesting wakeup interrupt, so we can
	 * process boot-time "wakeups" (e.g., from S5 soft-off)
	 */
<<<<<<< HEAD
	device_init_wakeup(dev, true);

	ret = platform_get_irq(pdev, 0);
	if (ret < 0)
		return -ENODEV;
	timer->wake_irq = (unsigned int)ret;
=======
	device_set_wakeup_capable(dev, true);
	device_wakeup_enable(dev);

	timer->irq = platform_get_irq(pdev, 0);
	if (timer->irq < 0)
		return -ENODEV;
>>>>>>> b7ba80a49124 (Commit)

	timer->clk = devm_clk_get(dev, NULL);
	if (!IS_ERR(timer->clk)) {
		ret = clk_prepare_enable(timer->clk);
		if (ret)
			return ret;
		timer->rate = clk_get_rate(timer->clk);
		if (!timer->rate)
			timer->rate = BRCMSTB_WKTMR_DEFAULT_FREQ;
	} else {
		timer->rate = BRCMSTB_WKTMR_DEFAULT_FREQ;
		timer->clk = NULL;
	}

<<<<<<< HEAD
	ret = devm_request_irq(dev, timer->wake_irq, brcmstb_waketmr_irq, 0,
=======
	ret = devm_request_irq(dev, timer->irq, brcmstb_waketmr_irq, 0,
>>>>>>> b7ba80a49124 (Commit)
			       "brcmstb-waketimer", timer);
	if (ret < 0)
		goto err_clk;

<<<<<<< HEAD
	brcmstb_waketmr_clear_alarm(timer);

	/* Attempt to initialize non-wake irq */
	ret = platform_get_irq(pdev, 1);
	if (ret > 0) {
		timer->alarm_irq = (unsigned int)ret;
		ret = devm_request_irq(dev, timer->alarm_irq, brcmstb_alarm_irq,
				       IRQF_NO_AUTOEN, "brcmstb-waketimer-rtc",
				       timer);
		if (ret < 0)
			timer->alarm_irq = 0;
	}

=======
>>>>>>> b7ba80a49124 (Commit)
	timer->reboot_notifier.notifier_call = brcmstb_waketmr_reboot;
	register_reboot_notifier(&timer->reboot_notifier);

	timer->rtc->ops = &brcmstb_waketmr_ops;
	timer->rtc->range_max = U32_MAX;

	ret = devm_rtc_register_device(timer->rtc);
	if (ret)
		goto err_notifier;

<<<<<<< HEAD
=======
	dev_info(dev, "registered, with irq %d\n", timer->irq);

>>>>>>> b7ba80a49124 (Commit)
	return 0;

err_notifier:
	unregister_reboot_notifier(&timer->reboot_notifier);

err_clk:
	clk_disable_unprepare(timer->clk);

	return ret;
}

<<<<<<< HEAD
static void brcmstb_waketmr_remove(struct platform_device *pdev)
=======
static int brcmstb_waketmr_remove(struct platform_device *pdev)
>>>>>>> b7ba80a49124 (Commit)
{
	struct brcmstb_waketmr *timer = dev_get_drvdata(&pdev->dev);

	unregister_reboot_notifier(&timer->reboot_notifier);
	clk_disable_unprepare(timer->clk);
<<<<<<< HEAD
=======

	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

#ifdef CONFIG_PM_SLEEP
static int brcmstb_waketmr_suspend(struct device *dev)
{
	struct brcmstb_waketmr *timer = dev_get_drvdata(dev);

	return brcmstb_waketmr_prepare_suspend(timer);
}

static int brcmstb_waketmr_resume(struct device *dev)
{
	struct brcmstb_waketmr *timer = dev_get_drvdata(dev);
	int ret;

	if (!device_may_wakeup(dev))
		return 0;

<<<<<<< HEAD
	ret = disable_irq_wake(timer->wake_irq);
	if (timer->alarm_en && timer->alarm_irq)
		disable_irq_wake(timer->alarm_irq);
=======
	ret = disable_irq_wake(timer->irq);
>>>>>>> b7ba80a49124 (Commit)

	brcmstb_waketmr_clear_alarm(timer);

	return ret;
}
#endif /* CONFIG_PM_SLEEP */

static SIMPLE_DEV_PM_OPS(brcmstb_waketmr_pm_ops,
			 brcmstb_waketmr_suspend, brcmstb_waketmr_resume);

static const __maybe_unused struct of_device_id brcmstb_waketmr_of_match[] = {
	{ .compatible = "brcm,brcmstb-waketimer" },
	{ /* sentinel */ },
};

static struct platform_driver brcmstb_waketmr_driver = {
	.probe			= brcmstb_waketmr_probe,
<<<<<<< HEAD
	.remove_new		= brcmstb_waketmr_remove,
=======
	.remove			= brcmstb_waketmr_remove,
>>>>>>> b7ba80a49124 (Commit)
	.driver = {
		.name		= "brcmstb-waketimer",
		.pm		= &brcmstb_waketmr_pm_ops,
		.of_match_table	= of_match_ptr(brcmstb_waketmr_of_match),
	}
};
module_platform_driver(brcmstb_waketmr_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Brian Norris");
MODULE_AUTHOR("Markus Mayer");
<<<<<<< HEAD
MODULE_AUTHOR("Doug Berger");
=======
>>>>>>> b7ba80a49124 (Commit)
MODULE_DESCRIPTION("Wake-up timer driver for STB chips");
