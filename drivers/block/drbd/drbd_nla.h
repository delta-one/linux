<<<<<<< HEAD
/* SPDX-License-Identifier: GPL-2.0-only */
=======
/* SPDX-License-Identifier: GPL-2.0 */
>>>>>>> b7ba80a49124 (Commit)
#ifndef __DRBD_NLA_H
#define __DRBD_NLA_H

extern int drbd_nla_parse_nested(struct nlattr *tb[], int maxtype, struct nlattr *nla,
				 const struct nla_policy *policy);
extern struct nlattr *drbd_nla_find_nested(int maxtype, struct nlattr *nla, int attrtype);

#endif  /* __DRBD_NLA_H */
