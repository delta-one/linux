// SPDX-License-Identifier: GPL-2.0-only
/*
 * Mediated device Core Driver
 *
 * Copyright (c) 2016, NVIDIA CORPORATION. All rights reserved.
 *     Author: Neo Jia <cjia@nvidia.com>
 *             Kirti Wankhede <kwankhede@nvidia.com>
 */

#include <linux/module.h>
<<<<<<< HEAD
#include <linux/slab.h>
=======
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uuid.h>
>>>>>>> b7ba80a49124 (Commit)
#include <linux/sysfs.h>
#include <linux/mdev.h>

#include "mdev_private.h"

#define DRIVER_VERSION		"0.1"
#define DRIVER_AUTHOR		"NVIDIA Corporation"
#define DRIVER_DESC		"Mediated device Core Driver"

<<<<<<< HEAD
=======
static LIST_HEAD(parent_list);
static DEFINE_MUTEX(parent_list_lock);
>>>>>>> b7ba80a49124 (Commit)
static struct class_compat *mdev_bus_compat_class;

static LIST_HEAD(mdev_list);
static DEFINE_MUTEX(mdev_list_lock);

<<<<<<< HEAD
=======
struct device *mdev_parent_dev(struct mdev_device *mdev)
{
	return mdev->type->parent->dev;
}
EXPORT_SYMBOL(mdev_parent_dev);

/*
 * Return the index in supported_type_groups that this mdev_device was created
 * from.
 */
unsigned int mdev_get_type_group_id(struct mdev_device *mdev)
{
	return mdev->type->type_group_id;
}
EXPORT_SYMBOL(mdev_get_type_group_id);

/*
 * Used in mdev_type_attribute sysfs functions to return the index in the
 * supported_type_groups that the sysfs is called from.
 */
unsigned int mtype_get_type_group_id(struct mdev_type *mtype)
{
	return mtype->type_group_id;
}
EXPORT_SYMBOL(mtype_get_type_group_id);

/*
 * Used in mdev_type_attribute sysfs functions to return the parent struct
 * device
 */
struct device *mtype_get_parent_dev(struct mdev_type *mtype)
{
	return mtype->parent->dev;
}
EXPORT_SYMBOL(mtype_get_parent_dev);

/* Should be called holding parent_list_lock */
static struct mdev_parent *__find_parent_device(struct device *dev)
{
	struct mdev_parent *parent;

	list_for_each_entry(parent, &parent_list, next) {
		if (parent->dev == dev)
			return parent;
	}
	return NULL;
}

void mdev_release_parent(struct kref *kref)
{
	struct mdev_parent *parent = container_of(kref, struct mdev_parent,
						  ref);
	struct device *dev = parent->dev;

	kfree(parent);
	put_device(dev);
}

>>>>>>> b7ba80a49124 (Commit)
/* Caller must hold parent unreg_sem read or write lock */
static void mdev_device_remove_common(struct mdev_device *mdev)
{
	struct mdev_parent *parent = mdev->type->parent;

	mdev_remove_sysfs_files(mdev);
	device_del(&mdev->dev);
	lockdep_assert_held(&parent->unreg_sem);
	/* Balances with device_initialize() */
	put_device(&mdev->dev);
}

static int mdev_device_remove_cb(struct device *dev, void *data)
{
<<<<<<< HEAD
	if (dev->bus == &mdev_bus_type)
		mdev_device_remove_common(to_mdev_device(dev));
=======
	struct mdev_device *mdev = mdev_from_dev(dev);

	if (mdev)
		mdev_device_remove_common(mdev);
>>>>>>> b7ba80a49124 (Commit)
	return 0;
}

/*
<<<<<<< HEAD
 * mdev_register_parent: Register a device as parent for mdevs
 * @parent: parent structure registered
 * @dev: device structure representing parent device.
 * @mdev_driver: Device driver to bind to the newly created mdev
 * @types: Array of supported mdev types
 * @nr_types: Number of entries in @types
 *
 * Registers the @parent stucture as a parent for mdev types and thus mdev
 * devices.  The caller needs to hold a reference on @dev that must not be
 * released until after the call to mdev_unregister_parent().
 *
 * Returns a negative value on error, otherwise 0.
 */
int mdev_register_parent(struct mdev_parent *parent, struct device *dev,
		struct mdev_driver *mdev_driver, struct mdev_type **types,
		unsigned int nr_types)
{
	char *env_string = "MDEV_STATE=registered";
	char *envp[] = { env_string, NULL };
	int ret;

	memset(parent, 0, sizeof(*parent));
	init_rwsem(&parent->unreg_sem);
	parent->dev = dev;
	parent->mdev_driver = mdev_driver;
	parent->types = types;
	parent->nr_types = nr_types;
	atomic_set(&parent->available_instances, mdev_driver->max_instances);

	if (!mdev_bus_compat_class) {
		mdev_bus_compat_class = class_compat_register("mdev_bus");
		if (!mdev_bus_compat_class)
			return -ENOMEM;
=======
 * mdev_register_device : Register a device
 * @dev: device structure representing parent device.
 * @mdev_driver: Device driver to bind to the newly created mdev
 *
 * Add device to list of registered parent devices.
 * Returns a negative value on error, otherwise 0.
 */
int mdev_register_device(struct device *dev, struct mdev_driver *mdev_driver)
{
	int ret;
	struct mdev_parent *parent;
	char *env_string = "MDEV_STATE=registered";
	char *envp[] = { env_string, NULL };

	/* check for mandatory ops */
	if (!mdev_driver->supported_type_groups)
		return -EINVAL;

	dev = get_device(dev);
	if (!dev)
		return -EINVAL;

	mutex_lock(&parent_list_lock);

	/* Check for duplicate */
	parent = __find_parent_device(dev);
	if (parent) {
		parent = NULL;
		ret = -EEXIST;
		goto add_dev_err;
	}

	parent = kzalloc(sizeof(*parent), GFP_KERNEL);
	if (!parent) {
		ret = -ENOMEM;
		goto add_dev_err;
	}

	kref_init(&parent->ref);
	init_rwsem(&parent->unreg_sem);

	parent->dev = dev;
	parent->mdev_driver = mdev_driver;

	if (!mdev_bus_compat_class) {
		mdev_bus_compat_class = class_compat_register("mdev_bus");
		if (!mdev_bus_compat_class) {
			ret = -ENOMEM;
			goto add_dev_err;
		}
>>>>>>> b7ba80a49124 (Commit)
	}

	ret = parent_create_sysfs_files(parent);
	if (ret)
<<<<<<< HEAD
		return ret;
=======
		goto add_dev_err;
>>>>>>> b7ba80a49124 (Commit)

	ret = class_compat_create_link(mdev_bus_compat_class, dev, NULL);
	if (ret)
		dev_warn(dev, "Failed to create compatibility class link\n");

<<<<<<< HEAD
	dev_info(dev, "MDEV: Registered\n");
	kobject_uevent_env(&dev->kobj, KOBJ_CHANGE, envp);
	return 0;
}
EXPORT_SYMBOL(mdev_register_parent);

/*
 * mdev_unregister_parent : Unregister a parent device
 * @parent: parent structure to unregister
 */
void mdev_unregister_parent(struct mdev_parent *parent)
{
	char *env_string = "MDEV_STATE=unregistered";
	char *envp[] = { env_string, NULL };

	dev_info(parent->dev, "MDEV: Unregistering\n");

	down_write(&parent->unreg_sem);
	class_compat_remove_link(mdev_bus_compat_class, parent->dev, NULL);
	device_for_each_child(parent->dev, NULL, mdev_device_remove_cb);
	parent_remove_sysfs_files(parent);
	up_write(&parent->unreg_sem);

	kobject_uevent_env(&parent->dev->kobj, KOBJ_CHANGE, envp);
}
EXPORT_SYMBOL(mdev_unregister_parent);
=======
	list_add(&parent->next, &parent_list);
	mutex_unlock(&parent_list_lock);

	dev_info(dev, "MDEV: Registered\n");
	kobject_uevent_env(&dev->kobj, KOBJ_CHANGE, envp);

	return 0;

add_dev_err:
	mutex_unlock(&parent_list_lock);
	if (parent)
		mdev_put_parent(parent);
	else
		put_device(dev);
	return ret;
}
EXPORT_SYMBOL(mdev_register_device);

/*
 * mdev_unregister_device : Unregister a parent device
 * @dev: device structure representing parent device.
 *
 * Remove device from list of registered parent devices. Give a chance to free
 * existing mediated devices for given device.
 */

void mdev_unregister_device(struct device *dev)
{
	struct mdev_parent *parent;
	char *env_string = "MDEV_STATE=unregistered";
	char *envp[] = { env_string, NULL };

	mutex_lock(&parent_list_lock);
	parent = __find_parent_device(dev);

	if (!parent) {
		mutex_unlock(&parent_list_lock);
		return;
	}
	dev_info(dev, "MDEV: Unregistering\n");

	list_del(&parent->next);
	mutex_unlock(&parent_list_lock);

	down_write(&parent->unreg_sem);

	class_compat_remove_link(mdev_bus_compat_class, dev, NULL);

	device_for_each_child(dev, NULL, mdev_device_remove_cb);

	parent_remove_sysfs_files(parent);
	up_write(&parent->unreg_sem);

	mdev_put_parent(parent);

	/* We still have the caller's reference to use for the uevent */
	kobject_uevent_env(&dev->kobj, KOBJ_CHANGE, envp);
}
EXPORT_SYMBOL(mdev_unregister_device);
>>>>>>> b7ba80a49124 (Commit)

static void mdev_device_release(struct device *dev)
{
	struct mdev_device *mdev = to_mdev_device(dev);
<<<<<<< HEAD
	struct mdev_parent *parent = mdev->type->parent;

	mutex_lock(&mdev_list_lock);
	list_del(&mdev->next);
	if (!parent->mdev_driver->get_available)
		atomic_inc(&parent->available_instances);
	mutex_unlock(&mdev_list_lock);
=======
>>>>>>> b7ba80a49124 (Commit)

	/* Pairs with the get in mdev_device_create() */
	kobject_put(&mdev->type->kobj);

<<<<<<< HEAD
=======
	mutex_lock(&mdev_list_lock);
	list_del(&mdev->next);
	mutex_unlock(&mdev_list_lock);

>>>>>>> b7ba80a49124 (Commit)
	dev_dbg(&mdev->dev, "MDEV: destroying\n");
	kfree(mdev);
}

int mdev_device_create(struct mdev_type *type, const guid_t *uuid)
{
	int ret;
	struct mdev_device *mdev, *tmp;
	struct mdev_parent *parent = type->parent;
	struct mdev_driver *drv = parent->mdev_driver;

	mutex_lock(&mdev_list_lock);

	/* Check for duplicate */
	list_for_each_entry(tmp, &mdev_list, next) {
		if (guid_equal(&tmp->uuid, uuid)) {
			mutex_unlock(&mdev_list_lock);
			return -EEXIST;
		}
	}

<<<<<<< HEAD
	if (!drv->get_available) {
		/*
		 * Note: that non-atomic read and dec is fine here because
		 * all modifications are under mdev_list_lock.
		 */
		if (!atomic_read(&parent->available_instances)) {
			mutex_unlock(&mdev_list_lock);
			return -EUSERS;
		}
		atomic_dec(&parent->available_instances);
	}

=======
>>>>>>> b7ba80a49124 (Commit)
	mdev = kzalloc(sizeof(*mdev), GFP_KERNEL);
	if (!mdev) {
		mutex_unlock(&mdev_list_lock);
		return -ENOMEM;
	}

	device_initialize(&mdev->dev);
	mdev->dev.parent  = parent->dev;
	mdev->dev.bus = &mdev_bus_type;
	mdev->dev.release = mdev_device_release;
	mdev->dev.groups = mdev_device_groups;
	mdev->type = type;
	/* Pairs with the put in mdev_device_release() */
	kobject_get(&type->kobj);

	guid_copy(&mdev->uuid, uuid);
	list_add(&mdev->next, &mdev_list);
	mutex_unlock(&mdev_list_lock);

	ret = dev_set_name(&mdev->dev, "%pUl", uuid);
	if (ret)
		goto out_put_device;

	/* Check if parent unregistration has started */
	if (!down_read_trylock(&parent->unreg_sem)) {
		ret = -ENODEV;
		goto out_put_device;
	}

	ret = device_add(&mdev->dev);
	if (ret)
		goto out_unlock;

	ret = device_driver_attach(&drv->driver, &mdev->dev);
	if (ret)
		goto out_del;

	ret = mdev_create_sysfs_files(mdev);
	if (ret)
		goto out_del;

	mdev->active = true;
	dev_dbg(&mdev->dev, "MDEV: created\n");
	up_read(&parent->unreg_sem);

	return 0;

out_del:
	device_del(&mdev->dev);
out_unlock:
	up_read(&parent->unreg_sem);
out_put_device:
	put_device(&mdev->dev);
	return ret;
}

int mdev_device_remove(struct mdev_device *mdev)
{
	struct mdev_device *tmp;
	struct mdev_parent *parent = mdev->type->parent;

	mutex_lock(&mdev_list_lock);
	list_for_each_entry(tmp, &mdev_list, next) {
		if (tmp == mdev)
			break;
	}

	if (tmp != mdev) {
		mutex_unlock(&mdev_list_lock);
		return -ENODEV;
	}

	if (!mdev->active) {
		mutex_unlock(&mdev_list_lock);
		return -EAGAIN;
	}

	mdev->active = false;
	mutex_unlock(&mdev_list_lock);

	/* Check if parent unregistration has started */
	if (!down_read_trylock(&parent->unreg_sem))
		return -ENODEV;

	mdev_device_remove_common(mdev);
	up_read(&parent->unreg_sem);
	return 0;
}

static int __init mdev_init(void)
{
	return bus_register(&mdev_bus_type);
}

static void __exit mdev_exit(void)
{
	if (mdev_bus_compat_class)
		class_compat_unregister(mdev_bus_compat_class);
	bus_unregister(&mdev_bus_type);
}

subsys_initcall(mdev_init)
module_exit(mdev_exit)

MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
