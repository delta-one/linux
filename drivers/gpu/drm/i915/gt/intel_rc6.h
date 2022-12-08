/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2019 Intel Corporation
 */

#ifndef INTEL_RC6_H
#define INTEL_RC6_H

<<<<<<< HEAD
#include <linux/types.h>

enum intel_rc6_res_type;
struct intel_rc6;
struct seq_file;
=======
#include "i915_reg_defs.h"

struct intel_engine_cs;
struct intel_rc6;
>>>>>>> b7ba80a49124 (Commit)

void intel_rc6_init(struct intel_rc6 *rc6);
void intel_rc6_fini(struct intel_rc6 *rc6);

void intel_rc6_unpark(struct intel_rc6 *rc6);
void intel_rc6_park(struct intel_rc6 *rc6);

void intel_rc6_sanitize(struct intel_rc6 *rc6);
void intel_rc6_enable(struct intel_rc6 *rc6);
void intel_rc6_disable(struct intel_rc6 *rc6);

<<<<<<< HEAD
u64 intel_rc6_residency_ns(struct intel_rc6 *rc6, enum intel_rc6_res_type id);
u64 intel_rc6_residency_us(struct intel_rc6 *rc6, enum intel_rc6_res_type id);
void intel_rc6_print_residency(struct seq_file *m, const char *title,
			       enum intel_rc6_res_type id);
=======
u64 intel_rc6_residency_ns(struct intel_rc6 *rc6, i915_reg_t reg);
u64 intel_rc6_residency_us(struct intel_rc6 *rc6, i915_reg_t reg);
>>>>>>> b7ba80a49124 (Commit)

#endif /* INTEL_RC6_H */
