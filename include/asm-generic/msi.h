/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_GENERIC_MSI_H
#define __ASM_GENERIC_MSI_H

#include <linux/types.h>

<<<<<<< HEAD
#ifdef CONFIG_GENERIC_MSI_IRQ
=======
#ifdef CONFIG_GENERIC_MSI_IRQ_DOMAIN
>>>>>>> b7ba80a49124 (Commit)

#ifndef NUM_MSI_ALLOC_SCRATCHPAD_REGS
# define NUM_MSI_ALLOC_SCRATCHPAD_REGS	2
#endif

struct msi_desc;

/**
 * struct msi_alloc_info - Default structure for MSI interrupt allocation.
 * @desc:	Pointer to msi descriptor
 * @hwirq:	Associated hw interrupt number in the domain
 * @scratchpad:	Storage for implementation specific scratch data
 *
 * Architectures can provide their own implementation by not including
 * asm-generic/msi.h into their arch specific header file.
 */
typedef struct msi_alloc_info {
	struct msi_desc			*desc;
	irq_hw_number_t			hwirq;
	unsigned long			flags;
	union {
		unsigned long		ul;
		void			*ptr;
	} scratchpad[NUM_MSI_ALLOC_SCRATCHPAD_REGS];
} msi_alloc_info_t;

/* Device generating MSIs is proxying for another device */
#define MSI_ALLOC_FLAGS_PROXY_DEVICE	(1UL << 0)

#define GENERIC_MSI_DOMAIN_OPS		1

<<<<<<< HEAD
#endif /* CONFIG_GENERIC_MSI_IRQ */
=======
#endif /* CONFIG_GENERIC_MSI_IRQ_DOMAIN */
>>>>>>> b7ba80a49124 (Commit)

#endif
