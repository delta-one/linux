// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause
/*
 * Copyright (c) 2016 BayLibre, SAS.
 * Author: Neil Armstrong <narmstrong@baylibre.com>
 * Copyright (C) 2014 Amlogic, Inc.
 */
#include <linux/err.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/hw_random.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/of.h>
#include <linux/clk.h>

#define RNG_DATA 0x00

struct meson_rng_data {
	void __iomem *base;
<<<<<<< HEAD
	struct hwrng rng;
=======
	struct platform_device *pdev;
	struct hwrng rng;
	struct clk *core_clk;
>>>>>>> b7ba80a49124 (Commit)
};

static int meson_rng_read(struct hwrng *rng, void *buf, size_t max, bool wait)
{
	struct meson_rng_data *data =
			container_of(rng, struct meson_rng_data, rng);

	*(u32 *)buf = readl_relaxed(data->base + RNG_DATA);

	return sizeof(u32);
}

<<<<<<< HEAD
=======
static void meson_rng_clk_disable(void *data)
{
	clk_disable_unprepare(data);
}

>>>>>>> b7ba80a49124 (Commit)
static int meson_rng_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct meson_rng_data *data;
<<<<<<< HEAD
	struct clk *core_clk;
=======
	int ret;
>>>>>>> b7ba80a49124 (Commit)

	data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

<<<<<<< HEAD
=======
	data->pdev = pdev;

>>>>>>> b7ba80a49124 (Commit)
	data->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(data->base))
		return PTR_ERR(data->base);

<<<<<<< HEAD
	core_clk = devm_clk_get_optional_enabled(dev, "core");
	if (IS_ERR(core_clk))
		return dev_err_probe(dev, PTR_ERR(core_clk),
				     "Failed to get core clock\n");

	data->rng.name = pdev->name;
	data->rng.read = meson_rng_read;

=======
	data->core_clk = devm_clk_get_optional(dev, "core");
	if (IS_ERR(data->core_clk))
		return dev_err_probe(dev, PTR_ERR(data->core_clk),
				     "Failed to get core clock\n");

	if (data->core_clk) {
		ret = clk_prepare_enable(data->core_clk);
		if (ret)
			return ret;
		ret = devm_add_action_or_reset(dev, meson_rng_clk_disable,
					       data->core_clk);
		if (ret)
			return ret;
	}

	data->rng.name = pdev->name;
	data->rng.read = meson_rng_read;

	platform_set_drvdata(pdev, data);

>>>>>>> b7ba80a49124 (Commit)
	return devm_hwrng_register(dev, &data->rng);
}

static const struct of_device_id meson_rng_of_match[] = {
	{ .compatible = "amlogic,meson-rng", },
	{},
};
MODULE_DEVICE_TABLE(of, meson_rng_of_match);

static struct platform_driver meson_rng_driver = {
	.probe	= meson_rng_probe,
	.driver	= {
		.name = "meson-rng",
		.of_match_table = meson_rng_of_match,
	},
};

module_platform_driver(meson_rng_driver);

MODULE_DESCRIPTION("Meson H/W Random Number Generator driver");
MODULE_AUTHOR("Lawrence Mok <lawrence.mok@amlogic.com>");
MODULE_AUTHOR("Neil Armstrong <narmstrong@baylibre.com>");
MODULE_LICENSE("Dual BSD/GPL");
