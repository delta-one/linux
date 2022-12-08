/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __SMT_H
#define __SMT_H 1

<<<<<<< HEAD
/*
 * Returns true if SMT (aka hyperthreading) is enabled. Determined via sysfs or
 * the online topology.
 */
bool smt_on(void);
=======
struct cpu_topology;

/* Returns true if SMT (aka hyperthreading) is enabled. */
bool smt_on(const struct cpu_topology *topology);
>>>>>>> b7ba80a49124 (Commit)

/*
 * Returns true when system wide and all SMT threads for a core are in the
 * user_requested_cpus map.
 */
<<<<<<< HEAD
bool core_wide(bool system_wide, const char *user_requested_cpu_list);
=======
bool core_wide(bool system_wide, const char *user_requested_cpu_list,
	       const struct cpu_topology *topology);
>>>>>>> b7ba80a49124 (Commit)

#endif /* __SMT_H */
