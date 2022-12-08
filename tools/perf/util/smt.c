// SPDX-License-Identifier: GPL-2.0-only
#include <string.h>
#include "api/fs/fs.h"
#include "cputopo.h"
#include "smt.h"

<<<<<<< HEAD
bool smt_on(void)
=======
bool smt_on(const struct cpu_topology *topology)
>>>>>>> b7ba80a49124 (Commit)
{
	static bool cached;
	static bool cached_result;
	int fs_value;

	if (cached)
		return cached_result;

	if (sysfs__read_int("devices/system/cpu/smt/active", &fs_value) >= 0)
		cached_result = (fs_value == 1);
	else
<<<<<<< HEAD
		cached_result = cpu_topology__smt_on(online_topology());
=======
		cached_result = cpu_topology__smt_on(topology);
>>>>>>> b7ba80a49124 (Commit)

	cached = true;
	return cached_result;
}

<<<<<<< HEAD
bool core_wide(bool system_wide, const char *user_requested_cpu_list)
=======
bool core_wide(bool system_wide, const char *user_requested_cpu_list,
	       const struct cpu_topology *topology)
>>>>>>> b7ba80a49124 (Commit)
{
	/* If not everything running on a core is being recorded then we can't use core_wide. */
	if (!system_wide)
		return false;

	/* Cheap case that SMT is disabled and therefore we're inherently core_wide. */
<<<<<<< HEAD
	if (!smt_on())
		return true;

	return cpu_topology__core_wide(online_topology(), user_requested_cpu_list);
=======
	if (!smt_on(topology))
		return true;

	return cpu_topology__core_wide(topology, user_requested_cpu_list);
>>>>>>> b7ba80a49124 (Commit)
}
