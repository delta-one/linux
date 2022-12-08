// SPDX-License-Identifier: GPL-2.0-only
/* Copyright(c) 2022 Intel Corporation. */

#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/semaphore.h>
<<<<<<< HEAD
#include <linux/slab.h>
=======
>>>>>>> b7ba80a49124 (Commit)

#include <asm/cpu_device_id.h>

#include "ifs.h"

#define X86_MATCH(model)				\
	X86_MATCH_VENDOR_FAM_MODEL_FEATURE(INTEL, 6,	\
		INTEL_FAM6_##model, X86_FEATURE_CORE_CAPABILITIES, NULL)

static const struct x86_cpu_id ifs_cpu_ids[] __initconst = {
	X86_MATCH(SAPPHIRERAPIDS_X),
	{}
};
MODULE_DEVICE_TABLE(x86cpu, ifs_cpu_ids);

static struct ifs_device ifs_device = {
	.data = {
		.integrity_cap_bit = MSR_INTEGRITY_CAPS_PERIODIC_BIST_BIT,
<<<<<<< HEAD
		.test_num = 0,
=======
>>>>>>> b7ba80a49124 (Commit)
	},
	.misc = {
		.name = "intel_ifs_0",
		.nodename = "intel_ifs/0",
		.minor = MISC_DYNAMIC_MINOR,
	},
};

static int __init ifs_init(void)
{
	const struct x86_cpu_id *m;
	u64 msrval;
<<<<<<< HEAD
	int ret;
=======
>>>>>>> b7ba80a49124 (Commit)

	m = x86_match_cpu(ifs_cpu_ids);
	if (!m)
		return -ENODEV;

	if (rdmsrl_safe(MSR_IA32_CORE_CAPS, &msrval))
		return -ENODEV;

	if (!(msrval & MSR_IA32_CORE_CAPS_INTEGRITY_CAPS))
		return -ENODEV;

	if (rdmsrl_safe(MSR_INTEGRITY_CAPS, &msrval))
		return -ENODEV;

	ifs_device.misc.groups = ifs_get_groups();

<<<<<<< HEAD
	if (!(msrval & BIT(ifs_device.data.integrity_cap_bit)))
		return -ENODEV;

	ifs_device.data.pkg_auth = kmalloc_array(topology_max_packages(), sizeof(bool), GFP_KERNEL);
	if (!ifs_device.data.pkg_auth)
		return -ENOMEM;

	ret = misc_register(&ifs_device.misc);
	if (ret) {
		kfree(ifs_device.data.pkg_auth);
		return ret;
	}

	return 0;
=======
	if ((msrval & BIT(ifs_device.data.integrity_cap_bit)) &&
	    !misc_register(&ifs_device.misc)) {
		down(&ifs_sem);
		ifs_load_firmware(ifs_device.misc.this_device);
		up(&ifs_sem);
		return 0;
	}

	return -ENODEV;
>>>>>>> b7ba80a49124 (Commit)
}

static void __exit ifs_exit(void)
{
	misc_deregister(&ifs_device.misc);
<<<<<<< HEAD
	kfree(ifs_device.data.pkg_auth);
=======
>>>>>>> b7ba80a49124 (Commit)
}

module_init(ifs_init);
module_exit(ifs_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Intel In Field Scan (IFS) device");
