/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2020, Intel Corporation. All rights reserved.
 */

#ifndef __INTEL_PXP_H__
#define __INTEL_PXP_H__

#include <linux/errno.h>
#include <linux/types.h>

<<<<<<< HEAD
struct drm_i915_gem_object;
struct drm_i915_private;
struct intel_pxp;

bool intel_pxp_is_supported(const struct intel_pxp *pxp);
bool intel_pxp_is_enabled(const struct intel_pxp *pxp);
bool intel_pxp_is_active(const struct intel_pxp *pxp);

int intel_pxp_init(struct drm_i915_private *i915);
void intel_pxp_fini(struct drm_i915_private *i915);
=======
struct intel_pxp;
struct drm_i915_gem_object;

#ifdef CONFIG_DRM_I915_PXP
struct intel_gt *pxp_to_gt(const struct intel_pxp *pxp);
bool intel_pxp_is_enabled(const struct intel_pxp *pxp);
bool intel_pxp_is_active(const struct intel_pxp *pxp);

void intel_pxp_init(struct intel_pxp *pxp);
void intel_pxp_fini(struct intel_pxp *pxp);
>>>>>>> b7ba80a49124 (Commit)

void intel_pxp_init_hw(struct intel_pxp *pxp);
void intel_pxp_fini_hw(struct intel_pxp *pxp);

void intel_pxp_mark_termination_in_progress(struct intel_pxp *pxp);
<<<<<<< HEAD
void intel_pxp_tee_end_arb_fw_session(struct intel_pxp *pxp, u32 arb_session_id);

int intel_pxp_start(struct intel_pxp *pxp);
void intel_pxp_end(struct intel_pxp *pxp);
=======

int intel_pxp_start(struct intel_pxp *pxp);
>>>>>>> b7ba80a49124 (Commit)

int intel_pxp_key_check(struct intel_pxp *pxp,
			struct drm_i915_gem_object *obj,
			bool assign);

void intel_pxp_invalidate(struct intel_pxp *pxp);
<<<<<<< HEAD
=======
#else
static inline void intel_pxp_init(struct intel_pxp *pxp)
{
}

static inline void intel_pxp_fini(struct intel_pxp *pxp)
{
}

static inline int intel_pxp_start(struct intel_pxp *pxp)
{
	return -ENODEV;
}

static inline bool intel_pxp_is_enabled(const struct intel_pxp *pxp)
{
	return false;
}

static inline bool intel_pxp_is_active(const struct intel_pxp *pxp)
{
	return false;
}

static inline int intel_pxp_key_check(struct intel_pxp *pxp,
				      struct drm_i915_gem_object *obj,
				      bool assign)
{
	return -ENODEV;
}
#endif
>>>>>>> b7ba80a49124 (Commit)

#endif /* __INTEL_PXP_H__ */
