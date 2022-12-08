/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_RPMSG_QCOM_GLINK_H
#define _LINUX_RPMSG_QCOM_GLINK_H

#include <linux/device.h>

<<<<<<< HEAD
struct qcom_glink_smem;
=======
struct qcom_glink;
>>>>>>> b7ba80a49124 (Commit)

#if IS_ENABLED(CONFIG_RPMSG_QCOM_GLINK)
void qcom_glink_ssr_notify(const char *ssr_name);
#else
static inline void qcom_glink_ssr_notify(const char *ssr_name) {}
#endif

#if IS_ENABLED(CONFIG_RPMSG_QCOM_GLINK_SMEM)

<<<<<<< HEAD
struct qcom_glink_smem *qcom_glink_smem_register(struct device *parent,
						 struct device_node *node);
void qcom_glink_smem_unregister(struct qcom_glink_smem *glink);

#else

static inline struct qcom_glink_smem *
=======
struct qcom_glink *qcom_glink_smem_register(struct device *parent,
					    struct device_node *node);
void qcom_glink_smem_unregister(struct qcom_glink *glink);

#else

static inline struct qcom_glink *
>>>>>>> b7ba80a49124 (Commit)
qcom_glink_smem_register(struct device *parent,
			 struct device_node *node)
{
	return NULL;
}

<<<<<<< HEAD
static inline void qcom_glink_smem_unregister(struct qcom_glink_smem *glink) {}
=======
static inline void qcom_glink_smem_unregister(struct qcom_glink *glink) {}
>>>>>>> b7ba80a49124 (Commit)
#endif

#endif
