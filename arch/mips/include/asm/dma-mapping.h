/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_DMA_MAPPING_H
#define _ASM_DMA_MAPPING_H

#include <linux/swiotlb.h>

extern const struct dma_map_ops jazz_dma_ops;

<<<<<<< HEAD
static inline const struct dma_map_ops *get_arch_dma_ops(void)
=======
static inline const struct dma_map_ops *get_arch_dma_ops(struct bus_type *bus)
>>>>>>> b7ba80a49124 (Commit)
{
#if defined(CONFIG_MACH_JAZZ)
	return &jazz_dma_ops;
#else
	return NULL;
#endif
}

#endif /* _ASM_DMA_MAPPING_H */
