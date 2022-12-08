/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_GENERIC_DMA_MAPPING_H
#define _ASM_GENERIC_DMA_MAPPING_H

<<<<<<< HEAD
static inline const struct dma_map_ops *get_arch_dma_ops(void)
=======
static inline const struct dma_map_ops *get_arch_dma_ops(struct bus_type *bus)
>>>>>>> b7ba80a49124 (Commit)
{
	return NULL;
}

#endif /* _ASM_GENERIC_DMA_MAPPING_H */
