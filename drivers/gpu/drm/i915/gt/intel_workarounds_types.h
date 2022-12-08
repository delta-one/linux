/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2014-2018 Intel Corporation
 */

#ifndef __INTEL_WORKAROUNDS_TYPES_H__
#define __INTEL_WORKAROUNDS_TYPES_H__

#include <linux/types.h>

#include "i915_reg_defs.h"

<<<<<<< HEAD
struct intel_gt;

struct i915_wa {
	union {
		i915_reg_t	reg;
		i915_mcr_reg_t	mcr_reg;
	};
	u32		clr;
	u32		set;
	u32		read;

	u32		masked_reg:1;
	u32		is_mcr:1;
};

struct i915_wa_list {
	struct intel_gt	*gt;
=======
struct i915_wa {
	i915_reg_t	reg;
	u32		clr;
	u32		set;
	u32		read;
	bool		masked_reg;
};

struct i915_wa_list {
>>>>>>> b7ba80a49124 (Commit)
	const char	*name;
	const char	*engine_name;
	struct i915_wa	*list;
	unsigned int	count;
	unsigned int	wa_count;
};

#endif /* __INTEL_WORKAROUNDS_TYPES_H__ */
