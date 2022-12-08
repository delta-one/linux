/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _NF_NAT_REDIRECT_H_
#define _NF_NAT_REDIRECT_H_

#include <linux/skbuff.h>
#include <uapi/linux/netfilter/nf_nat.h>

unsigned int
<<<<<<< HEAD
nf_nat_redirect_ipv4(struct sk_buff *skb, const struct nf_nat_range2 *range,
=======
nf_nat_redirect_ipv4(struct sk_buff *skb,
		     const struct nf_nat_ipv4_multi_range_compat *mr,
>>>>>>> b7ba80a49124 (Commit)
		     unsigned int hooknum);
unsigned int
nf_nat_redirect_ipv6(struct sk_buff *skb, const struct nf_nat_range2 *range,
		     unsigned int hooknum);

#endif /* _NF_NAT_REDIRECT_H_ */
