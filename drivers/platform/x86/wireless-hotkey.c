// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Airplane mode button for AMD, HP & Xiaomi laptops
 *
 *  Copyright (C) 2014-2017 Alex Hung <alex.hung@canonical.com>
 *  Copyright (C) 2021 Advanced Micro Devices
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/acpi.h>
#include <acpi/acpi_bus.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alex Hung");
MODULE_ALIAS("acpi*:HPQ6001:*");
MODULE_ALIAS("acpi*:WSTADEF:*");
MODULE_ALIAS("acpi*:AMDI0051:*");

<<<<<<< HEAD
struct wl_button {
	struct input_dev *input_dev;
	char phys[32];
};
=======
static struct input_dev *wl_input_dev;
>>>>>>> b7ba80a49124 (Commit)

static const struct acpi_device_id wl_ids[] = {
	{"HPQ6001", 0},
	{"WSTADEF", 0},
	{"AMDI0051", 0},
	{"", 0},
};

<<<<<<< HEAD
static int wireless_input_setup(struct acpi_device *device)
{
	struct wl_button *button = acpi_driver_data(device);
	int err;

	button->input_dev = input_allocate_device();
	if (!button->input_dev)
		return -ENOMEM;

	snprintf(button->phys, sizeof(button->phys), "%s/input0", acpi_device_hid(device));

	button->input_dev->name = "Wireless hotkeys";
	button->input_dev->phys = button->phys;
	button->input_dev->id.bustype = BUS_HOST;
	button->input_dev->evbit[0] = BIT(EV_KEY);
	set_bit(KEY_RFKILL, button->input_dev->keybit);

	err = input_register_device(button->input_dev);
=======
static int wireless_input_setup(void)
{
	int err;

	wl_input_dev = input_allocate_device();
	if (!wl_input_dev)
		return -ENOMEM;

	wl_input_dev->name = "Wireless hotkeys";
	wl_input_dev->phys = "hpq6001/input0";
	wl_input_dev->id.bustype = BUS_HOST;
	wl_input_dev->evbit[0] = BIT(EV_KEY);
	set_bit(KEY_RFKILL, wl_input_dev->keybit);

	err = input_register_device(wl_input_dev);
>>>>>>> b7ba80a49124 (Commit)
	if (err)
		goto err_free_dev;

	return 0;

err_free_dev:
<<<<<<< HEAD
	input_free_device(button->input_dev);
	return err;
}

static void wireless_input_destroy(struct acpi_device *device)
{
	struct wl_button *button = acpi_driver_data(device);

	input_unregister_device(button->input_dev);
	kfree(button);
=======
	input_free_device(wl_input_dev);
	return err;
}

static void wireless_input_destroy(void)
{
	input_unregister_device(wl_input_dev);
>>>>>>> b7ba80a49124 (Commit)
}

static void wl_notify(struct acpi_device *acpi_dev, u32 event)
{
<<<<<<< HEAD
	struct wl_button *button = acpi_driver_data(acpi_dev);

=======
>>>>>>> b7ba80a49124 (Commit)
	if (event != 0x80) {
		pr_info("Received unknown event (0x%x)\n", event);
		return;
	}

<<<<<<< HEAD
	input_report_key(button->input_dev, KEY_RFKILL, 1);
	input_sync(button->input_dev);
	input_report_key(button->input_dev, KEY_RFKILL, 0);
	input_sync(button->input_dev);
=======
	input_report_key(wl_input_dev, KEY_RFKILL, 1);
	input_sync(wl_input_dev);
	input_report_key(wl_input_dev, KEY_RFKILL, 0);
	input_sync(wl_input_dev);
>>>>>>> b7ba80a49124 (Commit)
}

static int wl_add(struct acpi_device *device)
{
<<<<<<< HEAD
	struct wl_button *button;
	int err;

	button = kzalloc(sizeof(struct wl_button), GFP_KERNEL);
	if (!button)
		return -ENOMEM;

	device->driver_data = button;

	err = wireless_input_setup(device);
	if (err) {
		pr_err("Failed to setup wireless hotkeys\n");
		kfree(button);
	}
=======
	int err;

	err = wireless_input_setup();
	if (err)
		pr_err("Failed to setup wireless hotkeys\n");
>>>>>>> b7ba80a49124 (Commit)

	return err;
}

<<<<<<< HEAD
static void wl_remove(struct acpi_device *device)
{
	wireless_input_destroy(device);
=======
static int wl_remove(struct acpi_device *device)
{
	wireless_input_destroy();
	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

static struct acpi_driver wl_driver = {
	.name	= "wireless-hotkey",
	.owner	= THIS_MODULE,
	.ids	= wl_ids,
	.ops	= {
		.add	= wl_add,
		.remove	= wl_remove,
		.notify	= wl_notify,
	},
};

module_acpi_driver(wl_driver);
