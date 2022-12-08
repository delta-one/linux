/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_DMA_MAPPING_H
#define _ASM_X86_DMA_MAPPING_H

extern const struct dma_map_ops *dma_ops;

<<<<<<< HEAD
static inline const struct dma_map_ops *get_arch_dma_ops(void)
=======
static inline const struct dma_map_ops *get_arch_dma_ops(struct bus_type *bus)
>>>>>>> b7ba80a49124 (Commit)
{
	return dma_ops;
}

#endif
