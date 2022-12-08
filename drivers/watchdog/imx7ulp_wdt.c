// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright 2019 NXP.
 */

#include <linux/clk.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
<<<<<<< HEAD
#include <linux/of_device.h>
=======
>>>>>>> b7ba80a49124 (Commit)
#include <linux/platform_device.h>
#include <linux/reboot.h>
#include <linux/watchdog.h>

#define WDOG_CS			0x0
<<<<<<< HEAD
#define WDOG_CS_FLG		BIT(14)
#define WDOG_CS_CMD32EN		BIT(13)
#define WDOG_CS_PRES		BIT(12)
=======
#define WDOG_CS_CMD32EN		BIT(13)
>>>>>>> b7ba80a49124 (Commit)
#define WDOG_CS_ULK		BIT(11)
#define WDOG_CS_RCS		BIT(10)
#define LPO_CLK			0x1
#define LPO_CLK_SHIFT		8
#define WDOG_CS_CLK		(LPO_CLK << LPO_CLK_SHIFT)
#define WDOG_CS_EN		BIT(7)
#define WDOG_CS_UPDATE		BIT(5)
#define WDOG_CS_WAIT		BIT(1)
#define WDOG_CS_STOP		BIT(0)

#define WDOG_CNT	0x4
#define WDOG_TOVAL	0x8

#define REFRESH_SEQ0	0xA602
#define REFRESH_SEQ1	0xB480
#define REFRESH		((REFRESH_SEQ1 << 16) | REFRESH_SEQ0)

#define UNLOCK_SEQ0	0xC520
#define UNLOCK_SEQ1	0xD928
#define UNLOCK		((UNLOCK_SEQ1 << 16) | UNLOCK_SEQ0)

#define DEFAULT_TIMEOUT	60
#define MAX_TIMEOUT	128
#define WDOG_CLOCK_RATE	1000
<<<<<<< HEAD
#define WDOG_ULK_WAIT_TIMEOUT	1000
#define WDOG_RCS_WAIT_TIMEOUT	10000
#define WDOG_RCS_POST_WAIT 3000

#define RETRY_MAX 5
=======
#define WDOG_WAIT_TIMEOUT	20
>>>>>>> b7ba80a49124 (Commit)

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0000);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started (default="
		 __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

<<<<<<< HEAD
struct imx_wdt_hw_feature {
	bool prescaler_enable;
	u32 wdog_clock_rate;
};

=======
>>>>>>> b7ba80a49124 (Commit)
struct imx7ulp_wdt_device {
	struct watchdog_device wdd;
	void __iomem *base;
	struct clk *clk;
<<<<<<< HEAD
	bool post_rcs_wait;
	const struct imx_wdt_hw_feature *hw;
};

static int imx7ulp_wdt_wait_ulk(void __iomem *base)
{
	u32 val = readl(base + WDOG_CS);

	if (!(val & WDOG_CS_ULK) &&
	    readl_poll_timeout_atomic(base + WDOG_CS, val,
				      val & WDOG_CS_ULK, 0,
				      WDOG_ULK_WAIT_TIMEOUT))
=======
};

static int imx7ulp_wdt_wait(void __iomem *base, u32 mask)
{
	u32 val = readl(base + WDOG_CS);

	if (!(val & mask) && readl_poll_timeout_atomic(base + WDOG_CS, val,
						       val & mask, 0,
						       WDOG_WAIT_TIMEOUT))
>>>>>>> b7ba80a49124 (Commit)
		return -ETIMEDOUT;

	return 0;
}

<<<<<<< HEAD
static int imx7ulp_wdt_wait_rcs(struct imx7ulp_wdt_device *wdt)
{
	int ret = 0;
	u32 val = readl(wdt->base + WDOG_CS);
	u64 timeout = (val & WDOG_CS_PRES) ?
		WDOG_RCS_WAIT_TIMEOUT * 256 : WDOG_RCS_WAIT_TIMEOUT;
	unsigned long wait_min = (val & WDOG_CS_PRES) ?
		WDOG_RCS_POST_WAIT * 256 : WDOG_RCS_POST_WAIT;

	if (!(val & WDOG_CS_RCS) &&
	    readl_poll_timeout(wdt->base + WDOG_CS, val, val & WDOG_CS_RCS, 100,
			       timeout))
		ret = -ETIMEDOUT;

	/* Wait 2.5 clocks after RCS done */
	if (wdt->post_rcs_wait)
		usleep_range(wait_min, wait_min + 2000);

	return ret;
}

static int _imx7ulp_wdt_enable(struct imx7ulp_wdt_device *wdt, bool enable)
{
=======
static int imx7ulp_wdt_enable(struct watchdog_device *wdog, bool enable)
{
	struct imx7ulp_wdt_device *wdt = watchdog_get_drvdata(wdog);

>>>>>>> b7ba80a49124 (Commit)
	u32 val = readl(wdt->base + WDOG_CS);
	int ret;

	local_irq_disable();
	writel(UNLOCK, wdt->base + WDOG_CNT);
<<<<<<< HEAD
	ret = imx7ulp_wdt_wait_ulk(wdt->base);
=======
	ret = imx7ulp_wdt_wait(wdt->base, WDOG_CS_ULK);
>>>>>>> b7ba80a49124 (Commit)
	if (ret)
		goto enable_out;
	if (enable)
		writel(val | WDOG_CS_EN, wdt->base + WDOG_CS);
	else
		writel(val & ~WDOG_CS_EN, wdt->base + WDOG_CS);
<<<<<<< HEAD

	local_irq_enable();
	ret = imx7ulp_wdt_wait_rcs(wdt);

	return ret;

enable_out:
	local_irq_enable();
	return ret;
}

static int imx7ulp_wdt_enable(struct watchdog_device *wdog, bool enable)
{
	struct imx7ulp_wdt_device *wdt = watchdog_get_drvdata(wdog);
	int ret;
	u32 val;
	u32 loop = RETRY_MAX;

	do {
		ret = _imx7ulp_wdt_enable(wdt, enable);
		val = readl(wdt->base + WDOG_CS);
	} while (--loop > 0 && ((!!(val & WDOG_CS_EN)) != enable || ret));

	if (loop == 0)
		return -EBUSY;

	return ret;
=======
	imx7ulp_wdt_wait(wdt->base, WDOG_CS_RCS);

enable_out:
	local_irq_enable();

	return ret;
}

static bool imx7ulp_wdt_is_enabled(void __iomem *base)
{
	u32 val = readl(base + WDOG_CS);

	return val & WDOG_CS_EN;
>>>>>>> b7ba80a49124 (Commit)
}

static int imx7ulp_wdt_ping(struct watchdog_device *wdog)
{
	struct imx7ulp_wdt_device *wdt = watchdog_get_drvdata(wdog);

	writel(REFRESH, wdt->base + WDOG_CNT);

	return 0;
}

static int imx7ulp_wdt_start(struct watchdog_device *wdog)
{
	return imx7ulp_wdt_enable(wdog, true);
}

static int imx7ulp_wdt_stop(struct watchdog_device *wdog)
{
	return imx7ulp_wdt_enable(wdog, false);
}

<<<<<<< HEAD
static int _imx7ulp_wdt_set_timeout(struct imx7ulp_wdt_device *wdt,
				   unsigned int toval)
{
	int ret;

	local_irq_disable();
	writel(UNLOCK, wdt->base + WDOG_CNT);
	ret = imx7ulp_wdt_wait_ulk(wdt->base);
	if (ret)
		goto timeout_out;
	writel(toval, wdt->base + WDOG_TOVAL);
	local_irq_enable();
	ret = imx7ulp_wdt_wait_rcs(wdt);
	return ret;

timeout_out:
	local_irq_enable();
	return ret;
}

=======
>>>>>>> b7ba80a49124 (Commit)
static int imx7ulp_wdt_set_timeout(struct watchdog_device *wdog,
				   unsigned int timeout)
{
	struct imx7ulp_wdt_device *wdt = watchdog_get_drvdata(wdog);
<<<<<<< HEAD
	u32 toval = wdt->hw->wdog_clock_rate * timeout;
	u32 val;
	int ret;
	u32 loop = RETRY_MAX;

	do {
		ret = _imx7ulp_wdt_set_timeout(wdt, toval);
		val = readl(wdt->base + WDOG_TOVAL);
	} while (--loop > 0 && (val != toval || ret));

	if (loop == 0)
		return -EBUSY;

	wdog->timeout = timeout;
=======
	u32 val = WDOG_CLOCK_RATE * timeout;
	int ret;

	local_irq_disable();
	writel(UNLOCK, wdt->base + WDOG_CNT);
	ret = imx7ulp_wdt_wait(wdt->base, WDOG_CS_ULK);
	if (ret)
		goto timeout_out;
	writel(val, wdt->base + WDOG_TOVAL);
	imx7ulp_wdt_wait(wdt->base, WDOG_CS_RCS);

	wdog->timeout = timeout;

timeout_out:
	local_irq_enable();

>>>>>>> b7ba80a49124 (Commit)
	return ret;
}

static int imx7ulp_wdt_restart(struct watchdog_device *wdog,
			       unsigned long action, void *data)
{
	struct imx7ulp_wdt_device *wdt = watchdog_get_drvdata(wdog);
	int ret;

	ret = imx7ulp_wdt_enable(wdog, true);
	if (ret)
		return ret;

	ret = imx7ulp_wdt_set_timeout(&wdt->wdd, 1);
	if (ret)
		return ret;

	/* wait for wdog to fire */
	while (true)
		;

	return NOTIFY_DONE;
}

static const struct watchdog_ops imx7ulp_wdt_ops = {
	.owner = THIS_MODULE,
	.start = imx7ulp_wdt_start,
	.stop  = imx7ulp_wdt_stop,
	.ping  = imx7ulp_wdt_ping,
	.set_timeout = imx7ulp_wdt_set_timeout,
	.restart = imx7ulp_wdt_restart,
};

static const struct watchdog_info imx7ulp_wdt_info = {
	.identity = "i.MX7ULP watchdog timer",
	.options  = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING |
		    WDIOF_MAGICCLOSE,
};

<<<<<<< HEAD
static int _imx7ulp_wdt_init(struct imx7ulp_wdt_device *wdt, unsigned int timeout, unsigned int cs)
=======
static int imx7ulp_wdt_init(void __iomem *base, unsigned int timeout)
>>>>>>> b7ba80a49124 (Commit)
{
	u32 val;
	int ret;

	local_irq_disable();
<<<<<<< HEAD

	val = readl(wdt->base + WDOG_CS);
	if (val & WDOG_CS_CMD32EN) {
		writel(UNLOCK, wdt->base + WDOG_CNT);
	} else {
		mb();
		/* unlock the wdog for reconfiguration */
		writel_relaxed(UNLOCK_SEQ0, wdt->base + WDOG_CNT);
		writel_relaxed(UNLOCK_SEQ1, wdt->base + WDOG_CNT);
		mb();
	}

	ret = imx7ulp_wdt_wait_ulk(wdt->base);
=======
	/* unlock the wdog for reconfiguration */
	writel_relaxed(UNLOCK_SEQ0, base + WDOG_CNT);
	writel_relaxed(UNLOCK_SEQ1, base + WDOG_CNT);
	ret = imx7ulp_wdt_wait(base, WDOG_CS_ULK);
>>>>>>> b7ba80a49124 (Commit)
	if (ret)
		goto init_out;

	/* set an initial timeout value in TOVAL */
<<<<<<< HEAD
	writel(timeout, wdt->base + WDOG_TOVAL);
	writel(cs, wdt->base + WDOG_CS);
	local_irq_enable();
	ret = imx7ulp_wdt_wait_rcs(wdt);

	return ret;

init_out:
	local_irq_enable();
	return ret;
}

static int imx7ulp_wdt_init(struct imx7ulp_wdt_device *wdt, unsigned int timeout)
{
	/* enable 32bit command sequence and reconfigure */
	u32 val = WDOG_CS_CMD32EN | WDOG_CS_CLK | WDOG_CS_UPDATE |
		  WDOG_CS_WAIT | WDOG_CS_STOP;
	u32 cs, toval;
	int ret;
	u32 loop = RETRY_MAX;

	if (wdt->hw->prescaler_enable)
		val |= WDOG_CS_PRES;

	do {
		ret = _imx7ulp_wdt_init(wdt, timeout, val);
		toval = readl(wdt->base + WDOG_TOVAL);
		cs = readl(wdt->base + WDOG_CS);
		cs &= ~(WDOG_CS_FLG | WDOG_CS_ULK | WDOG_CS_RCS);
	} while (--loop > 0 && (cs != val || toval != timeout || ret));

	if (loop == 0)
		return -EBUSY;

	return ret;
=======
	writel(timeout, base + WDOG_TOVAL);
	/* enable 32bit command sequence and reconfigure */
	val = WDOG_CS_CMD32EN | WDOG_CS_CLK | WDOG_CS_UPDATE |
	      WDOG_CS_WAIT | WDOG_CS_STOP;
	writel(val, base + WDOG_CS);
	imx7ulp_wdt_wait(base, WDOG_CS_RCS);

init_out:
	local_irq_enable();

	return ret;
}

static void imx7ulp_wdt_action(void *data)
{
	clk_disable_unprepare(data);
>>>>>>> b7ba80a49124 (Commit)
}

static int imx7ulp_wdt_probe(struct platform_device *pdev)
{
	struct imx7ulp_wdt_device *imx7ulp_wdt;
	struct device *dev = &pdev->dev;
	struct watchdog_device *wdog;
	int ret;

	imx7ulp_wdt = devm_kzalloc(dev, sizeof(*imx7ulp_wdt), GFP_KERNEL);
	if (!imx7ulp_wdt)
		return -ENOMEM;

	platform_set_drvdata(pdev, imx7ulp_wdt);

	imx7ulp_wdt->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(imx7ulp_wdt->base))
		return PTR_ERR(imx7ulp_wdt->base);

<<<<<<< HEAD
	imx7ulp_wdt->clk = devm_clk_get_enabled(dev, NULL);
=======
	imx7ulp_wdt->clk = devm_clk_get(dev, NULL);
>>>>>>> b7ba80a49124 (Commit)
	if (IS_ERR(imx7ulp_wdt->clk)) {
		dev_err(dev, "Failed to get watchdog clock\n");
		return PTR_ERR(imx7ulp_wdt->clk);
	}

<<<<<<< HEAD
	imx7ulp_wdt->post_rcs_wait = true;
	if (of_device_is_compatible(dev->of_node,
				    "fsl,imx8ulp-wdt")) {
		dev_info(dev, "imx8ulp wdt probe\n");
		imx7ulp_wdt->post_rcs_wait = false;
	} else {
		dev_info(dev, "imx7ulp wdt probe\n");
	}
=======
	ret = clk_prepare_enable(imx7ulp_wdt->clk);
	if (ret)
		return ret;

	ret = devm_add_action_or_reset(dev, imx7ulp_wdt_action, imx7ulp_wdt->clk);
	if (ret)
		return ret;
>>>>>>> b7ba80a49124 (Commit)

	wdog = &imx7ulp_wdt->wdd;
	wdog->info = &imx7ulp_wdt_info;
	wdog->ops = &imx7ulp_wdt_ops;
	wdog->min_timeout = 1;
	wdog->max_timeout = MAX_TIMEOUT;
	wdog->parent = dev;
	wdog->timeout = DEFAULT_TIMEOUT;

	watchdog_init_timeout(wdog, 0, dev);
	watchdog_stop_on_reboot(wdog);
	watchdog_stop_on_unregister(wdog);
	watchdog_set_drvdata(wdog, imx7ulp_wdt);
<<<<<<< HEAD

	imx7ulp_wdt->hw = of_device_get_match_data(dev);
	ret = imx7ulp_wdt_init(imx7ulp_wdt, wdog->timeout * imx7ulp_wdt->hw->wdog_clock_rate);
=======
	ret = imx7ulp_wdt_init(imx7ulp_wdt->base, wdog->timeout * WDOG_CLOCK_RATE);
>>>>>>> b7ba80a49124 (Commit)
	if (ret)
		return ret;

	return devm_watchdog_register_device(dev, wdog);
}

<<<<<<< HEAD
static int __maybe_unused imx7ulp_wdt_suspend_noirq(struct device *dev)
=======
static int __maybe_unused imx7ulp_wdt_suspend(struct device *dev)
>>>>>>> b7ba80a49124 (Commit)
{
	struct imx7ulp_wdt_device *imx7ulp_wdt = dev_get_drvdata(dev);

	if (watchdog_active(&imx7ulp_wdt->wdd))
		imx7ulp_wdt_stop(&imx7ulp_wdt->wdd);

	clk_disable_unprepare(imx7ulp_wdt->clk);

	return 0;
}

<<<<<<< HEAD
static int __maybe_unused imx7ulp_wdt_resume_noirq(struct device *dev)
{
	struct imx7ulp_wdt_device *imx7ulp_wdt = dev_get_drvdata(dev);
	u32 timeout = imx7ulp_wdt->wdd.timeout * imx7ulp_wdt->hw->wdog_clock_rate;
=======
static int __maybe_unused imx7ulp_wdt_resume(struct device *dev)
{
	struct imx7ulp_wdt_device *imx7ulp_wdt = dev_get_drvdata(dev);
	u32 timeout = imx7ulp_wdt->wdd.timeout * WDOG_CLOCK_RATE;
>>>>>>> b7ba80a49124 (Commit)
	int ret;

	ret = clk_prepare_enable(imx7ulp_wdt->clk);
	if (ret)
		return ret;

<<<<<<< HEAD
	if (watchdog_active(&imx7ulp_wdt->wdd)) {
		imx7ulp_wdt_init(imx7ulp_wdt, timeout);
		imx7ulp_wdt_start(&imx7ulp_wdt->wdd);
		imx7ulp_wdt_ping(&imx7ulp_wdt->wdd);
	}
=======
	if (imx7ulp_wdt_is_enabled(imx7ulp_wdt->base))
		imx7ulp_wdt_init(imx7ulp_wdt->base, timeout);

	if (watchdog_active(&imx7ulp_wdt->wdd))
		imx7ulp_wdt_start(&imx7ulp_wdt->wdd);
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

<<<<<<< HEAD
static const struct dev_pm_ops imx7ulp_wdt_pm_ops = {
	SET_NOIRQ_SYSTEM_SLEEP_PM_OPS(imx7ulp_wdt_suspend_noirq,
				      imx7ulp_wdt_resume_noirq)
};

static const struct imx_wdt_hw_feature imx7ulp_wdt_hw = {
	.prescaler_enable = false,
	.wdog_clock_rate = 1000,
};

static const struct imx_wdt_hw_feature imx93_wdt_hw = {
	.prescaler_enable = true,
	.wdog_clock_rate = 125,
};

static const struct of_device_id imx7ulp_wdt_dt_ids[] = {
	{ .compatible = "fsl,imx8ulp-wdt", .data = &imx7ulp_wdt_hw, },
	{ .compatible = "fsl,imx7ulp-wdt", .data = &imx7ulp_wdt_hw, },
	{ .compatible = "fsl,imx93-wdt", .data = &imx93_wdt_hw, },
=======
static SIMPLE_DEV_PM_OPS(imx7ulp_wdt_pm_ops, imx7ulp_wdt_suspend,
			 imx7ulp_wdt_resume);

static const struct of_device_id imx7ulp_wdt_dt_ids[] = {
	{ .compatible = "fsl,imx7ulp-wdt", },
>>>>>>> b7ba80a49124 (Commit)
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, imx7ulp_wdt_dt_ids);

static struct platform_driver imx7ulp_wdt_driver = {
	.probe		= imx7ulp_wdt_probe,
	.driver		= {
		.name	= "imx7ulp-wdt",
		.pm	= &imx7ulp_wdt_pm_ops,
		.of_match_table = imx7ulp_wdt_dt_ids,
	},
};
module_platform_driver(imx7ulp_wdt_driver);

MODULE_AUTHOR("Anson Huang <Anson.Huang@nxp.com>");
MODULE_DESCRIPTION("Freescale i.MX7ULP watchdog driver");
MODULE_LICENSE("GPL v2");
