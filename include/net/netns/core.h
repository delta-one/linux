/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __NETNS_CORE_H__
#define __NETNS_CORE_H__

#include <linux/types.h>

struct ctl_table_header;
struct prot_inuse;
<<<<<<< HEAD
struct cpumask;
=======
>>>>>>> b7ba80a49124 (Commit)

struct netns_core {
	/* core sysctls */
	struct ctl_table_header	*sysctl_hdr;

	int	sysctl_somaxconn;
	u8	sysctl_txrehash;

#ifdef CONFIG_PROC_FS
	struct prot_inuse __percpu *prot_inuse;
#endif
<<<<<<< HEAD

#if IS_ENABLED(CONFIG_RPS) && IS_ENABLED(CONFIG_SYSCTL)
	struct cpumask *rps_default_mask;
#endif
=======
>>>>>>> b7ba80a49124 (Commit)
};

#endif
