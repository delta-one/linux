/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2021 Intel Corporation
 */

#ifndef __INTEL_PXP_DEBUGFS_H__
#define __INTEL_PXP_DEBUGFS_H__

struct intel_pxp;
struct dentry;

#ifdef CONFIG_DRM_I915_PXP
<<<<<<< HEAD
void intel_pxp_debugfs_register(struct intel_pxp *pxp);
#else
static inline void
intel_pxp_debugfs_register(struct intel_pxp *pxp)
=======
void intel_pxp_debugfs_register(struct intel_pxp *pxp, struct dentry *root);
#else
static inline void
intel_pxp_debugfs_register(struct intel_pxp *pxp, struct dentry *root)
>>>>>>> b7ba80a49124 (Commit)
{
}
#endif

#endif /* __INTEL_PXP_DEBUGFS_H__ */
