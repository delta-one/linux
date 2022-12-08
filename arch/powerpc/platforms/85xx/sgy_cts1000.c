// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Servergy CTS-1000 Setup
 *
 * Maintained by Ben Collins <ben.c@servergy.com>
 *
 * Copyright 2012 by Servergy, Inc.
 */

<<<<<<< HEAD
#define pr_fmt(fmt) "gpio-halt: " fmt

#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
=======
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/of_gpio.h>
>>>>>>> b7ba80a49124 (Commit)
#include <linux/of_irq.h>
#include <linux/workqueue.h>
#include <linux/reboot.h>
#include <linux/interrupt.h>

#include <asm/machdep.h>

<<<<<<< HEAD
static struct gpio_desc *halt_gpio;
static int halt_irq;
=======
static struct device_node *halt_node;
>>>>>>> b7ba80a49124 (Commit)

static const struct of_device_id child_match[] = {
	{
		.compatible = "sgy,gpio-halt",
	},
	{},
};

static void gpio_halt_wfn(struct work_struct *work)
{
	/* Likely wont return */
	orderly_poweroff(true);
}
static DECLARE_WORK(gpio_halt_wq, gpio_halt_wfn);

static void __noreturn gpio_halt_cb(void)
{
<<<<<<< HEAD
	pr_info("triggering GPIO.\n");

	/* Probably wont return */
	gpiod_set_value(halt_gpio, 1);
=======
	enum of_gpio_flags flags;
	int trigger, gpio;

	if (!halt_node)
		panic("No reset GPIO information was provided in DT\n");

	gpio = of_get_gpio_flags(halt_node, 0, &flags);

	if (!gpio_is_valid(gpio))
		panic("Provided GPIO is invalid\n");

	trigger = (flags == OF_GPIO_ACTIVE_LOW);

	printk(KERN_INFO "gpio-halt: triggering GPIO.\n");

	/* Probably wont return */
	gpio_set_value(gpio, trigger);
>>>>>>> b7ba80a49124 (Commit)

	panic("Halt failed\n");
}

/* This IRQ means someone pressed the power button and it is waiting for us
 * to handle the shutdown/poweroff. */
static irqreturn_t gpio_halt_irq(int irq, void *__data)
{
<<<<<<< HEAD
	struct platform_device *pdev = __data;

	dev_info(&pdev->dev, "scheduling shutdown due to power button IRQ\n");
=======
	printk(KERN_INFO "gpio-halt: shutdown due to power button IRQ.\n");
>>>>>>> b7ba80a49124 (Commit)
	schedule_work(&gpio_halt_wq);

        return IRQ_HANDLED;
};

<<<<<<< HEAD
static int __gpio_halt_probe(struct platform_device *pdev,
			     struct device_node *halt_node)
{
	int err;

	halt_gpio = fwnode_gpiod_get_index(of_fwnode_handle(halt_node),
					   NULL, 0, GPIOD_OUT_LOW, "gpio-halt");
	err = PTR_ERR_OR_ZERO(halt_gpio);
	if (err) {
		dev_err(&pdev->dev, "failed to request halt GPIO: %d\n", err);
		return err;
	}

	/* Now get the IRQ which tells us when the power button is hit */
	halt_irq = irq_of_parse_and_map(halt_node, 0);
	err = request_irq(halt_irq, gpio_halt_irq,
			  IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
			  "gpio-halt", pdev);
	if (err) {
		dev_err(&pdev->dev, "failed to request IRQ %d: %d\n",
			halt_irq, err);
		gpiod_put(halt_gpio);
		halt_gpio = NULL;
		return err;
=======
static int gpio_halt_probe(struct platform_device *pdev)
{
	enum of_gpio_flags flags;
	struct device_node *node = pdev->dev.of_node;
	struct device_node *child_node;
	int gpio, err, irq;
	int trigger;

	if (!node)
		return -ENODEV;

	/* If there's no matching child, this isn't really an error */
	child_node = of_find_matching_node(node, child_match);
	if (!child_node)
		return 0;

	/* Technically we could just read the first one, but punish
	 * DT writers for invalid form. */
	if (of_gpio_count(child_node) != 1) {
		err = -EINVAL;
		goto err_put;
	}

	/* Get the gpio number relative to the dynamic base. */
	gpio = of_get_gpio_flags(child_node, 0, &flags);
	if (!gpio_is_valid(gpio)) {
		err = -EINVAL;
		goto err_put;
	}

	err = gpio_request(gpio, "gpio-halt");
	if (err) {
		printk(KERN_ERR "gpio-halt: error requesting GPIO %d.\n",
		       gpio);
		goto err_put;
	}

	trigger = (flags == OF_GPIO_ACTIVE_LOW);

	gpio_direction_output(gpio, !trigger);

	/* Now get the IRQ which tells us when the power button is hit */
	irq = irq_of_parse_and_map(child_node, 0);
	err = request_irq(irq, gpio_halt_irq, IRQF_TRIGGER_RISING |
			  IRQF_TRIGGER_FALLING, "gpio-halt", child_node);
	if (err) {
		printk(KERN_ERR "gpio-halt: error requesting IRQ %d for "
		       "GPIO %d.\n", irq, gpio);
		gpio_free(gpio);
		goto err_put;
>>>>>>> b7ba80a49124 (Commit)
	}

	/* Register our halt function */
	ppc_md.halt = gpio_halt_cb;
	pm_power_off = gpio_halt_cb;

<<<<<<< HEAD
	dev_info(&pdev->dev, "registered halt GPIO, irq: %d\n", halt_irq);

	return 0;
}

static int gpio_halt_probe(struct platform_device *pdev)
{
	struct device_node *halt_node;
	int ret;

	if (!pdev->dev.of_node)
		return -ENODEV;

	/* If there's no matching child, this isn't really an error */
	halt_node = of_find_matching_node(pdev->dev.of_node, child_match);
	if (!halt_node)
		return -ENODEV;

	ret = __gpio_halt_probe(pdev, halt_node);
	of_node_put(halt_node);

	return ret;
=======
	printk(KERN_INFO "gpio-halt: registered GPIO %d (%d trigger, %d"
	       " irq).\n", gpio, trigger, irq);

	halt_node = child_node;
	return 0;

err_put:
	of_node_put(child_node);
	return err;
>>>>>>> b7ba80a49124 (Commit)
}

static int gpio_halt_remove(struct platform_device *pdev)
{
<<<<<<< HEAD
	free_irq(halt_irq, pdev);
	cancel_work_sync(&gpio_halt_wq);

	ppc_md.halt = NULL;
	pm_power_off = NULL;

	gpiod_put(halt_gpio);
	halt_gpio = NULL;
=======
	if (halt_node) {
		int gpio = of_get_gpio(halt_node, 0);
		int irq = irq_of_parse_and_map(halt_node, 0);

		free_irq(irq, halt_node);

		ppc_md.halt = NULL;
		pm_power_off = NULL;

		gpio_free(gpio);

		of_node_put(halt_node);
		halt_node = NULL;
	}
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

static const struct of_device_id gpio_halt_match[] = {
	/* We match on the gpio bus itself and scan the children since they
	 * wont be matched against us. We know the bus wont match until it
	 * has been registered too. */
	{
		.compatible = "fsl,qoriq-gpio",
	},
	{},
};
MODULE_DEVICE_TABLE(of, gpio_halt_match);

static struct platform_driver gpio_halt_driver = {
	.driver = {
		.name		= "gpio-halt",
		.of_match_table = gpio_halt_match,
	},
	.probe		= gpio_halt_probe,
	.remove		= gpio_halt_remove,
};

module_platform_driver(gpio_halt_driver);

MODULE_DESCRIPTION("Driver to support GPIO triggered system halt for Servergy CTS-1000 Systems.");
MODULE_VERSION("1.0");
MODULE_AUTHOR("Ben Collins <ben.c@servergy.com>");
MODULE_LICENSE("GPL");
