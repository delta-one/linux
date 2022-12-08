// SPDX-License-Identifier: MIT
/*
 * Copyright © 2020 Intel Corporation
 */

<<<<<<< HEAD
#include <linux/kernel.h>

#include "i915_config.h"
#include "i915_utils.h"
=======
#include "i915_drv.h"
>>>>>>> b7ba80a49124 (Commit)

unsigned long
i915_fence_context_timeout(const struct drm_i915_private *i915, u64 context)
{
	if (CONFIG_DRM_I915_FENCE_TIMEOUT && context)
		return msecs_to_jiffies_timeout(CONFIG_DRM_I915_FENCE_TIMEOUT);

	return 0;
}
