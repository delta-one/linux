/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __NET_TC_PED_H
#define __NET_TC_PED_H

#include <net/act_api.h>
#include <linux/tc_act/tc_pedit.h>
<<<<<<< HEAD
#include <linux/types.h>
=======
>>>>>>> b7ba80a49124 (Commit)

struct tcf_pedit_key_ex {
	enum pedit_header_type htype;
	enum pedit_cmd cmd;
};

<<<<<<< HEAD
struct tcf_pedit_parms {
	struct tc_pedit_key	*tcfp_keys;
	struct tcf_pedit_key_ex	*tcfp_keys_ex;
	u32 tcfp_off_max_hint;
	unsigned char tcfp_nkeys;
	unsigned char tcfp_flags;
	struct rcu_head rcu;
};

struct tcf_pedit {
	struct tc_action common;
	struct tcf_pedit_parms __rcu *parms;
};

#define to_pedit(a) ((struct tcf_pedit *)a)
#define to_pedit_parms(a) (rcu_dereference(to_pedit(a)->parms))
=======
struct tcf_pedit {
	struct tc_action	common;
	unsigned char		tcfp_nkeys;
	unsigned char		tcfp_flags;
	u32			tcfp_off_max_hint;
	struct tc_pedit_key	*tcfp_keys;
	struct tcf_pedit_key_ex	*tcfp_keys_ex;
};

#define to_pedit(a) ((struct tcf_pedit *)a)
>>>>>>> b7ba80a49124 (Commit)

static inline bool is_tcf_pedit(const struct tc_action *a)
{
#ifdef CONFIG_NET_CLS_ACT
	if (a->ops && a->ops->id == TCA_ID_PEDIT)
		return true;
#endif
	return false;
}

static inline int tcf_pedit_nkeys(const struct tc_action *a)
{
<<<<<<< HEAD
	struct tcf_pedit_parms *parms;
	int nkeys;

	rcu_read_lock();
	parms = to_pedit_parms(a);
	nkeys = parms->tcfp_nkeys;
	rcu_read_unlock();

	return nkeys;
=======
	return to_pedit(a)->tcfp_nkeys;
>>>>>>> b7ba80a49124 (Commit)
}

static inline u32 tcf_pedit_htype(const struct tc_action *a, int index)
{
<<<<<<< HEAD
	u32 htype = TCA_PEDIT_KEY_EX_HDR_TYPE_NETWORK;
	struct tcf_pedit_parms *parms;

	rcu_read_lock();
	parms = to_pedit_parms(a);
	if (parms->tcfp_keys_ex)
		htype = parms->tcfp_keys_ex[index].htype;
	rcu_read_unlock();

	return htype;
=======
	if (to_pedit(a)->tcfp_keys_ex)
		return to_pedit(a)->tcfp_keys_ex[index].htype;

	return TCA_PEDIT_KEY_EX_HDR_TYPE_NETWORK;
>>>>>>> b7ba80a49124 (Commit)
}

static inline u32 tcf_pedit_cmd(const struct tc_action *a, int index)
{
<<<<<<< HEAD
	struct tcf_pedit_parms *parms;
	u32 cmd = __PEDIT_CMD_MAX;

	rcu_read_lock();
	parms = to_pedit_parms(a);
	if (parms->tcfp_keys_ex)
		cmd = parms->tcfp_keys_ex[index].cmd;
	rcu_read_unlock();

	return cmd;
=======
	if (to_pedit(a)->tcfp_keys_ex)
		return to_pedit(a)->tcfp_keys_ex[index].cmd;

	return __PEDIT_CMD_MAX;
>>>>>>> b7ba80a49124 (Commit)
}

static inline u32 tcf_pedit_mask(const struct tc_action *a, int index)
{
<<<<<<< HEAD
	struct tcf_pedit_parms *parms;
	u32 mask;

	rcu_read_lock();
	parms = to_pedit_parms(a);
	mask = parms->tcfp_keys[index].mask;
	rcu_read_unlock();

	return mask;
=======
	return to_pedit(a)->tcfp_keys[index].mask;
>>>>>>> b7ba80a49124 (Commit)
}

static inline u32 tcf_pedit_val(const struct tc_action *a, int index)
{
<<<<<<< HEAD
	struct tcf_pedit_parms *parms;
	u32 val;

	rcu_read_lock();
	parms = to_pedit_parms(a);
	val = parms->tcfp_keys[index].val;
	rcu_read_unlock();

	return val;
=======
	return to_pedit(a)->tcfp_keys[index].val;
>>>>>>> b7ba80a49124 (Commit)
}

static inline u32 tcf_pedit_offset(const struct tc_action *a, int index)
{
<<<<<<< HEAD
	struct tcf_pedit_parms *parms;
	u32 off;

	rcu_read_lock();
	parms = to_pedit_parms(a);
	off = parms->tcfp_keys[index].off;
	rcu_read_unlock();

	return off;
=======
	return to_pedit(a)->tcfp_keys[index].off;
>>>>>>> b7ba80a49124 (Commit)
}
#endif /* __NET_TC_PED_H */
