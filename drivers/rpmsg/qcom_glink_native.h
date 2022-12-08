/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2016-2017, Linaro Ltd
 */

#ifndef __QCOM_GLINK_NATIVE_H__
#define __QCOM_GLINK_NATIVE_H__

<<<<<<< HEAD
#include <linux/types.h>

=======
>>>>>>> b7ba80a49124 (Commit)
#define GLINK_FEATURE_INTENT_REUSE	BIT(0)
#define GLINK_FEATURE_MIGRATION		BIT(1)
#define GLINK_FEATURE_TRACER_PKT	BIT(2)

struct qcom_glink_pipe {
	size_t length;

	size_t (*avail)(struct qcom_glink_pipe *glink_pipe);

<<<<<<< HEAD
	void (*peek)(struct qcom_glink_pipe *glink_pipe, void *data,
=======
	void (*peak)(struct qcom_glink_pipe *glink_pipe, void *data,
>>>>>>> b7ba80a49124 (Commit)
		     unsigned int offset, size_t count);
	void (*advance)(struct qcom_glink_pipe *glink_pipe, size_t count);

	void (*write)(struct qcom_glink_pipe *glink_pipe,
		      const void *hdr, size_t hlen,
		      const void *data, size_t dlen);
<<<<<<< HEAD
	void (*kick)(struct qcom_glink_pipe *glink_pipe);
};

struct device;
=======
};

>>>>>>> b7ba80a49124 (Commit)
struct qcom_glink;

struct qcom_glink *qcom_glink_native_probe(struct device *dev,
					   unsigned long features,
					   struct qcom_glink_pipe *rx,
					   struct qcom_glink_pipe *tx,
					   bool intentless);
void qcom_glink_native_remove(struct qcom_glink *glink);
<<<<<<< HEAD
void qcom_glink_native_rx(struct qcom_glink *glink);

=======

void qcom_glink_native_unregister(struct qcom_glink *glink);
>>>>>>> b7ba80a49124 (Commit)
#endif
