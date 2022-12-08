// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2014 MundoReader S.L.
 * Author: Heiko Stuebner <heiko@sntech.de>
 */

#include <linux/slab.h>
#include <linux/io.h>
#include <linux/reset-controller.h>
#include <linux/spinlock.h>
#include "clk.h"

struct rockchip_softrst {
	struct reset_controller_dev	rcdev;
<<<<<<< HEAD
	const int			*lut;
=======
>>>>>>> b7ba80a49124 (Commit)
	void __iomem			*reg_base;
	int				num_regs;
	int				num_per_reg;
	u8				flags;
	spinlock_t			lock;
};

static int rockchip_softrst_assert(struct reset_controller_dev *rcdev,
			      unsigned long id)
{
	struct rockchip_softrst *softrst = container_of(rcdev,
						     struct rockchip_softrst,
						     rcdev);
<<<<<<< HEAD
	int bank, offset;

	if (softrst->lut)
		id = softrst->lut[id];

	bank = id / softrst->num_per_reg;
	offset = id % softrst->num_per_reg;
=======
	int bank = id / softrst->num_per_reg;
	int offset = id % softrst->num_per_reg;
>>>>>>> b7ba80a49124 (Commit)

	if (softrst->flags & ROCKCHIP_SOFTRST_HIWORD_MASK) {
		writel(BIT(offset) | (BIT(offset) << 16),
		       softrst->reg_base + (bank * 4));
	} else {
		unsigned long flags;
		u32 reg;

		spin_lock_irqsave(&softrst->lock, flags);

		reg = readl(softrst->reg_base + (bank * 4));
		writel(reg | BIT(offset), softrst->reg_base + (bank * 4));

		spin_unlock_irqrestore(&softrst->lock, flags);
	}

	return 0;
}

static int rockchip_softrst_deassert(struct reset_controller_dev *rcdev,
				unsigned long id)
{
	struct rockchip_softrst *softrst = container_of(rcdev,
						     struct rockchip_softrst,
						     rcdev);
<<<<<<< HEAD
	int bank, offset;

	if (softrst->lut)
		id = softrst->lut[id];

	bank = id / softrst->num_per_reg;
	offset = id % softrst->num_per_reg;
=======
	int bank = id / softrst->num_per_reg;
	int offset = id % softrst->num_per_reg;
>>>>>>> b7ba80a49124 (Commit)

	if (softrst->flags & ROCKCHIP_SOFTRST_HIWORD_MASK) {
		writel((BIT(offset) << 16), softrst->reg_base + (bank * 4));
	} else {
		unsigned long flags;
		u32 reg;

		spin_lock_irqsave(&softrst->lock, flags);

		reg = readl(softrst->reg_base + (bank * 4));
		writel(reg & ~BIT(offset), softrst->reg_base + (bank * 4));

		spin_unlock_irqrestore(&softrst->lock, flags);
	}

	return 0;
}

static const struct reset_control_ops rockchip_softrst_ops = {
	.assert		= rockchip_softrst_assert,
	.deassert	= rockchip_softrst_deassert,
};

<<<<<<< HEAD
void rockchip_register_softrst_lut(struct device_node *np,
				   const int *lookup_table,
				   unsigned int num_regs,
				   void __iomem *base, u8 flags)
=======
void rockchip_register_softrst(struct device_node *np,
			       unsigned int num_regs,
			       void __iomem *base, u8 flags)
>>>>>>> b7ba80a49124 (Commit)
{
	struct rockchip_softrst *softrst;
	int ret;

	softrst = kzalloc(sizeof(*softrst), GFP_KERNEL);
	if (!softrst)
		return;

	spin_lock_init(&softrst->lock);

	softrst->reg_base = base;
<<<<<<< HEAD
	softrst->lut = lookup_table;
=======
>>>>>>> b7ba80a49124 (Commit)
	softrst->flags = flags;
	softrst->num_regs = num_regs;
	softrst->num_per_reg = (flags & ROCKCHIP_SOFTRST_HIWORD_MASK) ? 16
								      : 32;

	softrst->rcdev.owner = THIS_MODULE;
<<<<<<< HEAD
	if (lookup_table)
		softrst->rcdev.nr_resets = num_regs;
	else
		softrst->rcdev.nr_resets = num_regs * softrst->num_per_reg;
=======
	softrst->rcdev.nr_resets =  num_regs * softrst->num_per_reg;
>>>>>>> b7ba80a49124 (Commit)
	softrst->rcdev.ops = &rockchip_softrst_ops;
	softrst->rcdev.of_node = np;
	ret = reset_controller_register(&softrst->rcdev);
	if (ret) {
		pr_err("%s: could not register reset controller, %d\n",
		       __func__, ret);
		kfree(softrst);
	}
};
<<<<<<< HEAD
EXPORT_SYMBOL_GPL(rockchip_register_softrst_lut);
=======
EXPORT_SYMBOL_GPL(rockchip_register_softrst);
>>>>>>> b7ba80a49124 (Commit)
