// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2017 MediaTek Inc.
 * Author: Weiyi Lu <weiyi.lu@mediatek.com>
 */

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-mtk.h"
#include "clk-gate.h"

#include <dt-bindings/clock/mt2712-clk.h>

static const struct mtk_gate_regs mfg_cg_regs = {
	.set_ofs = 0x4,
	.clr_ofs = 0x8,
	.sta_ofs = 0x0,
};

<<<<<<< HEAD
#define GATE_MFG(_id, _name, _parent, _shift)			\
	GATE_MTK(_id, _name, _parent, &mfg_cg_regs, _shift, &mtk_clk_gate_ops_setclr)
=======
#define GATE_MFG(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &mfg_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}
>>>>>>> b7ba80a49124 (Commit)

static const struct mtk_gate mfg_clks[] = {
	GATE_MFG(CLK_MFG_BG3D, "mfg_bg3d", "mfg_sel", 0),
};

<<<<<<< HEAD
static const struct mtk_clk_desc mfg_desc = {
	.clks = mfg_clks,
	.num_clks = ARRAY_SIZE(mfg_clks),
};

static const struct of_device_id of_match_clk_mt2712_mfg[] = {
	{
		.compatible = "mediatek,mt2712-mfgcfg",
		.data = &mfg_desc,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt2712_mfg);

static struct platform_driver clk_mt2712_mfg_drv = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
=======
static int clk_mt2712_mfg_probe(struct platform_device *pdev)
{
	struct clk_hw_onecell_data *clk_data;
	int r;
	struct device_node *node = pdev->dev.of_node;

	clk_data = mtk_alloc_clk_data(CLK_MFG_NR_CLK);

	mtk_clk_register_gates(node, mfg_clks, ARRAY_SIZE(mfg_clks),
			clk_data);

	r = of_clk_add_hw_provider(node, of_clk_hw_onecell_get, clk_data);

	if (r != 0)
		pr_err("%s(): could not register clock provider: %d\n",
			__func__, r);

	return r;
}

static const struct of_device_id of_match_clk_mt2712_mfg[] = {
	{ .compatible = "mediatek,mt2712-mfgcfg", },
	{}
};

static struct platform_driver clk_mt2712_mfg_drv = {
	.probe = clk_mt2712_mfg_probe,
>>>>>>> b7ba80a49124 (Commit)
	.driver = {
		.name = "clk-mt2712-mfg",
		.of_match_table = of_match_clk_mt2712_mfg,
	},
};
<<<<<<< HEAD
module_platform_driver(clk_mt2712_mfg_drv);
MODULE_LICENSE("GPL");
=======

builtin_platform_driver(clk_mt2712_mfg_drv);
>>>>>>> b7ba80a49124 (Commit)
