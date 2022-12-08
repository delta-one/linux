/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __NETNS_XDP_H__
#define __NETNS_XDP_H__

<<<<<<< HEAD
#include <linux/mutex.h>
#include <linux/types.h>
=======
#include <linux/rculist.h>
#include <linux/mutex.h>
>>>>>>> b7ba80a49124 (Commit)

struct netns_xdp {
	struct mutex		lock;
	struct hlist_head	list;
};

#endif /* __NETNS_XDP_H__ */
