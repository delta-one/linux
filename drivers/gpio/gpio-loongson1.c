<<<<<<< HEAD
// SPDX-License-Identifier: GPL-2.0-only
/*
 * GPIO Driver for Loongson 1 SoC
 *
 * Copyright (C) 2015-2023 Keguang Zhang <keguang.zhang@gmail.com>
=======
/*
 * GPIO Driver for Loongson 1 SoC
 *
 * Copyright (C) 2015-2016 Zhang, Keguang <keguang.zhang@gmail.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
>>>>>>> b7ba80a49124 (Commit)
 */

#include <linux/module.h>
#include <linux/gpio/driver.h>
#include <linux/platform_device.h>
#include <linux/bitops.h>

/* Loongson 1 GPIO Register Definitions */
#define GPIO_CFG		0x0
#define GPIO_DIR		0x10
#define GPIO_DATA		0x20
#define GPIO_OUTPUT		0x30

<<<<<<< HEAD
struct ls1x_gpio_chip {
	struct gpio_chip gc;
	void __iomem *reg_base;
};

static int ls1x_gpio_request(struct gpio_chip *gc, unsigned int offset)
{
	struct ls1x_gpio_chip *ls1x_gc = gpiochip_get_data(gc);
	unsigned long flags;

	raw_spin_lock_irqsave(&gc->bgpio_lock, flags);
	__raw_writel(__raw_readl(ls1x_gc->reg_base + GPIO_CFG) | BIT(offset),
		     ls1x_gc->reg_base + GPIO_CFG);
=======
static void __iomem *gpio_reg_base;

static int ls1x_gpio_request(struct gpio_chip *gc, unsigned int offset)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&gc->bgpio_lock, flags);
	__raw_writel(__raw_readl(gpio_reg_base + GPIO_CFG) | BIT(offset),
		     gpio_reg_base + GPIO_CFG);
>>>>>>> b7ba80a49124 (Commit)
	raw_spin_unlock_irqrestore(&gc->bgpio_lock, flags);

	return 0;
}

static void ls1x_gpio_free(struct gpio_chip *gc, unsigned int offset)
{
<<<<<<< HEAD
	struct ls1x_gpio_chip *ls1x_gc = gpiochip_get_data(gc);
	unsigned long flags;

	raw_spin_lock_irqsave(&gc->bgpio_lock, flags);
	__raw_writel(__raw_readl(ls1x_gc->reg_base + GPIO_CFG) & ~BIT(offset),
		     ls1x_gc->reg_base + GPIO_CFG);
=======
	unsigned long flags;

	raw_spin_lock_irqsave(&gc->bgpio_lock, flags);
	__raw_writel(__raw_readl(gpio_reg_base + GPIO_CFG) & ~BIT(offset),
		     gpio_reg_base + GPIO_CFG);
>>>>>>> b7ba80a49124 (Commit)
	raw_spin_unlock_irqrestore(&gc->bgpio_lock, flags);
}

static int ls1x_gpio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
<<<<<<< HEAD
	struct ls1x_gpio_chip *ls1x_gc;
	int ret;

	ls1x_gc = devm_kzalloc(dev, sizeof(*ls1x_gc), GFP_KERNEL);
	if (!ls1x_gc)
		return -ENOMEM;

	ls1x_gc->reg_base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(ls1x_gc->reg_base))
		return PTR_ERR(ls1x_gc->reg_base);

	ret = bgpio_init(&ls1x_gc->gc, dev, 4, ls1x_gc->reg_base + GPIO_DATA,
			 ls1x_gc->reg_base + GPIO_OUTPUT, NULL,
			 NULL, ls1x_gc->reg_base + GPIO_DIR, 0);
	if (ret)
		goto err;

	ls1x_gc->gc.owner = THIS_MODULE;
	ls1x_gc->gc.request = ls1x_gpio_request;
	ls1x_gc->gc.free = ls1x_gpio_free;
	/*
	 * Clear ngpio to let gpiolib get the correct number
	 * by reading ngpios property
	 */
	ls1x_gc->gc.ngpio = 0;

	ret = devm_gpiochip_add_data(dev, &ls1x_gc->gc, ls1x_gc);
	if (ret)
		goto err;

	platform_set_drvdata(pdev, ls1x_gc);

	dev_info(dev, "GPIO controller registered with %d pins\n",
		 ls1x_gc->gc.ngpio);

	return 0;
err:
	dev_err(dev, "failed to register GPIO controller\n");
	return ret;
}

static const struct of_device_id ls1x_gpio_dt_ids[] = {
	{ .compatible = "loongson,ls1x-gpio" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ls1x_gpio_dt_ids);

=======
	struct gpio_chip *gc;
	int ret;

	gc = devm_kzalloc(dev, sizeof(*gc), GFP_KERNEL);
	if (!gc)
		return -ENOMEM;

	gpio_reg_base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(gpio_reg_base))
		return PTR_ERR(gpio_reg_base);

	ret = bgpio_init(gc, dev, 4, gpio_reg_base + GPIO_DATA,
			 gpio_reg_base + GPIO_OUTPUT, NULL,
			 NULL, gpio_reg_base + GPIO_DIR, 0);
	if (ret)
		goto err;

	gc->owner = THIS_MODULE;
	gc->request = ls1x_gpio_request;
	gc->free = ls1x_gpio_free;
	gc->base = pdev->id * 32;

	ret = devm_gpiochip_add_data(dev, gc, NULL);
	if (ret)
		goto err;

	platform_set_drvdata(pdev, gc);
	dev_info(dev, "Loongson1 GPIO driver registered\n");

	return 0;
err:
	dev_err(dev, "failed to register GPIO device\n");
	return ret;
}

>>>>>>> b7ba80a49124 (Commit)
static struct platform_driver ls1x_gpio_driver = {
	.probe	= ls1x_gpio_probe,
	.driver	= {
		.name	= "ls1x-gpio",
<<<<<<< HEAD
		.of_match_table = ls1x_gpio_dt_ids,
=======
>>>>>>> b7ba80a49124 (Commit)
	},
};

module_platform_driver(ls1x_gpio_driver);

<<<<<<< HEAD
MODULE_AUTHOR("Keguang Zhang <keguang.zhang@gmail.com>");
=======
MODULE_AUTHOR("Kelvin Cheung <keguang.zhang@gmail.com>");
>>>>>>> b7ba80a49124 (Commit)
MODULE_DESCRIPTION("Loongson1 GPIO driver");
MODULE_LICENSE("GPL");
