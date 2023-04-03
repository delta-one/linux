// SPDX-License-Identifier: GPL-2.0-only
/*
 * Ralink MT7621/MT7628 built-in hardware watchdog timer
 *
 * Copyright (C) 2014 John Crispin <john@phrozen.org>
 *
 * This driver was based on: drivers/watchdog/rt2880_wdt.c
 */

#include <linux/clk.h>
#include <linux/reset.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/watchdog.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
<<<<<<< HEAD
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
=======

#include <asm/mach-ralink/ralink_regs.h>
>>>>>>> b7ba80a49124 (Commit)

#define SYSC_RSTSTAT			0x38
#define WDT_RST_CAUSE			BIT(1)

#define RALINK_WDT_TIMEOUT		30

#define TIMER_REG_TMRSTAT		0x00
#define TIMER_REG_TMR1LOAD		0x24
#define TIMER_REG_TMR1CTL		0x20

#define TMR1CTL_ENABLE			BIT(7)
#define TMR1CTL_RESTART			BIT(9)
#define TMR1CTL_PRESCALE_SHIFT		16

<<<<<<< HEAD
struct mt7621_wdt_data {
	void __iomem *base;
	struct reset_control *rst;
	struct regmap *sysc;
	struct watchdog_device wdt;
};
=======
static void __iomem *mt7621_wdt_base;
static struct reset_control *mt7621_wdt_reset;
>>>>>>> b7ba80a49124 (Commit)

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout,
		 "Watchdog cannot be stopped once started (default="
		 __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

<<<<<<< HEAD
static inline void rt_wdt_w32(void __iomem *base, unsigned int reg, u32 val)
{
	iowrite32(val, base + reg);
}

static inline u32 rt_wdt_r32(void __iomem *base, unsigned int reg)
{
	return ioread32(base + reg);
=======
static inline void rt_wdt_w32(unsigned reg, u32 val)
{
	iowrite32(val, mt7621_wdt_base + reg);
}

static inline u32 rt_wdt_r32(unsigned reg)
{
	return ioread32(mt7621_wdt_base + reg);
>>>>>>> b7ba80a49124 (Commit)
}

static int mt7621_wdt_ping(struct watchdog_device *w)
{
<<<<<<< HEAD
	struct mt7621_wdt_data *drvdata = watchdog_get_drvdata(w);

	rt_wdt_w32(drvdata->base, TIMER_REG_TMRSTAT, TMR1CTL_RESTART);
=======
	rt_wdt_w32(TIMER_REG_TMRSTAT, TMR1CTL_RESTART);
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

static int mt7621_wdt_set_timeout(struct watchdog_device *w, unsigned int t)
{
<<<<<<< HEAD
	struct mt7621_wdt_data *drvdata = watchdog_get_drvdata(w);

	w->timeout = t;
	rt_wdt_w32(drvdata->base, TIMER_REG_TMR1LOAD, t * 1000);
=======
	w->timeout = t;
	rt_wdt_w32(TIMER_REG_TMR1LOAD, t * 1000);
>>>>>>> b7ba80a49124 (Commit)
	mt7621_wdt_ping(w);

	return 0;
}

static int mt7621_wdt_start(struct watchdog_device *w)
{
<<<<<<< HEAD
	struct mt7621_wdt_data *drvdata = watchdog_get_drvdata(w);
	u32 t;

	/* set the prescaler to 1ms == 1000us */
	rt_wdt_w32(drvdata->base, TIMER_REG_TMR1CTL, 1000 << TMR1CTL_PRESCALE_SHIFT);

	mt7621_wdt_set_timeout(w, w->timeout);

	t = rt_wdt_r32(drvdata->base, TIMER_REG_TMR1CTL);
	t |= TMR1CTL_ENABLE;
	rt_wdt_w32(drvdata->base, TIMER_REG_TMR1CTL, t);
=======
	u32 t;

	/* set the prescaler to 1ms == 1000us */
	rt_wdt_w32(TIMER_REG_TMR1CTL, 1000 << TMR1CTL_PRESCALE_SHIFT);

	mt7621_wdt_set_timeout(w, w->timeout);

	t = rt_wdt_r32(TIMER_REG_TMR1CTL);
	t |= TMR1CTL_ENABLE;
	rt_wdt_w32(TIMER_REG_TMR1CTL, t);
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

static int mt7621_wdt_stop(struct watchdog_device *w)
{
<<<<<<< HEAD
	struct mt7621_wdt_data *drvdata = watchdog_get_drvdata(w);
=======
>>>>>>> b7ba80a49124 (Commit)
	u32 t;

	mt7621_wdt_ping(w);

<<<<<<< HEAD
	t = rt_wdt_r32(drvdata->base, TIMER_REG_TMR1CTL);
	t &= ~TMR1CTL_ENABLE;
	rt_wdt_w32(drvdata->base, TIMER_REG_TMR1CTL, t);
=======
	t = rt_wdt_r32(TIMER_REG_TMR1CTL);
	t &= ~TMR1CTL_ENABLE;
	rt_wdt_w32(TIMER_REG_TMR1CTL, t);
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

<<<<<<< HEAD
static int mt7621_wdt_bootcause(struct mt7621_wdt_data *d)
{
	u32 val;

	regmap_read(d->sysc, SYSC_RSTSTAT, &val);
	if (val & WDT_RST_CAUSE)
=======
static int mt7621_wdt_bootcause(void)
{
	if (rt_sysc_r32(SYSC_RSTSTAT) & WDT_RST_CAUSE)
>>>>>>> b7ba80a49124 (Commit)
		return WDIOF_CARDRESET;

	return 0;
}

static int mt7621_wdt_is_running(struct watchdog_device *w)
{
<<<<<<< HEAD
	struct mt7621_wdt_data *drvdata = watchdog_get_drvdata(w);

	return !!(rt_wdt_r32(drvdata->base, TIMER_REG_TMR1CTL) & TMR1CTL_ENABLE);
=======
	return !!(rt_wdt_r32(TIMER_REG_TMR1CTL) & TMR1CTL_ENABLE);
>>>>>>> b7ba80a49124 (Commit)
}

static const struct watchdog_info mt7621_wdt_info = {
	.identity = "Mediatek Watchdog",
	.options = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE,
};

static const struct watchdog_ops mt7621_wdt_ops = {
	.owner = THIS_MODULE,
	.start = mt7621_wdt_start,
	.stop = mt7621_wdt_stop,
	.ping = mt7621_wdt_ping,
	.set_timeout = mt7621_wdt_set_timeout,
};

<<<<<<< HEAD
static int mt7621_wdt_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct device *dev = &pdev->dev;
	struct watchdog_device *mt7621_wdt;
	struct mt7621_wdt_data *drvdata;
	int err;

	drvdata = devm_kzalloc(dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;

	drvdata->sysc = syscon_regmap_lookup_by_phandle(np, "mediatek,sysctl");
	if (IS_ERR(drvdata->sysc)) {
		drvdata->sysc = syscon_regmap_lookup_by_compatible("mediatek,mt7621-sysc");
		if (IS_ERR(drvdata->sysc))
			return PTR_ERR(drvdata->sysc);
	}

	drvdata->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(drvdata->base))
		return PTR_ERR(drvdata->base);

	drvdata->rst = devm_reset_control_get_exclusive(dev, NULL);
	if (!IS_ERR(drvdata->rst))
		reset_control_deassert(drvdata->rst);

	mt7621_wdt = &drvdata->wdt;
	mt7621_wdt->info = &mt7621_wdt_info;
	mt7621_wdt->ops = &mt7621_wdt_ops;
	mt7621_wdt->min_timeout = 1;
	mt7621_wdt->max_timeout = 0xfffful / 1000;
	mt7621_wdt->parent = dev;

	mt7621_wdt->bootstatus = mt7621_wdt_bootcause(drvdata);

	watchdog_init_timeout(mt7621_wdt, mt7621_wdt->max_timeout, dev);
	watchdog_set_nowayout(mt7621_wdt, nowayout);
	watchdog_set_drvdata(mt7621_wdt, drvdata);

	if (mt7621_wdt_is_running(mt7621_wdt)) {
=======
static struct watchdog_device mt7621_wdt_dev = {
	.info = &mt7621_wdt_info,
	.ops = &mt7621_wdt_ops,
	.min_timeout = 1,
	.max_timeout = 0xfffful / 1000,
};

static int mt7621_wdt_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	mt7621_wdt_base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(mt7621_wdt_base))
		return PTR_ERR(mt7621_wdt_base);

	mt7621_wdt_reset = devm_reset_control_get_exclusive(dev, NULL);
	if (!IS_ERR(mt7621_wdt_reset))
		reset_control_deassert(mt7621_wdt_reset);

	mt7621_wdt_dev.bootstatus = mt7621_wdt_bootcause();

	watchdog_init_timeout(&mt7621_wdt_dev, mt7621_wdt_dev.max_timeout,
			      dev);
	watchdog_set_nowayout(&mt7621_wdt_dev, nowayout);
	if (mt7621_wdt_is_running(&mt7621_wdt_dev)) {
>>>>>>> b7ba80a49124 (Commit)
		/*
		 * Make sure to apply timeout from watchdog core, taking
		 * the prescaler of this driver here into account (the
		 * boot loader might be using a different prescaler).
		 *
		 * To avoid spurious resets because of different scaling,
		 * we first disable the watchdog, set the new prescaler
		 * and timeout, and then re-enable the watchdog.
		 */
<<<<<<< HEAD
		mt7621_wdt_stop(mt7621_wdt);
		mt7621_wdt_start(mt7621_wdt);
		set_bit(WDOG_HW_RUNNING, &mt7621_wdt->status);
	}

	err = devm_watchdog_register_device(dev, &drvdata->wdt);
	if (err)
		return err;

	platform_set_drvdata(pdev, drvdata);

	return 0;
=======
		mt7621_wdt_stop(&mt7621_wdt_dev);
		mt7621_wdt_start(&mt7621_wdt_dev);
		set_bit(WDOG_HW_RUNNING, &mt7621_wdt_dev.status);
	}

	return devm_watchdog_register_device(dev, &mt7621_wdt_dev);
>>>>>>> b7ba80a49124 (Commit)
}

static void mt7621_wdt_shutdown(struct platform_device *pdev)
{
<<<<<<< HEAD
	struct mt7621_wdt_data *drvdata = platform_get_drvdata(pdev);

	mt7621_wdt_stop(&drvdata->wdt);
=======
	mt7621_wdt_stop(&mt7621_wdt_dev);
>>>>>>> b7ba80a49124 (Commit)
}

static const struct of_device_id mt7621_wdt_match[] = {
	{ .compatible = "mediatek,mt7621-wdt" },
	{},
};
MODULE_DEVICE_TABLE(of, mt7621_wdt_match);

static struct platform_driver mt7621_wdt_driver = {
	.probe		= mt7621_wdt_probe,
	.shutdown	= mt7621_wdt_shutdown,
	.driver		= {
		.name		= KBUILD_MODNAME,
		.of_match_table	= mt7621_wdt_match,
	},
};

module_platform_driver(mt7621_wdt_driver);

MODULE_DESCRIPTION("MediaTek MT762x hardware watchdog driver");
MODULE_AUTHOR("John Crispin <john@phrozen.org");
MODULE_LICENSE("GPL v2");
