<<<<<<< HEAD
/* SPDX-License-Identifier: ((GPL-2.0 WITH Linux-syscall-note) OR BSD-3-Clause) */
/* Do not edit directly, auto-generated from: */
/*	Documentation/netlink/specs/fou.yaml */
/* YNL-GEN uapi header */
=======
/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/* fou.h - FOU Interface */
>>>>>>> b7ba80a49124 (Commit)

#ifndef _UAPI_LINUX_FOU_H
#define _UAPI_LINUX_FOU_H

<<<<<<< HEAD
#define FOU_GENL_NAME		"fou"
#define FOU_GENL_VERSION	1

enum {
	FOU_ENCAP_UNSPEC,
	FOU_ENCAP_DIRECT,
	FOU_ENCAP_GUE,
};

enum {
	FOU_ATTR_UNSPEC,
	FOU_ATTR_PORT,
	FOU_ATTR_AF,
	FOU_ATTR_IPPROTO,
	FOU_ATTR_TYPE,
	FOU_ATTR_REMCSUM_NOPARTIAL,
	FOU_ATTR_LOCAL_V4,
	FOU_ATTR_LOCAL_V6,
	FOU_ATTR_PEER_V4,
	FOU_ATTR_PEER_V6,
	FOU_ATTR_PEER_PORT,
	FOU_ATTR_IFINDEX,

	__FOU_ATTR_MAX
};
#define FOU_ATTR_MAX (__FOU_ATTR_MAX - 1)
=======
/* NETLINK_GENERIC related info
 */
#define FOU_GENL_NAME		"fou"
#define FOU_GENL_VERSION	0x1

enum {
	FOU_ATTR_UNSPEC,
	FOU_ATTR_PORT,				/* u16 */
	FOU_ATTR_AF,				/* u8 */
	FOU_ATTR_IPPROTO,			/* u8 */
	FOU_ATTR_TYPE,				/* u8 */
	FOU_ATTR_REMCSUM_NOPARTIAL,		/* flag */
	FOU_ATTR_LOCAL_V4,			/* u32 */
	FOU_ATTR_LOCAL_V6,			/* in6_addr */
	FOU_ATTR_PEER_V4,			/* u32 */
	FOU_ATTR_PEER_V6,			/* in6_addr */
	FOU_ATTR_PEER_PORT,			/* u16 */
	FOU_ATTR_IFINDEX,			/* s32 */

	__FOU_ATTR_MAX,
};

#define FOU_ATTR_MAX		(__FOU_ATTR_MAX - 1)
>>>>>>> b7ba80a49124 (Commit)

enum {
	FOU_CMD_UNSPEC,
	FOU_CMD_ADD,
	FOU_CMD_DEL,
	FOU_CMD_GET,

<<<<<<< HEAD
	__FOU_CMD_MAX
};
#define FOU_CMD_MAX (__FOU_CMD_MAX - 1)
=======
	__FOU_CMD_MAX,
};

enum {
	FOU_ENCAP_UNSPEC,
	FOU_ENCAP_DIRECT,
	FOU_ENCAP_GUE,
};

#define FOU_CMD_MAX	(__FOU_CMD_MAX - 1)
>>>>>>> b7ba80a49124 (Commit)

#endif /* _UAPI_LINUX_FOU_H */
