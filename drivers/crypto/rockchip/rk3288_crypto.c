// SPDX-License-Identifier: GPL-2.0-only
/*
 * Crypto acceleration support for Rockchip RK3288
 *
 * Copyright (c) 2015, Fuzhou Rockchip Electronics Co., Ltd
 *
 * Author: Zain Wang <zain.wang@rock-chips.com>
 *
 * Some ideas are from marvell-cesa.c and s5p-sss.c driver.
 */

#include "rk3288_crypto.h"
#include <linux/dma-mapping.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
<<<<<<< HEAD
#include <linux/of_device.h>
=======
>>>>>>> b7ba80a49124 (Commit)
#include <linux/clk.h>
#include <linux/crypto.h>
#include <linux/reset.h>

<<<<<<< HEAD
static struct rockchip_ip rocklist = {
	.dev_list = LIST_HEAD_INIT(rocklist.dev_list),
	.lock = __SPIN_LOCK_UNLOCKED(rocklist.lock),
};

struct rk_crypto_info *get_rk_crypto(void)
{
	struct rk_crypto_info *first;

	spin_lock(&rocklist.lock);
	first = list_first_entry_or_null(&rocklist.dev_list,
					 struct rk_crypto_info, list);
	list_rotate_left(&rocklist.dev_list);
	spin_unlock(&rocklist.lock);
	return first;
}

static const struct rk_variant rk3288_variant = {
	.num_clks = 4,
	.rkclks = {
		{ "sclk", 150000000},
	}
};

static const struct rk_variant rk3328_variant = {
	.num_clks = 3,
};

static const struct rk_variant rk3399_variant = {
	.num_clks = 3,
};

static int rk_crypto_get_clks(struct rk_crypto_info *dev)
{
	int i, j, err;
	unsigned long cr;

	dev->num_clks = devm_clk_bulk_get_all(dev->dev, &dev->clks);
	if (dev->num_clks < dev->variant->num_clks) {
		dev_err(dev->dev, "Missing clocks, got %d instead of %d\n",
			dev->num_clks, dev->variant->num_clks);
		return -EINVAL;
	}

	for (i = 0; i < dev->num_clks; i++) {
		cr = clk_get_rate(dev->clks[i].clk);
		for (j = 0; j < ARRAY_SIZE(dev->variant->rkclks); j++) {
			if (dev->variant->rkclks[j].max == 0)
				continue;
			if (strcmp(dev->variant->rkclks[j].name, dev->clks[i].id))
				continue;
			if (cr > dev->variant->rkclks[j].max) {
				err = clk_set_rate(dev->clks[i].clk,
						   dev->variant->rkclks[j].max);
				if (err)
					dev_err(dev->dev, "Fail downclocking %s from %lu to %lu\n",
						dev->variant->rkclks[j].name, cr,
						dev->variant->rkclks[j].max);
				else
					dev_info(dev->dev, "Downclocking %s from %lu to %lu\n",
						 dev->variant->rkclks[j].name, cr,
						 dev->variant->rkclks[j].max);
			}
		}
	}
	return 0;
}

=======
>>>>>>> b7ba80a49124 (Commit)
static int rk_crypto_enable_clk(struct rk_crypto_info *dev)
{
	int err;

<<<<<<< HEAD
	err = clk_bulk_prepare_enable(dev->num_clks, dev->clks);
	if (err)
		dev_err(dev->dev, "Could not enable clock clks\n");

=======
	err = clk_prepare_enable(dev->sclk);
	if (err) {
		dev_err(dev->dev, "[%s:%d], Couldn't enable clock sclk\n",
			__func__, __LINE__);
		goto err_return;
	}
	err = clk_prepare_enable(dev->aclk);
	if (err) {
		dev_err(dev->dev, "[%s:%d], Couldn't enable clock aclk\n",
			__func__, __LINE__);
		goto err_aclk;
	}
	err = clk_prepare_enable(dev->hclk);
	if (err) {
		dev_err(dev->dev, "[%s:%d], Couldn't enable clock hclk\n",
			__func__, __LINE__);
		goto err_hclk;
	}
	err = clk_prepare_enable(dev->dmaclk);
	if (err) {
		dev_err(dev->dev, "[%s:%d], Couldn't enable clock dmaclk\n",
			__func__, __LINE__);
		goto err_dmaclk;
	}
	return err;
err_dmaclk:
	clk_disable_unprepare(dev->hclk);
err_hclk:
	clk_disable_unprepare(dev->aclk);
err_aclk:
	clk_disable_unprepare(dev->sclk);
err_return:
>>>>>>> b7ba80a49124 (Commit)
	return err;
}

static void rk_crypto_disable_clk(struct rk_crypto_info *dev)
{
<<<<<<< HEAD
	clk_bulk_disable_unprepare(dev->num_clks, dev->clks);
}

/*
 * Power management strategy: The device is suspended until a request
 * is handled. For avoiding suspend/resume yoyo, the autosuspend is set to 2s.
 */
static int rk_crypto_pm_suspend(struct device *dev)
{
	struct rk_crypto_info *rkdev = dev_get_drvdata(dev);

	rk_crypto_disable_clk(rkdev);
	reset_control_assert(rkdev->rst);

	return 0;
}

static int rk_crypto_pm_resume(struct device *dev)
{
	struct rk_crypto_info *rkdev = dev_get_drvdata(dev);
	int ret;

	ret = rk_crypto_enable_clk(rkdev);
	if (ret)
		return ret;

	reset_control_deassert(rkdev->rst);
	return 0;

}

static const struct dev_pm_ops rk_crypto_pm_ops = {
	SET_RUNTIME_PM_OPS(rk_crypto_pm_suspend, rk_crypto_pm_resume, NULL)
};

static int rk_crypto_pm_init(struct rk_crypto_info *rkdev)
{
	int err;

	pm_runtime_use_autosuspend(rkdev->dev);
	pm_runtime_set_autosuspend_delay(rkdev->dev, 2000);

	err = pm_runtime_set_suspended(rkdev->dev);
	if (err)
		return err;
	pm_runtime_enable(rkdev->dev);
	return err;
}

static void rk_crypto_pm_exit(struct rk_crypto_info *rkdev)
{
	pm_runtime_disable(rkdev->dev);
=======
	clk_disable_unprepare(dev->dmaclk);
	clk_disable_unprepare(dev->hclk);
	clk_disable_unprepare(dev->aclk);
	clk_disable_unprepare(dev->sclk);
}

static int check_alignment(struct scatterlist *sg_src,
			   struct scatterlist *sg_dst,
			   int align_mask)
{
	int in, out, align;

	in = IS_ALIGNED((uint32_t)sg_src->offset, 4) &&
	     IS_ALIGNED((uint32_t)sg_src->length, align_mask);
	if (!sg_dst)
		return in;
	out = IS_ALIGNED((uint32_t)sg_dst->offset, 4) &&
	      IS_ALIGNED((uint32_t)sg_dst->length, align_mask);
	align = in && out;

	return (align && (sg_src->length == sg_dst->length));
}

static int rk_load_data(struct rk_crypto_info *dev,
			struct scatterlist *sg_src,
			struct scatterlist *sg_dst)
{
	unsigned int count;

	dev->aligned = dev->aligned ?
		check_alignment(sg_src, sg_dst, dev->align_size) :
		dev->aligned;
	if (dev->aligned) {
		count = min(dev->left_bytes, sg_src->length);
		dev->left_bytes -= count;

		if (!dma_map_sg(dev->dev, sg_src, 1, DMA_TO_DEVICE)) {
			dev_err(dev->dev, "[%s:%d] dma_map_sg(src)  error\n",
				__func__, __LINE__);
			return -EINVAL;
		}
		dev->addr_in = sg_dma_address(sg_src);

		if (sg_dst) {
			if (!dma_map_sg(dev->dev, sg_dst, 1, DMA_FROM_DEVICE)) {
				dev_err(dev->dev,
					"[%s:%d] dma_map_sg(dst)  error\n",
					__func__, __LINE__);
				dma_unmap_sg(dev->dev, sg_src, 1,
					     DMA_TO_DEVICE);
				return -EINVAL;
			}
			dev->addr_out = sg_dma_address(sg_dst);
		}
	} else {
		count = (dev->left_bytes > PAGE_SIZE) ?
			PAGE_SIZE : dev->left_bytes;

		if (!sg_pcopy_to_buffer(dev->first, dev->src_nents,
					dev->addr_vir, count,
					dev->total - dev->left_bytes)) {
			dev_err(dev->dev, "[%s:%d] pcopy err\n",
				__func__, __LINE__);
			return -EINVAL;
		}
		dev->left_bytes -= count;
		sg_init_one(&dev->sg_tmp, dev->addr_vir, count);
		if (!dma_map_sg(dev->dev, &dev->sg_tmp, 1, DMA_TO_DEVICE)) {
			dev_err(dev->dev, "[%s:%d] dma_map_sg(sg_tmp)  error\n",
				__func__, __LINE__);
			return -ENOMEM;
		}
		dev->addr_in = sg_dma_address(&dev->sg_tmp);

		if (sg_dst) {
			if (!dma_map_sg(dev->dev, &dev->sg_tmp, 1,
					DMA_FROM_DEVICE)) {
				dev_err(dev->dev,
					"[%s:%d] dma_map_sg(sg_tmp)  error\n",
					__func__, __LINE__);
				dma_unmap_sg(dev->dev, &dev->sg_tmp, 1,
					     DMA_TO_DEVICE);
				return -ENOMEM;
			}
			dev->addr_out = sg_dma_address(&dev->sg_tmp);
		}
	}
	dev->count = count;
	return 0;
}

static void rk_unload_data(struct rk_crypto_info *dev)
{
	struct scatterlist *sg_in, *sg_out;

	sg_in = dev->aligned ? dev->sg_src : &dev->sg_tmp;
	dma_unmap_sg(dev->dev, sg_in, 1, DMA_TO_DEVICE);

	if (dev->sg_dst) {
		sg_out = dev->aligned ? dev->sg_dst : &dev->sg_tmp;
		dma_unmap_sg(dev->dev, sg_out, 1, DMA_FROM_DEVICE);
	}
>>>>>>> b7ba80a49124 (Commit)
}

static irqreturn_t rk_crypto_irq_handle(int irq, void *dev_id)
{
	struct rk_crypto_info *dev  = platform_get_drvdata(dev_id);
	u32 interrupt_status;

<<<<<<< HEAD
	interrupt_status = CRYPTO_READ(dev, RK_CRYPTO_INTSTS);
	CRYPTO_WRITE(dev, RK_CRYPTO_INTSTS, interrupt_status);

	dev->status = 1;
	if (interrupt_status & 0x0a) {
		dev_warn(dev->dev, "DMA Error\n");
		dev->status = 0;
	}
	complete(&dev->complete);

	return IRQ_HANDLED;
}

=======
	spin_lock(&dev->lock);
	interrupt_status = CRYPTO_READ(dev, RK_CRYPTO_INTSTS);
	CRYPTO_WRITE(dev, RK_CRYPTO_INTSTS, interrupt_status);

	if (interrupt_status & 0x0a) {
		dev_warn(dev->dev, "DMA Error\n");
		dev->err = -EFAULT;
	}
	tasklet_schedule(&dev->done_task);

	spin_unlock(&dev->lock);
	return IRQ_HANDLED;
}

static int rk_crypto_enqueue(struct rk_crypto_info *dev,
			      struct crypto_async_request *async_req)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&dev->lock, flags);
	ret = crypto_enqueue_request(&dev->queue, async_req);
	if (dev->busy) {
		spin_unlock_irqrestore(&dev->lock, flags);
		return ret;
	}
	dev->busy = true;
	spin_unlock_irqrestore(&dev->lock, flags);
	tasklet_schedule(&dev->queue_task);

	return ret;
}

static void rk_crypto_queue_task_cb(unsigned long data)
{
	struct rk_crypto_info *dev = (struct rk_crypto_info *)data;
	struct crypto_async_request *async_req, *backlog;
	unsigned long flags;
	int err = 0;

	dev->err = 0;
	spin_lock_irqsave(&dev->lock, flags);
	backlog   = crypto_get_backlog(&dev->queue);
	async_req = crypto_dequeue_request(&dev->queue);

	if (!async_req) {
		dev->busy = false;
		spin_unlock_irqrestore(&dev->lock, flags);
		return;
	}
	spin_unlock_irqrestore(&dev->lock, flags);

	if (backlog) {
		backlog->complete(backlog, -EINPROGRESS);
		backlog = NULL;
	}

	dev->async_req = async_req;
	err = dev->start(dev);
	if (err)
		dev->complete(dev->async_req, err);
}

static void rk_crypto_done_task_cb(unsigned long data)
{
	struct rk_crypto_info *dev = (struct rk_crypto_info *)data;

	if (dev->err) {
		dev->complete(dev->async_req, dev->err);
		return;
	}

	dev->err = dev->update(dev);
	if (dev->err)
		dev->complete(dev->async_req, dev->err);
}

>>>>>>> b7ba80a49124 (Commit)
static struct rk_crypto_tmp *rk_cipher_algs[] = {
	&rk_ecb_aes_alg,
	&rk_cbc_aes_alg,
	&rk_ecb_des_alg,
	&rk_cbc_des_alg,
	&rk_ecb_des3_ede_alg,
	&rk_cbc_des3_ede_alg,
	&rk_ahash_sha1,
	&rk_ahash_sha256,
	&rk_ahash_md5,
};

<<<<<<< HEAD
#ifdef CONFIG_CRYPTO_DEV_ROCKCHIP_DEBUG
static int rk_crypto_debugfs_show(struct seq_file *seq, void *v)
{
	struct rk_crypto_info *dd;
	unsigned int i;

	spin_lock(&rocklist.lock);
	list_for_each_entry(dd, &rocklist.dev_list, list) {
		seq_printf(seq, "%s %s requests: %lu\n",
			   dev_driver_string(dd->dev), dev_name(dd->dev),
			   dd->nreq);
	}
	spin_unlock(&rocklist.lock);

	for (i = 0; i < ARRAY_SIZE(rk_cipher_algs); i++) {
		if (!rk_cipher_algs[i]->dev)
			continue;
		switch (rk_cipher_algs[i]->type) {
		case CRYPTO_ALG_TYPE_SKCIPHER:
			seq_printf(seq, "%s %s reqs=%lu fallback=%lu\n",
				   rk_cipher_algs[i]->alg.skcipher.base.cra_driver_name,
				   rk_cipher_algs[i]->alg.skcipher.base.cra_name,
				   rk_cipher_algs[i]->stat_req, rk_cipher_algs[i]->stat_fb);
			seq_printf(seq, "\tfallback due to length: %lu\n",
				   rk_cipher_algs[i]->stat_fb_len);
			seq_printf(seq, "\tfallback due to alignment: %lu\n",
				   rk_cipher_algs[i]->stat_fb_align);
			seq_printf(seq, "\tfallback due to SGs: %lu\n",
				   rk_cipher_algs[i]->stat_fb_sgdiff);
			break;
		case CRYPTO_ALG_TYPE_AHASH:
			seq_printf(seq, "%s %s reqs=%lu fallback=%lu\n",
				   rk_cipher_algs[i]->alg.hash.halg.base.cra_driver_name,
				   rk_cipher_algs[i]->alg.hash.halg.base.cra_name,
				   rk_cipher_algs[i]->stat_req, rk_cipher_algs[i]->stat_fb);
			break;
		}
	}
	return 0;
}

DEFINE_SHOW_ATTRIBUTE(rk_crypto_debugfs);
#endif

static void register_debugfs(struct rk_crypto_info *crypto_info)
{
#ifdef CONFIG_CRYPTO_DEV_ROCKCHIP_DEBUG
	/* Ignore error of debugfs */
	rocklist.dbgfs_dir = debugfs_create_dir("rk3288_crypto", NULL);
	rocklist.dbgfs_stats = debugfs_create_file("stats", 0444,
						   rocklist.dbgfs_dir,
						   &rocklist,
						   &rk_crypto_debugfs_fops);
#endif
}

=======
>>>>>>> b7ba80a49124 (Commit)
static int rk_crypto_register(struct rk_crypto_info *crypto_info)
{
	unsigned int i, k;
	int err = 0;

	for (i = 0; i < ARRAY_SIZE(rk_cipher_algs); i++) {
		rk_cipher_algs[i]->dev = crypto_info;
<<<<<<< HEAD
		switch (rk_cipher_algs[i]->type) {
		case CRYPTO_ALG_TYPE_SKCIPHER:
			dev_info(crypto_info->dev, "Register %s as %s\n",
				 rk_cipher_algs[i]->alg.skcipher.base.cra_name,
				 rk_cipher_algs[i]->alg.skcipher.base.cra_driver_name);
			err = crypto_register_skcipher(&rk_cipher_algs[i]->alg.skcipher);
			break;
		case CRYPTO_ALG_TYPE_AHASH:
			dev_info(crypto_info->dev, "Register %s as %s\n",
				 rk_cipher_algs[i]->alg.hash.halg.base.cra_name,
				 rk_cipher_algs[i]->alg.hash.halg.base.cra_driver_name);
			err = crypto_register_ahash(&rk_cipher_algs[i]->alg.hash);
			break;
		default:
			dev_err(crypto_info->dev, "unknown algorithm\n");
		}
=======
		if (rk_cipher_algs[i]->type == ALG_TYPE_CIPHER)
			err = crypto_register_skcipher(
					&rk_cipher_algs[i]->alg.skcipher);
		else
			err = crypto_register_ahash(
					&rk_cipher_algs[i]->alg.hash);
>>>>>>> b7ba80a49124 (Commit)
		if (err)
			goto err_cipher_algs;
	}
	return 0;

err_cipher_algs:
	for (k = 0; k < i; k++) {
<<<<<<< HEAD
		if (rk_cipher_algs[i]->type == CRYPTO_ALG_TYPE_SKCIPHER)
=======
		if (rk_cipher_algs[i]->type == ALG_TYPE_CIPHER)
>>>>>>> b7ba80a49124 (Commit)
			crypto_unregister_skcipher(&rk_cipher_algs[k]->alg.skcipher);
		else
			crypto_unregister_ahash(&rk_cipher_algs[i]->alg.hash);
	}
	return err;
}

static void rk_crypto_unregister(void)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(rk_cipher_algs); i++) {
<<<<<<< HEAD
		if (rk_cipher_algs[i]->type == CRYPTO_ALG_TYPE_SKCIPHER)
=======
		if (rk_cipher_algs[i]->type == ALG_TYPE_CIPHER)
>>>>>>> b7ba80a49124 (Commit)
			crypto_unregister_skcipher(&rk_cipher_algs[i]->alg.skcipher);
		else
			crypto_unregister_ahash(&rk_cipher_algs[i]->alg.hash);
	}
}

<<<<<<< HEAD
static const struct of_device_id crypto_of_id_table[] = {
	{ .compatible = "rockchip,rk3288-crypto",
	  .data = &rk3288_variant,
	},
	{ .compatible = "rockchip,rk3328-crypto",
	  .data = &rk3328_variant,
	},
	{ .compatible = "rockchip,rk3399-crypto",
	  .data = &rk3399_variant,
	},
=======
static void rk_crypto_action(void *data)
{
	struct rk_crypto_info *crypto_info = data;

	reset_control_assert(crypto_info->rst);
}

static const struct of_device_id crypto_of_id_table[] = {
	{ .compatible = "rockchip,rk3288-crypto" },
>>>>>>> b7ba80a49124 (Commit)
	{}
};
MODULE_DEVICE_TABLE(of, crypto_of_id_table);

static int rk_crypto_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
<<<<<<< HEAD
	struct rk_crypto_info *crypto_info, *first;
=======
	struct rk_crypto_info *crypto_info;
>>>>>>> b7ba80a49124 (Commit)
	int err = 0;

	crypto_info = devm_kzalloc(&pdev->dev,
				   sizeof(*crypto_info), GFP_KERNEL);
	if (!crypto_info) {
		err = -ENOMEM;
		goto err_crypto;
	}

<<<<<<< HEAD
	crypto_info->dev = &pdev->dev;
	platform_set_drvdata(pdev, crypto_info);

	crypto_info->variant = of_device_get_match_data(&pdev->dev);
	if (!crypto_info->variant) {
		dev_err(&pdev->dev, "Missing variant\n");
		return -EINVAL;
	}

	crypto_info->rst = devm_reset_control_array_get_exclusive(dev);
=======
	crypto_info->rst = devm_reset_control_get(dev, "crypto-rst");
>>>>>>> b7ba80a49124 (Commit)
	if (IS_ERR(crypto_info->rst)) {
		err = PTR_ERR(crypto_info->rst);
		goto err_crypto;
	}

	reset_control_assert(crypto_info->rst);
	usleep_range(10, 20);
	reset_control_deassert(crypto_info->rst);

<<<<<<< HEAD
=======
	err = devm_add_action_or_reset(dev, rk_crypto_action, crypto_info);
	if (err)
		goto err_crypto;

	spin_lock_init(&crypto_info->lock);

>>>>>>> b7ba80a49124 (Commit)
	crypto_info->reg = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(crypto_info->reg)) {
		err = PTR_ERR(crypto_info->reg);
		goto err_crypto;
	}

<<<<<<< HEAD
	err = rk_crypto_get_clks(crypto_info);
	if (err)
		goto err_crypto;

	crypto_info->irq = platform_get_irq(pdev, 0);
	if (crypto_info->irq < 0) {
=======
	crypto_info->aclk = devm_clk_get(&pdev->dev, "aclk");
	if (IS_ERR(crypto_info->aclk)) {
		err = PTR_ERR(crypto_info->aclk);
		goto err_crypto;
	}

	crypto_info->hclk = devm_clk_get(&pdev->dev, "hclk");
	if (IS_ERR(crypto_info->hclk)) {
		err = PTR_ERR(crypto_info->hclk);
		goto err_crypto;
	}

	crypto_info->sclk = devm_clk_get(&pdev->dev, "sclk");
	if (IS_ERR(crypto_info->sclk)) {
		err = PTR_ERR(crypto_info->sclk);
		goto err_crypto;
	}

	crypto_info->dmaclk = devm_clk_get(&pdev->dev, "apb_pclk");
	if (IS_ERR(crypto_info->dmaclk)) {
		err = PTR_ERR(crypto_info->dmaclk);
		goto err_crypto;
	}

	crypto_info->irq = platform_get_irq(pdev, 0);
	if (crypto_info->irq < 0) {
		dev_warn(crypto_info->dev,
			 "control Interrupt is not available.\n");
>>>>>>> b7ba80a49124 (Commit)
		err = crypto_info->irq;
		goto err_crypto;
	}

	err = devm_request_irq(&pdev->dev, crypto_info->irq,
			       rk_crypto_irq_handle, IRQF_SHARED,
			       "rk-crypto", pdev);

	if (err) {
<<<<<<< HEAD
		dev_err(&pdev->dev, "irq request failed.\n");
		goto err_crypto;
	}

	crypto_info->engine = crypto_engine_alloc_init(&pdev->dev, true);
	crypto_engine_start(crypto_info->engine);
	init_completion(&crypto_info->complete);

	err = rk_crypto_pm_init(crypto_info);
	if (err)
		goto err_pm;

	spin_lock(&rocklist.lock);
	first = list_first_entry_or_null(&rocklist.dev_list,
					 struct rk_crypto_info, list);
	list_add_tail(&crypto_info->list, &rocklist.dev_list);
	spin_unlock(&rocklist.lock);

	if (!first) {
		err = rk_crypto_register(crypto_info);
		if (err) {
			dev_err(dev, "Fail to register crypto algorithms");
			goto err_register_alg;
		}

		register_debugfs(crypto_info);
	}

	return 0;

err_register_alg:
	rk_crypto_pm_exit(crypto_info);
err_pm:
	crypto_engine_exit(crypto_info->engine);
err_crypto:
	dev_err(dev, "Crypto Accelerator not successfully registered\n");
=======
		dev_err(crypto_info->dev, "irq request failed.\n");
		goto err_crypto;
	}

	crypto_info->dev = &pdev->dev;
	platform_set_drvdata(pdev, crypto_info);

	tasklet_init(&crypto_info->queue_task,
		     rk_crypto_queue_task_cb, (unsigned long)crypto_info);
	tasklet_init(&crypto_info->done_task,
		     rk_crypto_done_task_cb, (unsigned long)crypto_info);
	crypto_init_queue(&crypto_info->queue, 50);

	crypto_info->enable_clk = rk_crypto_enable_clk;
	crypto_info->disable_clk = rk_crypto_disable_clk;
	crypto_info->load_data = rk_load_data;
	crypto_info->unload_data = rk_unload_data;
	crypto_info->enqueue = rk_crypto_enqueue;
	crypto_info->busy = false;

	err = rk_crypto_register(crypto_info);
	if (err) {
		dev_err(dev, "err in register alg");
		goto err_register_alg;
	}

	dev_info(dev, "Crypto Accelerator successfully registered\n");
	return 0;

err_register_alg:
	tasklet_kill(&crypto_info->queue_task);
	tasklet_kill(&crypto_info->done_task);
err_crypto:
>>>>>>> b7ba80a49124 (Commit)
	return err;
}

static int rk_crypto_remove(struct platform_device *pdev)
{
	struct rk_crypto_info *crypto_tmp = platform_get_drvdata(pdev);
<<<<<<< HEAD
	struct rk_crypto_info *first;

	spin_lock_bh(&rocklist.lock);
	list_del(&crypto_tmp->list);
	first = list_first_entry_or_null(&rocklist.dev_list,
					 struct rk_crypto_info, list);
	spin_unlock_bh(&rocklist.lock);

	if (!first) {
#ifdef CONFIG_CRYPTO_DEV_ROCKCHIP_DEBUG
		debugfs_remove_recursive(rocklist.dbgfs_dir);
#endif
		rk_crypto_unregister();
	}
	rk_crypto_pm_exit(crypto_tmp);
	crypto_engine_exit(crypto_tmp->engine);
=======

	rk_crypto_unregister();
	tasklet_kill(&crypto_tmp->done_task);
	tasklet_kill(&crypto_tmp->queue_task);
>>>>>>> b7ba80a49124 (Commit)
	return 0;
}

static struct platform_driver crypto_driver = {
	.probe		= rk_crypto_probe,
	.remove		= rk_crypto_remove,
	.driver		= {
		.name	= "rk3288-crypto",
<<<<<<< HEAD
		.pm		= &rk_crypto_pm_ops,
=======
>>>>>>> b7ba80a49124 (Commit)
		.of_match_table	= crypto_of_id_table,
	},
};

module_platform_driver(crypto_driver);

MODULE_AUTHOR("Zain Wang <zain.wang@rock-chips.com>");
MODULE_DESCRIPTION("Support for Rockchip's cryptographic engine");
MODULE_LICENSE("GPL");
