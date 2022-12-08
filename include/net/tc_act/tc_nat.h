/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __NET_TC_NAT_H
#define __NET_TC_NAT_H

#include <linux/types.h>
#include <net/act_api.h>

<<<<<<< HEAD
struct tcf_nat_parms {
=======
struct tcf_nat {
	struct tc_action common;

>>>>>>> b7ba80a49124 (Commit)
	__be32 old_addr;
	__be32 new_addr;
	__be32 mask;
	u32 flags;
<<<<<<< HEAD
	struct rcu_head rcu;
};

struct tcf_nat {
	struct tc_action common;
	struct tcf_nat_parms __rcu *parms;
=======
>>>>>>> b7ba80a49124 (Commit)
};

#define to_tcf_nat(a) ((struct tcf_nat *)a)

#endif /* __NET_TC_NAT_H */
