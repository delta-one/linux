// SPDX-License-Identifier: MIT
/*
 * Copyright © 2021 Intel Corporation
 */

#include <linux/debugfs.h>
#include <linux/string_helpers.h>

#include <drm/drm_print.h>

#include "gt/intel_gt_debugfs.h"
<<<<<<< HEAD

#include "i915_drv.h"

#include "intel_pxp.h"
#include "intel_pxp_debugfs.h"
#include "intel_pxp_irq.h"
#include "intel_pxp_types.h"
=======
#include "i915_drv.h"
#include "intel_pxp.h"
#include "intel_pxp_debugfs.h"
#include "intel_pxp_irq.h"
>>>>>>> b7ba80a49124 (Commit)

static int pxp_info_show(struct seq_file *m, void *data)
{
	struct intel_pxp *pxp = m->private;
	struct drm_printer p = drm_seq_file_printer(m);
<<<<<<< HEAD

	if (!intel_pxp_is_enabled(pxp)) {
=======
	bool enabled = intel_pxp_is_enabled(pxp);

	if (!enabled) {
>>>>>>> b7ba80a49124 (Commit)
		drm_printf(&p, "pxp disabled\n");
		return 0;
	}

	drm_printf(&p, "active: %s\n", str_yes_no(intel_pxp_is_active(pxp)));
	drm_printf(&p, "instance counter: %u\n", pxp->key_instance);

	return 0;
}
<<<<<<< HEAD

DEFINE_SHOW_ATTRIBUTE(pxp_info);
=======
DEFINE_INTEL_GT_DEBUGFS_ATTRIBUTE(pxp_info);
>>>>>>> b7ba80a49124 (Commit)

static int pxp_terminate_get(void *data, u64 *val)
{
	/* nothing to read */
	return -EPERM;
}

static int pxp_terminate_set(void *data, u64 val)
{
	struct intel_pxp *pxp = data;
<<<<<<< HEAD
	struct intel_gt *gt = pxp->ctrl_gt;
=======
	struct intel_gt *gt = pxp_to_gt(pxp);
>>>>>>> b7ba80a49124 (Commit)

	if (!intel_pxp_is_active(pxp))
		return -ENODEV;

	/* simulate a termination interrupt */
	spin_lock_irq(gt->irq_lock);
	intel_pxp_irq_handler(pxp, GEN12_DISPLAY_PXP_STATE_TERMINATED_INTERRUPT);
	spin_unlock_irq(gt->irq_lock);

	if (!wait_for_completion_timeout(&pxp->termination,
					 msecs_to_jiffies(100)))
		return -ETIMEDOUT;

	return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(pxp_terminate_fops, pxp_terminate_get, pxp_terminate_set, "%llx\n");
<<<<<<< HEAD

void intel_pxp_debugfs_register(struct intel_pxp *pxp)
{
	struct drm_minor *minor;
	struct dentry *pxproot;

	if (!intel_pxp_is_supported(pxp))
		return;

	minor = pxp->ctrl_gt->i915->drm.primary;
	if (!minor->debugfs_root)
		return;

	pxproot = debugfs_create_dir("pxp", minor->debugfs_root);
	if (IS_ERR(pxproot))
		return;

	debugfs_create_file("info", 0444, pxproot,
			    pxp, &pxp_info_fops);

	debugfs_create_file("terminate_state", 0644, pxproot,
			    pxp, &pxp_terminate_fops);
=======
void intel_pxp_debugfs_register(struct intel_pxp *pxp, struct dentry *gt_root)
{
	static const struct intel_gt_debugfs_file files[] = {
		{ "info", &pxp_info_fops, NULL },
		{ "terminate_state", &pxp_terminate_fops, NULL },
	};
	struct dentry *root;

	if (!gt_root)
		return;

	if (!HAS_PXP((pxp_to_gt(pxp)->i915)))
		return;

	root = debugfs_create_dir("pxp", gt_root);
	if (IS_ERR(root))
		return;

	intel_gt_debugfs_register_files(root, files, ARRAY_SIZE(files), pxp);
>>>>>>> b7ba80a49124 (Commit)
}
