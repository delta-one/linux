// SPDX-License-Identifier: GPL-2.0
/*
 * ZynqMP DisplayPort Subsystem Driver
 *
 * Copyright (C) 2017 - 2020 Xilinx, Inc.
 *
 * Authors:
 * - Hyun Woo Kwon <hyun.kwon@xilinx.com>
 * - Laurent Pinchart <laurent.pinchart@ideasonboard.com>
 */

#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
<<<<<<< HEAD
#include <linux/of_graph.h>
#include <linux/of_reserved_mem.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>

#include <drm/drm_atomic_helper.h>
#include <drm/drm_bridge.h>
#include <drm/drm_modeset_helper.h>
#include <drm/drm_module.h>
=======
#include <linux/of_reserved_mem.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>

#include <drm/drm_atomic_helper.h>
#include <drm/drm_device.h>
#include <drm/drm_drv.h>
#include <drm/drm_fb_helper.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_gem_dma_helper.h>
#include <drm/drm_gem_framebuffer_helper.h>
#include <drm/drm_managed.h>
#include <drm/drm_mode_config.h>
#include <drm/drm_module.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_vblank.h>
>>>>>>> b7ba80a49124 (Commit)

#include "zynqmp_disp.h"
#include "zynqmp_dp.h"
#include "zynqmp_dpsub.h"
<<<<<<< HEAD
#include "zynqmp_kms.h"
=======

/* -----------------------------------------------------------------------------
 * Dumb Buffer & Framebuffer Allocation
 */

static int zynqmp_dpsub_dumb_create(struct drm_file *file_priv,
				    struct drm_device *drm,
				    struct drm_mode_create_dumb *args)
{
	struct zynqmp_dpsub *dpsub = to_zynqmp_dpsub(drm);
	unsigned int pitch = DIV_ROUND_UP(args->width * args->bpp, 8);

	/* Enforce the alignment constraints of the DMA engine. */
	args->pitch = ALIGN(pitch, dpsub->dma_align);

	return drm_gem_dma_dumb_create_internal(file_priv, drm, args);
}

static struct drm_framebuffer *
zynqmp_dpsub_fb_create(struct drm_device *drm, struct drm_file *file_priv,
		       const struct drm_mode_fb_cmd2 *mode_cmd)
{
	struct zynqmp_dpsub *dpsub = to_zynqmp_dpsub(drm);
	struct drm_mode_fb_cmd2 cmd = *mode_cmd;
	unsigned int i;

	/* Enforce the alignment constraints of the DMA engine. */
	for (i = 0; i < ARRAY_SIZE(cmd.pitches); ++i)
		cmd.pitches[i] = ALIGN(cmd.pitches[i], dpsub->dma_align);

	return drm_gem_fb_create(drm, file_priv, &cmd);
}

static const struct drm_mode_config_funcs zynqmp_dpsub_mode_config_funcs = {
	.fb_create		= zynqmp_dpsub_fb_create,
	.atomic_check		= drm_atomic_helper_check,
	.atomic_commit		= drm_atomic_helper_commit,
};

/* -----------------------------------------------------------------------------
 * DRM/KMS Driver
 */

DEFINE_DRM_GEM_DMA_FOPS(zynqmp_dpsub_drm_fops);

static const struct drm_driver zynqmp_dpsub_drm_driver = {
	.driver_features		= DRIVER_MODESET | DRIVER_GEM |
					  DRIVER_ATOMIC,

	DRM_GEM_DMA_DRIVER_OPS_WITH_DUMB_CREATE(zynqmp_dpsub_dumb_create),

	.fops				= &zynqmp_dpsub_drm_fops,

	.name				= "zynqmp-dpsub",
	.desc				= "Xilinx DisplayPort Subsystem Driver",
	.date				= "20130509",
	.major				= 1,
	.minor				= 0,
};

static int zynqmp_dpsub_drm_init(struct zynqmp_dpsub *dpsub)
{
	struct drm_device *drm = &dpsub->drm;
	int ret;

	/* Initialize mode config, vblank and the KMS poll helper. */
	ret = drmm_mode_config_init(drm);
	if (ret < 0)
		return ret;

	drm->mode_config.funcs = &zynqmp_dpsub_mode_config_funcs;
	drm->mode_config.min_width = 0;
	drm->mode_config.min_height = 0;
	drm->mode_config.max_width = ZYNQMP_DISP_MAX_WIDTH;
	drm->mode_config.max_height = ZYNQMP_DISP_MAX_HEIGHT;

	ret = drm_vblank_init(drm, 1);
	if (ret)
		return ret;

	drm_kms_helper_poll_init(drm);

	/*
	 * Initialize the DISP and DP components. This will creates planes,
	 * CRTC, encoder and connector. The DISP should be initialized first as
	 * the DP encoder needs the CRTC.
	 */
	ret = zynqmp_disp_drm_init(dpsub);
	if (ret)
		goto err_poll_fini;

	ret = zynqmp_dp_drm_init(dpsub);
	if (ret)
		goto err_poll_fini;

	/* Reset all components and register the DRM device. */
	drm_mode_config_reset(drm);

	ret = drm_dev_register(drm, 0);
	if (ret < 0)
		goto err_poll_fini;

	/* Initialize fbdev generic emulation. */
	drm_fbdev_generic_setup(drm, 24);

	return 0;

err_poll_fini:
	drm_kms_helper_poll_fini(drm);
	return ret;
}
>>>>>>> b7ba80a49124 (Commit)

/* -----------------------------------------------------------------------------
 * Power Management
 */

static int __maybe_unused zynqmp_dpsub_suspend(struct device *dev)
{
	struct zynqmp_dpsub *dpsub = dev_get_drvdata(dev);

<<<<<<< HEAD
	if (!dpsub->drm)
		return 0;

	return drm_mode_config_helper_suspend(&dpsub->drm->dev);
=======
	return drm_mode_config_helper_suspend(&dpsub->drm);
>>>>>>> b7ba80a49124 (Commit)
}

static int __maybe_unused zynqmp_dpsub_resume(struct device *dev)
{
	struct zynqmp_dpsub *dpsub = dev_get_drvdata(dev);

<<<<<<< HEAD
	if (!dpsub->drm)
		return 0;

	return drm_mode_config_helper_resume(&dpsub->drm->dev);
=======
	return drm_mode_config_helper_resume(&dpsub->drm);
>>>>>>> b7ba80a49124 (Commit)
}

static const struct dev_pm_ops zynqmp_dpsub_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(zynqmp_dpsub_suspend, zynqmp_dpsub_resume)
};

/* -----------------------------------------------------------------------------
<<<<<<< HEAD
 * DPSUB Configuration
 */

/**
 * zynqmp_dpsub_audio_enabled - If the audio is enabled
 * @dpsub: DisplayPort subsystem
 *
 * Return if the audio is enabled depending on the audio clock.
 *
 * Return: true if audio is enabled, or false.
 */
bool zynqmp_dpsub_audio_enabled(struct zynqmp_dpsub *dpsub)
{
	return !!dpsub->aud_clk;
}

/**
 * zynqmp_dpsub_get_audio_clk_rate - Get the current audio clock rate
 * @dpsub: DisplayPort subsystem
 *
 * Return: the current audio clock rate.
 */
unsigned int zynqmp_dpsub_get_audio_clk_rate(struct zynqmp_dpsub *dpsub)
{
	if (zynqmp_dpsub_audio_enabled(dpsub))
		return 0;
	return clk_get_rate(dpsub->aud_clk);
}

/* -----------------------------------------------------------------------------
=======
>>>>>>> b7ba80a49124 (Commit)
 * Probe & Remove
 */

static int zynqmp_dpsub_init_clocks(struct zynqmp_dpsub *dpsub)
{
	int ret;

	dpsub->apb_clk = devm_clk_get(dpsub->dev, "dp_apb_clk");
	if (IS_ERR(dpsub->apb_clk))
		return PTR_ERR(dpsub->apb_clk);

	ret = clk_prepare_enable(dpsub->apb_clk);
	if (ret) {
		dev_err(dpsub->dev, "failed to enable the APB clock\n");
		return ret;
	}

<<<<<<< HEAD
	/*
	 * Try the live PL video clock, and fall back to the PS clock if the
	 * live PL video clock isn't valid.
	 */
	dpsub->vid_clk = devm_clk_get(dpsub->dev, "dp_live_video_in_clk");
	if (!IS_ERR(dpsub->vid_clk))
		dpsub->vid_clk_from_ps = false;
	else if (PTR_ERR(dpsub->vid_clk) == -EPROBE_DEFER)
		return PTR_ERR(dpsub->vid_clk);

	if (IS_ERR_OR_NULL(dpsub->vid_clk)) {
		dpsub->vid_clk = devm_clk_get(dpsub->dev, "dp_vtc_pixel_clk_in");
		if (IS_ERR(dpsub->vid_clk)) {
			dev_err(dpsub->dev, "failed to init any video clock\n");
			return PTR_ERR(dpsub->vid_clk);
		}
		dpsub->vid_clk_from_ps = true;
	}

	/*
	 * Try the live PL audio clock, and fall back to the PS clock if the
	 * live PL audio clock isn't valid. Missing audio clock disables audio
	 * but isn't an error.
	 */
	dpsub->aud_clk = devm_clk_get(dpsub->dev, "dp_live_audio_aclk");
	if (!IS_ERR(dpsub->aud_clk)) {
		dpsub->aud_clk_from_ps = false;
		return 0;
	}

	dpsub->aud_clk = devm_clk_get(dpsub->dev, "dp_aud_clk");
	if (!IS_ERR(dpsub->aud_clk)) {
		dpsub->aud_clk_from_ps = true;
		return 0;
	}

	dev_info(dpsub->dev, "audio disabled due to missing clock\n");
	return 0;
}

static int zynqmp_dpsub_parse_dt(struct zynqmp_dpsub *dpsub)
{
	struct device_node *np;
	unsigned int i;

	/*
	 * For backward compatibility with old device trees that don't contain
	 * ports, consider that only the DP output port is connected if no
	 * ports child no exists.
	 */
	np = of_get_child_by_name(dpsub->dev->of_node, "ports");
	of_node_put(np);
	if (!np) {
		dev_warn(dpsub->dev, "missing ports, update DT bindings\n");
		dpsub->connected_ports = BIT(ZYNQMP_DPSUB_PORT_OUT_DP);
		dpsub->dma_enabled = true;
		return 0;
	}

	/* Check which ports are connected. */
	for (i = 0; i < ZYNQMP_DPSUB_NUM_PORTS; ++i) {
		struct device_node *np;

		np = of_graph_get_remote_node(dpsub->dev->of_node, i, -1);
		if (np) {
			dpsub->connected_ports |= BIT(i);
			of_node_put(np);
		}
	}

	/* Sanity checks. */
	if ((dpsub->connected_ports & BIT(ZYNQMP_DPSUB_PORT_LIVE_VIDEO)) &&
	    (dpsub->connected_ports & BIT(ZYNQMP_DPSUB_PORT_LIVE_GFX))) {
		dev_err(dpsub->dev, "only one live video input is supported\n");
		return -EINVAL;
	}

	if ((dpsub->connected_ports & BIT(ZYNQMP_DPSUB_PORT_LIVE_VIDEO)) ||
	    (dpsub->connected_ports & BIT(ZYNQMP_DPSUB_PORT_LIVE_GFX))) {
		if (dpsub->vid_clk_from_ps) {
			dev_err(dpsub->dev,
				"live video input requires PL clock\n");
			return -EINVAL;
		}
	} else {
		dpsub->dma_enabled = true;
	}

	if (dpsub->connected_ports & BIT(ZYNQMP_DPSUB_PORT_LIVE_AUDIO))
		dev_warn(dpsub->dev, "live audio unsupported, ignoring\n");

	if ((dpsub->connected_ports & BIT(ZYNQMP_DPSUB_PORT_OUT_VIDEO)) ||
	    (dpsub->connected_ports & BIT(ZYNQMP_DPSUB_PORT_OUT_AUDIO)))
		dev_warn(dpsub->dev, "output to PL unsupported, ignoring\n");

	if (!(dpsub->connected_ports & BIT(ZYNQMP_DPSUB_PORT_OUT_DP))) {
		dev_err(dpsub->dev, "DP output port not connected\n");
		return -EINVAL;
	}

	return 0;
}

void zynqmp_dpsub_release(struct zynqmp_dpsub *dpsub)
{
	kfree(dpsub->disp);
	kfree(dpsub->dp);
	kfree(dpsub);
}

=======
	return 0;
}

>>>>>>> b7ba80a49124 (Commit)
static int zynqmp_dpsub_probe(struct platform_device *pdev)
{
	struct zynqmp_dpsub *dpsub;
	int ret;

	/* Allocate private data. */
<<<<<<< HEAD
	dpsub = kzalloc(sizeof(*dpsub), GFP_KERNEL);
	if (!dpsub)
		return -ENOMEM;
=======
	dpsub = devm_drm_dev_alloc(&pdev->dev, &zynqmp_dpsub_drm_driver,
				   struct zynqmp_dpsub, drm);
	if (IS_ERR(dpsub))
		return PTR_ERR(dpsub);
>>>>>>> b7ba80a49124 (Commit)

	dpsub->dev = &pdev->dev;
	platform_set_drvdata(pdev, dpsub);

	dma_set_mask(dpsub->dev, DMA_BIT_MASK(ZYNQMP_DISP_MAX_DMA_BIT));

	/* Try the reserved memory. Proceed if there's none. */
	of_reserved_mem_device_init(&pdev->dev);

	ret = zynqmp_dpsub_init_clocks(dpsub);
	if (ret < 0)
		goto err_mem;

<<<<<<< HEAD
	ret = zynqmp_dpsub_parse_dt(dpsub);
	if (ret < 0)
		goto err_mem;

=======
>>>>>>> b7ba80a49124 (Commit)
	pm_runtime_enable(&pdev->dev);

	/*
	 * DP should be probed first so that the zynqmp_disp can set the output
	 * format accordingly.
	 */
<<<<<<< HEAD
	ret = zynqmp_dp_probe(dpsub);
	if (ret)
		goto err_pm;

	ret = zynqmp_disp_probe(dpsub);
	if (ret)
		goto err_dp;

	if (dpsub->dma_enabled) {
		ret = zynqmp_dpsub_drm_init(dpsub);
		if (ret)
			goto err_disp;
	} else {
		drm_bridge_add(dpsub->bridge);
	}
=======
	ret = zynqmp_dp_probe(dpsub, &dpsub->drm);
	if (ret)
		goto err_pm;

	ret = zynqmp_disp_probe(dpsub, &dpsub->drm);
	if (ret)
		goto err_dp;

	ret = zynqmp_dpsub_drm_init(dpsub);
	if (ret)
		goto err_disp;
>>>>>>> b7ba80a49124 (Commit)

	dev_info(&pdev->dev, "ZynqMP DisplayPort Subsystem driver probed");

	return 0;

err_disp:
	zynqmp_disp_remove(dpsub);
err_dp:
	zynqmp_dp_remove(dpsub);
err_pm:
	pm_runtime_disable(&pdev->dev);
	clk_disable_unprepare(dpsub->apb_clk);
err_mem:
	of_reserved_mem_device_release(&pdev->dev);
<<<<<<< HEAD
	if (!dpsub->drm)
		zynqmp_dpsub_release(dpsub);
=======
>>>>>>> b7ba80a49124 (Commit)
	return ret;
}

static int zynqmp_dpsub_remove(struct platform_device *pdev)
{
	struct zynqmp_dpsub *dpsub = platform_get_drvdata(pdev);
<<<<<<< HEAD

	if (dpsub->drm)
		zynqmp_dpsub_drm_cleanup(dpsub);
	else
		drm_bridge_remove(dpsub->bridge);
=======
	struct drm_device *drm = &dpsub->drm;

	drm_dev_unregister(drm);
	drm_atomic_helper_shutdown(drm);
	drm_kms_helper_poll_fini(drm);
>>>>>>> b7ba80a49124 (Commit)

	zynqmp_disp_remove(dpsub);
	zynqmp_dp_remove(dpsub);

	pm_runtime_disable(&pdev->dev);
	clk_disable_unprepare(dpsub->apb_clk);
	of_reserved_mem_device_release(&pdev->dev);

<<<<<<< HEAD
	if (!dpsub->drm)
		zynqmp_dpsub_release(dpsub);

=======
>>>>>>> b7ba80a49124 (Commit)
	return 0;
}

static void zynqmp_dpsub_shutdown(struct platform_device *pdev)
{
	struct zynqmp_dpsub *dpsub = platform_get_drvdata(pdev);

<<<<<<< HEAD
	if (!dpsub->drm)
		return;

	drm_atomic_helper_shutdown(&dpsub->drm->dev);
=======
	drm_atomic_helper_shutdown(&dpsub->drm);
>>>>>>> b7ba80a49124 (Commit)
}

static const struct of_device_id zynqmp_dpsub_of_match[] = {
	{ .compatible = "xlnx,zynqmp-dpsub-1.7", },
	{ /* end of table */ },
};
MODULE_DEVICE_TABLE(of, zynqmp_dpsub_of_match);

static struct platform_driver zynqmp_dpsub_driver = {
	.probe			= zynqmp_dpsub_probe,
	.remove			= zynqmp_dpsub_remove,
	.shutdown		= zynqmp_dpsub_shutdown,
	.driver			= {
		.name		= "zynqmp-dpsub",
		.pm		= &zynqmp_dpsub_pm_ops,
		.of_match_table	= zynqmp_dpsub_of_match,
	},
};

drm_module_platform_driver(zynqmp_dpsub_driver);

MODULE_AUTHOR("Xilinx, Inc.");
MODULE_DESCRIPTION("ZynqMP DP Subsystem Driver");
MODULE_LICENSE("GPL v2");
