/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_ARM_XEN_OPS_H
#define _ASM_ARM_XEN_OPS_H

#include <xen/swiotlb-xen.h>
#include <xen/xen-ops.h>

static inline void xen_setup_dma_ops(struct device *dev)
{
#ifdef CONFIG_XEN
<<<<<<< HEAD
	if (xen_swiotlb_detect())
=======
	if (xen_is_grant_dma_device(dev))
		xen_grant_setup_dma_ops(dev);
	else if (xen_swiotlb_detect())
>>>>>>> b7ba80a49124 (Commit)
		dev->dma_ops = &xen_swiotlb_dma_ops;
#endif
}

#endif /* _ASM_ARM_XEN_OPS_H */
