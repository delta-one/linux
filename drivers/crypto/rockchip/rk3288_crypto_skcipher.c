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
#include <linux/device.h>
<<<<<<< HEAD
#include <crypto/scatterwalk.h>
=======
>>>>>>> b7ba80a49124 (Commit)
#include "rk3288_crypto.h"

#define RK_CRYPTO_DEC			BIT(0)

<<<<<<< HEAD
static int rk_cipher_need_fallback(struct skcipher_request *req)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct skcipher_alg *alg = crypto_skcipher_alg(tfm);
	struct rk_crypto_tmp *algt = container_of(alg, struct rk_crypto_tmp, alg.skcipher);
	struct scatterlist *sgs, *sgd;
	unsigned int stodo, dtodo, len;
	unsigned int bs = crypto_skcipher_blocksize(tfm);

	if (!req->cryptlen)
		return true;

	len = req->cryptlen;
	sgs = req->src;
	sgd = req->dst;
	while (sgs && sgd) {
		if (!IS_ALIGNED(sgs->offset, sizeof(u32))) {
			algt->stat_fb_align++;
			return true;
		}
		if (!IS_ALIGNED(sgd->offset, sizeof(u32))) {
			algt->stat_fb_align++;
			return true;
		}
		stodo = min(len, sgs->length);
		if (stodo % bs) {
			algt->stat_fb_len++;
			return true;
		}
		dtodo = min(len, sgd->length);
		if (dtodo % bs) {
			algt->stat_fb_len++;
			return true;
		}
		if (stodo != dtodo) {
			algt->stat_fb_sgdiff++;
			return true;
		}
		len -= stodo;
		sgs = sg_next(sgs);
		sgd = sg_next(sgd);
	}
	return false;
}

static int rk_cipher_fallback(struct skcipher_request *areq)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(areq);
	struct rk_cipher_ctx *op = crypto_skcipher_ctx(tfm);
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(areq);
	struct skcipher_alg *alg = crypto_skcipher_alg(tfm);
	struct rk_crypto_tmp *algt = container_of(alg, struct rk_crypto_tmp, alg.skcipher);
	int err;

	algt->stat_fb++;

	skcipher_request_set_tfm(&rctx->fallback_req, op->fallback_tfm);
	skcipher_request_set_callback(&rctx->fallback_req, areq->base.flags,
				      areq->base.complete, areq->base.data);
	skcipher_request_set_crypt(&rctx->fallback_req, areq->src, areq->dst,
				   areq->cryptlen, areq->iv);
	if (rctx->mode & RK_CRYPTO_DEC)
		err = crypto_skcipher_decrypt(&rctx->fallback_req);
	else
		err = crypto_skcipher_encrypt(&rctx->fallback_req);
	return err;
}

static int rk_cipher_handle_req(struct skcipher_request *req)
{
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(req);
	struct rk_crypto_info *rkc;
	struct crypto_engine *engine;

	if (rk_cipher_need_fallback(req))
		return rk_cipher_fallback(req);

	rkc = get_rk_crypto();

	engine = rkc->engine;
	rctx->dev = rkc;

	return crypto_transfer_skcipher_request_to_engine(engine, req);
=======
static void rk_crypto_complete(struct crypto_async_request *base, int err)
{
	if (base->complete)
		base->complete(base, err);
}

static int rk_handle_req(struct rk_crypto_info *dev,
			 struct skcipher_request *req)
{
	if (!IS_ALIGNED(req->cryptlen, dev->align_size))
		return -EINVAL;
	else
		return dev->enqueue(dev, &req->base);
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_aes_setkey(struct crypto_skcipher *cipher,
			 const u8 *key, unsigned int keylen)
{
	struct crypto_tfm *tfm = crypto_skcipher_tfm(cipher);
	struct rk_cipher_ctx *ctx = crypto_tfm_ctx(tfm);

	if (keylen != AES_KEYSIZE_128 && keylen != AES_KEYSIZE_192 &&
	    keylen != AES_KEYSIZE_256)
		return -EINVAL;
	ctx->keylen = keylen;
<<<<<<< HEAD
	memcpy(ctx->key, key, keylen);

	return crypto_skcipher_setkey(ctx->fallback_tfm, key, keylen);
=======
	memcpy_toio(ctx->dev->reg + RK_CRYPTO_AES_KEY_0, key, keylen);
	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_des_setkey(struct crypto_skcipher *cipher,
			 const u8 *key, unsigned int keylen)
{
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(cipher);
	int err;

	err = verify_skcipher_des_key(cipher, key);
	if (err)
		return err;

	ctx->keylen = keylen;
<<<<<<< HEAD
	memcpy(ctx->key, key, keylen);

	return crypto_skcipher_setkey(ctx->fallback_tfm, key, keylen);
=======
	memcpy_toio(ctx->dev->reg + RK_CRYPTO_TDES_KEY1_0, key, keylen);
	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_tdes_setkey(struct crypto_skcipher *cipher,
			  const u8 *key, unsigned int keylen)
{
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(cipher);
	int err;

	err = verify_skcipher_des3_key(cipher, key);
	if (err)
		return err;

	ctx->keylen = keylen;
<<<<<<< HEAD
	memcpy(ctx->key, key, keylen);

	return crypto_skcipher_setkey(ctx->fallback_tfm, key, keylen);
=======
	memcpy_toio(ctx->dev->reg + RK_CRYPTO_TDES_KEY1_0, key, keylen);
	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_aes_ecb_encrypt(struct skcipher_request *req)
{
<<<<<<< HEAD
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(req);

	rctx->mode = RK_CRYPTO_AES_ECB_MODE;
	return rk_cipher_handle_req(req);
=======
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct rk_crypto_info *dev = ctx->dev;

	ctx->mode = RK_CRYPTO_AES_ECB_MODE;
	return rk_handle_req(dev, req);
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_aes_ecb_decrypt(struct skcipher_request *req)
{
<<<<<<< HEAD
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(req);

	rctx->mode = RK_CRYPTO_AES_ECB_MODE | RK_CRYPTO_DEC;
	return rk_cipher_handle_req(req);
=======
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct rk_crypto_info *dev = ctx->dev;

	ctx->mode = RK_CRYPTO_AES_ECB_MODE | RK_CRYPTO_DEC;
	return rk_handle_req(dev, req);
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_aes_cbc_encrypt(struct skcipher_request *req)
{
<<<<<<< HEAD
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(req);

	rctx->mode = RK_CRYPTO_AES_CBC_MODE;
	return rk_cipher_handle_req(req);
=======
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct rk_crypto_info *dev = ctx->dev;

	ctx->mode = RK_CRYPTO_AES_CBC_MODE;
	return rk_handle_req(dev, req);
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_aes_cbc_decrypt(struct skcipher_request *req)
{
<<<<<<< HEAD
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(req);

	rctx->mode = RK_CRYPTO_AES_CBC_MODE | RK_CRYPTO_DEC;
	return rk_cipher_handle_req(req);
=======
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct rk_crypto_info *dev = ctx->dev;

	ctx->mode = RK_CRYPTO_AES_CBC_MODE | RK_CRYPTO_DEC;
	return rk_handle_req(dev, req);
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_des_ecb_encrypt(struct skcipher_request *req)
{
<<<<<<< HEAD
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(req);

	rctx->mode = 0;
	return rk_cipher_handle_req(req);
=======
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct rk_crypto_info *dev = ctx->dev;

	ctx->mode = 0;
	return rk_handle_req(dev, req);
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_des_ecb_decrypt(struct skcipher_request *req)
{
<<<<<<< HEAD
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(req);

	rctx->mode = RK_CRYPTO_DEC;
	return rk_cipher_handle_req(req);
=======
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct rk_crypto_info *dev = ctx->dev;

	ctx->mode = RK_CRYPTO_DEC;
	return rk_handle_req(dev, req);
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_des_cbc_encrypt(struct skcipher_request *req)
{
<<<<<<< HEAD
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(req);

	rctx->mode = RK_CRYPTO_TDES_CHAINMODE_CBC;
	return rk_cipher_handle_req(req);
=======
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct rk_crypto_info *dev = ctx->dev;

	ctx->mode = RK_CRYPTO_TDES_CHAINMODE_CBC;
	return rk_handle_req(dev, req);
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_des_cbc_decrypt(struct skcipher_request *req)
{
<<<<<<< HEAD
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(req);

	rctx->mode = RK_CRYPTO_TDES_CHAINMODE_CBC | RK_CRYPTO_DEC;
	return rk_cipher_handle_req(req);
=======
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct rk_crypto_info *dev = ctx->dev;

	ctx->mode = RK_CRYPTO_TDES_CHAINMODE_CBC | RK_CRYPTO_DEC;
	return rk_handle_req(dev, req);
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_des3_ede_ecb_encrypt(struct skcipher_request *req)
{
<<<<<<< HEAD
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(req);

	rctx->mode = RK_CRYPTO_TDES_SELECT;
	return rk_cipher_handle_req(req);
=======
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct rk_crypto_info *dev = ctx->dev;

	ctx->mode = RK_CRYPTO_TDES_SELECT;
	return rk_handle_req(dev, req);
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_des3_ede_ecb_decrypt(struct skcipher_request *req)
{
<<<<<<< HEAD
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(req);

	rctx->mode = RK_CRYPTO_TDES_SELECT | RK_CRYPTO_DEC;
	return rk_cipher_handle_req(req);
=======
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct rk_crypto_info *dev = ctx->dev;

	ctx->mode = RK_CRYPTO_TDES_SELECT | RK_CRYPTO_DEC;
	return rk_handle_req(dev, req);
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_des3_ede_cbc_encrypt(struct skcipher_request *req)
{
<<<<<<< HEAD
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(req);

	rctx->mode = RK_CRYPTO_TDES_SELECT | RK_CRYPTO_TDES_CHAINMODE_CBC;
	return rk_cipher_handle_req(req);
=======
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct rk_crypto_info *dev = ctx->dev;

	ctx->mode = RK_CRYPTO_TDES_SELECT | RK_CRYPTO_TDES_CHAINMODE_CBC;
	return rk_handle_req(dev, req);
>>>>>>> b7ba80a49124 (Commit)
}

static int rk_des3_ede_cbc_decrypt(struct skcipher_request *req)
{
<<<<<<< HEAD
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(req);

	rctx->mode = RK_CRYPTO_TDES_SELECT | RK_CRYPTO_TDES_CHAINMODE_CBC |
		    RK_CRYPTO_DEC;
	return rk_cipher_handle_req(req);
}

static void rk_cipher_hw_init(struct rk_crypto_info *dev, struct skcipher_request *req)
{
	struct crypto_skcipher *cipher = crypto_skcipher_reqtfm(req);
	struct crypto_tfm *tfm = crypto_skcipher_tfm(cipher);
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(cipher);
	u32 block, conf_reg = 0;

	block = crypto_tfm_alg_blocksize(tfm);

	if (block == DES_BLOCK_SIZE) {
		rctx->mode |= RK_CRYPTO_TDES_FIFO_MODE |
			     RK_CRYPTO_TDES_BYTESWAP_KEY |
			     RK_CRYPTO_TDES_BYTESWAP_IV;
		CRYPTO_WRITE(dev, RK_CRYPTO_TDES_CTRL, rctx->mode);
		memcpy_toio(dev->reg + RK_CRYPTO_TDES_KEY1_0, ctx->key, ctx->keylen);
		conf_reg = RK_CRYPTO_DESSEL;
	} else {
		rctx->mode |= RK_CRYPTO_AES_FIFO_MODE |
=======
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct rk_crypto_info *dev = ctx->dev;

	ctx->mode = RK_CRYPTO_TDES_SELECT | RK_CRYPTO_TDES_CHAINMODE_CBC |
		    RK_CRYPTO_DEC;
	return rk_handle_req(dev, req);
}

static void rk_ablk_hw_init(struct rk_crypto_info *dev)
{
	struct skcipher_request *req =
		skcipher_request_cast(dev->async_req);
	struct crypto_skcipher *cipher = crypto_skcipher_reqtfm(req);
	struct crypto_tfm *tfm = crypto_skcipher_tfm(cipher);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(cipher);
	u32 ivsize, block, conf_reg = 0;

	block = crypto_tfm_alg_blocksize(tfm);
	ivsize = crypto_skcipher_ivsize(cipher);

	if (block == DES_BLOCK_SIZE) {
		ctx->mode |= RK_CRYPTO_TDES_FIFO_MODE |
			     RK_CRYPTO_TDES_BYTESWAP_KEY |
			     RK_CRYPTO_TDES_BYTESWAP_IV;
		CRYPTO_WRITE(dev, RK_CRYPTO_TDES_CTRL, ctx->mode);
		memcpy_toio(dev->reg + RK_CRYPTO_TDES_IV_0, req->iv, ivsize);
		conf_reg = RK_CRYPTO_DESSEL;
	} else {
		ctx->mode |= RK_CRYPTO_AES_FIFO_MODE |
>>>>>>> b7ba80a49124 (Commit)
			     RK_CRYPTO_AES_KEY_CHANGE |
			     RK_CRYPTO_AES_BYTESWAP_KEY |
			     RK_CRYPTO_AES_BYTESWAP_IV;
		if (ctx->keylen == AES_KEYSIZE_192)
<<<<<<< HEAD
			rctx->mode |= RK_CRYPTO_AES_192BIT_key;
		else if (ctx->keylen == AES_KEYSIZE_256)
			rctx->mode |= RK_CRYPTO_AES_256BIT_key;
		CRYPTO_WRITE(dev, RK_CRYPTO_AES_CTRL, rctx->mode);
		memcpy_toio(dev->reg + RK_CRYPTO_AES_KEY_0, ctx->key, ctx->keylen);
=======
			ctx->mode |= RK_CRYPTO_AES_192BIT_key;
		else if (ctx->keylen == AES_KEYSIZE_256)
			ctx->mode |= RK_CRYPTO_AES_256BIT_key;
		CRYPTO_WRITE(dev, RK_CRYPTO_AES_CTRL, ctx->mode);
		memcpy_toio(dev->reg + RK_CRYPTO_AES_IV_0, req->iv, ivsize);
>>>>>>> b7ba80a49124 (Commit)
	}
	conf_reg |= RK_CRYPTO_BYTESWAP_BTFIFO |
		    RK_CRYPTO_BYTESWAP_BRFIFO;
	CRYPTO_WRITE(dev, RK_CRYPTO_CONF, conf_reg);
	CRYPTO_WRITE(dev, RK_CRYPTO_INTENA,
		     RK_CRYPTO_BCDMA_ERR_ENA | RK_CRYPTO_BCDMA_DONE_ENA);
}

<<<<<<< HEAD
static void crypto_dma_start(struct rk_crypto_info *dev,
			     struct scatterlist *sgs,
			     struct scatterlist *sgd, unsigned int todo)
{
	CRYPTO_WRITE(dev, RK_CRYPTO_BRDMAS, sg_dma_address(sgs));
	CRYPTO_WRITE(dev, RK_CRYPTO_BRDMAL, todo);
	CRYPTO_WRITE(dev, RK_CRYPTO_BTDMAS, sg_dma_address(sgd));
=======
static void crypto_dma_start(struct rk_crypto_info *dev)
{
	CRYPTO_WRITE(dev, RK_CRYPTO_BRDMAS, dev->addr_in);
	CRYPTO_WRITE(dev, RK_CRYPTO_BRDMAL, dev->count / 4);
	CRYPTO_WRITE(dev, RK_CRYPTO_BTDMAS, dev->addr_out);
>>>>>>> b7ba80a49124 (Commit)
	CRYPTO_WRITE(dev, RK_CRYPTO_CTRL, RK_CRYPTO_BLOCK_START |
		     _SBF(RK_CRYPTO_BLOCK_START, 16));
}

<<<<<<< HEAD
static int rk_cipher_run(struct crypto_engine *engine, void *async_req)
{
	struct skcipher_request *areq = container_of(async_req, struct skcipher_request, base);
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(areq);
	struct rk_cipher_rctx *rctx = skcipher_request_ctx(areq);
	struct scatterlist *sgs, *sgd;
	int err = 0;
	int ivsize = crypto_skcipher_ivsize(tfm);
	int offset;
	u8 iv[AES_BLOCK_SIZE];
	u8 biv[AES_BLOCK_SIZE];
	u8 *ivtouse = areq->iv;
	unsigned int len = areq->cryptlen;
	unsigned int todo;
	struct skcipher_alg *alg = crypto_skcipher_alg(tfm);
	struct rk_crypto_tmp *algt = container_of(alg, struct rk_crypto_tmp, alg.skcipher);
	struct rk_crypto_info *rkc = rctx->dev;

	err = pm_runtime_resume_and_get(rkc->dev);
	if (err)
		return err;

	algt->stat_req++;
	rkc->nreq++;

	ivsize = crypto_skcipher_ivsize(tfm);
	if (areq->iv && crypto_skcipher_ivsize(tfm) > 0) {
		if (rctx->mode & RK_CRYPTO_DEC) {
			offset = areq->cryptlen - ivsize;
			scatterwalk_map_and_copy(rctx->backup_iv, areq->src,
						 offset, ivsize, 0);
		}
	}

	sgs = areq->src;
	sgd = areq->dst;

	while (sgs && sgd && len) {
		if (!sgs->length) {
			sgs = sg_next(sgs);
			sgd = sg_next(sgd);
			continue;
		}
		if (rctx->mode & RK_CRYPTO_DEC) {
			/* we backup last block of source to be used as IV at next step */
			offset = sgs->length - ivsize;
			scatterwalk_map_and_copy(biv, sgs, offset, ivsize, 0);
		}
		if (sgs == sgd) {
			err = dma_map_sg(rkc->dev, sgs, 1, DMA_BIDIRECTIONAL);
			if (err <= 0) {
				err = -EINVAL;
				goto theend_iv;
			}
		} else {
			err = dma_map_sg(rkc->dev, sgs, 1, DMA_TO_DEVICE);
			if (err <= 0) {
				err = -EINVAL;
				goto theend_iv;
			}
			err = dma_map_sg(rkc->dev, sgd, 1, DMA_FROM_DEVICE);
			if (err <= 0) {
				err = -EINVAL;
				goto theend_sgs;
			}
		}
		err = 0;
		rk_cipher_hw_init(rkc, areq);
		if (ivsize) {
			if (ivsize == DES_BLOCK_SIZE)
				memcpy_toio(rkc->reg + RK_CRYPTO_TDES_IV_0, ivtouse, ivsize);
			else
				memcpy_toio(rkc->reg + RK_CRYPTO_AES_IV_0, ivtouse, ivsize);
		}
		reinit_completion(&rkc->complete);
		rkc->status = 0;

		todo = min(sg_dma_len(sgs), len);
		len -= todo;
		crypto_dma_start(rkc, sgs, sgd, todo / 4);
		wait_for_completion_interruptible_timeout(&rkc->complete,
							  msecs_to_jiffies(2000));
		if (!rkc->status) {
			dev_err(rkc->dev, "DMA timeout\n");
			err = -EFAULT;
			goto theend;
		}
		if (sgs == sgd) {
			dma_unmap_sg(rkc->dev, sgs, 1, DMA_BIDIRECTIONAL);
		} else {
			dma_unmap_sg(rkc->dev, sgs, 1, DMA_TO_DEVICE);
			dma_unmap_sg(rkc->dev, sgd, 1, DMA_FROM_DEVICE);
		}
		if (rctx->mode & RK_CRYPTO_DEC) {
			memcpy(iv, biv, ivsize);
			ivtouse = iv;
		} else {
			offset = sgd->length - ivsize;
			scatterwalk_map_and_copy(iv, sgd, offset, ivsize, 0);
			ivtouse = iv;
		}
		sgs = sg_next(sgs);
		sgd = sg_next(sgd);
	}

	if (areq->iv && ivsize > 0) {
		offset = areq->cryptlen - ivsize;
		if (rctx->mode & RK_CRYPTO_DEC) {
			memcpy(areq->iv, rctx->backup_iv, ivsize);
			memzero_explicit(rctx->backup_iv, ivsize);
		} else {
			scatterwalk_map_and_copy(areq->iv, areq->dst, offset,
						 ivsize, 0);
		}
	}

theend:
	pm_runtime_put_autosuspend(rkc->dev);

	local_bh_disable();
	crypto_finalize_skcipher_request(engine, areq, err);
	local_bh_enable();
	return 0;

theend_sgs:
	if (sgs == sgd) {
		dma_unmap_sg(rkc->dev, sgs, 1, DMA_BIDIRECTIONAL);
	} else {
		dma_unmap_sg(rkc->dev, sgs, 1, DMA_TO_DEVICE);
		dma_unmap_sg(rkc->dev, sgd, 1, DMA_FROM_DEVICE);
	}
theend_iv:
	return err;
}

static int rk_cipher_tfm_init(struct crypto_skcipher *tfm)
{
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	const char *name = crypto_tfm_alg_name(&tfm->base);
	struct skcipher_alg *alg = crypto_skcipher_alg(tfm);
	struct rk_crypto_tmp *algt = container_of(alg, struct rk_crypto_tmp, alg.skcipher);

	ctx->fallback_tfm = crypto_alloc_skcipher(name, 0, CRYPTO_ALG_NEED_FALLBACK);
	if (IS_ERR(ctx->fallback_tfm)) {
		dev_err(algt->dev->dev, "ERROR: Cannot allocate fallback for %s %ld\n",
			name, PTR_ERR(ctx->fallback_tfm));
		return PTR_ERR(ctx->fallback_tfm);
	}

	tfm->reqsize = sizeof(struct rk_cipher_rctx) +
		crypto_skcipher_reqsize(ctx->fallback_tfm);

	ctx->enginectx.op.do_one_request = rk_cipher_run;

	return 0;
}

static void rk_cipher_tfm_exit(struct crypto_skcipher *tfm)
{
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);

	memzero_explicit(ctx->key, ctx->keylen);
	crypto_free_skcipher(ctx->fallback_tfm);
}

struct rk_crypto_tmp rk_ecb_aes_alg = {
	.type = CRYPTO_ALG_TYPE_SKCIPHER,
=======
static int rk_set_data_start(struct rk_crypto_info *dev)
{
	int err;
	struct skcipher_request *req =
		skcipher_request_cast(dev->async_req);
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	u32 ivsize = crypto_skcipher_ivsize(tfm);
	u8 *src_last_blk = page_address(sg_page(dev->sg_src)) +
		dev->sg_src->offset + dev->sg_src->length - ivsize;

	/* Store the iv that need to be updated in chain mode.
	 * And update the IV buffer to contain the next IV for decryption mode.
	 */
	if (ctx->mode & RK_CRYPTO_DEC) {
		memcpy(ctx->iv, src_last_blk, ivsize);
		sg_pcopy_to_buffer(dev->first, dev->src_nents, req->iv,
				   ivsize, dev->total - ivsize);
	}

	err = dev->load_data(dev, dev->sg_src, dev->sg_dst);
	if (!err)
		crypto_dma_start(dev);
	return err;
}

static int rk_ablk_start(struct rk_crypto_info *dev)
{
	struct skcipher_request *req =
		skcipher_request_cast(dev->async_req);
	unsigned long flags;
	int err = 0;

	dev->left_bytes = req->cryptlen;
	dev->total = req->cryptlen;
	dev->sg_src = req->src;
	dev->first = req->src;
	dev->src_nents = sg_nents(req->src);
	dev->sg_dst = req->dst;
	dev->dst_nents = sg_nents(req->dst);
	dev->aligned = 1;

	spin_lock_irqsave(&dev->lock, flags);
	rk_ablk_hw_init(dev);
	err = rk_set_data_start(dev);
	spin_unlock_irqrestore(&dev->lock, flags);
	return err;
}

static void rk_iv_copyback(struct rk_crypto_info *dev)
{
	struct skcipher_request *req =
		skcipher_request_cast(dev->async_req);
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	u32 ivsize = crypto_skcipher_ivsize(tfm);

	/* Update the IV buffer to contain the next IV for encryption mode. */
	if (!(ctx->mode & RK_CRYPTO_DEC)) {
		if (dev->aligned) {
			memcpy(req->iv, sg_virt(dev->sg_dst) +
				dev->sg_dst->length - ivsize, ivsize);
		} else {
			memcpy(req->iv, dev->addr_vir +
				dev->count - ivsize, ivsize);
		}
	}
}

static void rk_update_iv(struct rk_crypto_info *dev)
{
	struct skcipher_request *req =
		skcipher_request_cast(dev->async_req);
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	u32 ivsize = crypto_skcipher_ivsize(tfm);
	u8 *new_iv = NULL;

	if (ctx->mode & RK_CRYPTO_DEC) {
		new_iv = ctx->iv;
	} else {
		new_iv = page_address(sg_page(dev->sg_dst)) +
			 dev->sg_dst->offset + dev->sg_dst->length - ivsize;
	}

	if (ivsize == DES_BLOCK_SIZE)
		memcpy_toio(dev->reg + RK_CRYPTO_TDES_IV_0, new_iv, ivsize);
	else if (ivsize == AES_BLOCK_SIZE)
		memcpy_toio(dev->reg + RK_CRYPTO_AES_IV_0, new_iv, ivsize);
}

/* return:
 *	true	some err was occurred
 *	fault	no err, continue
 */
static int rk_ablk_rx(struct rk_crypto_info *dev)
{
	int err = 0;
	struct skcipher_request *req =
		skcipher_request_cast(dev->async_req);

	dev->unload_data(dev);
	if (!dev->aligned) {
		if (!sg_pcopy_from_buffer(req->dst, dev->dst_nents,
					  dev->addr_vir, dev->count,
					  dev->total - dev->left_bytes -
					  dev->count)) {
			err = -EINVAL;
			goto out_rx;
		}
	}
	if (dev->left_bytes) {
		rk_update_iv(dev);
		if (dev->aligned) {
			if (sg_is_last(dev->sg_src)) {
				dev_err(dev->dev, "[%s:%d] Lack of data\n",
					__func__, __LINE__);
				err = -ENOMEM;
				goto out_rx;
			}
			dev->sg_src = sg_next(dev->sg_src);
			dev->sg_dst = sg_next(dev->sg_dst);
		}
		err = rk_set_data_start(dev);
	} else {
		rk_iv_copyback(dev);
		/* here show the calculation is over without any err */
		dev->complete(dev->async_req, 0);
		tasklet_schedule(&dev->queue_task);
	}
out_rx:
	return err;
}

static int rk_ablk_init_tfm(struct crypto_skcipher *tfm)
{
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct skcipher_alg *alg = crypto_skcipher_alg(tfm);
	struct rk_crypto_tmp *algt;

	algt = container_of(alg, struct rk_crypto_tmp, alg.skcipher);

	ctx->dev = algt->dev;
	ctx->dev->align_size = crypto_tfm_alg_alignmask(crypto_skcipher_tfm(tfm)) + 1;
	ctx->dev->start = rk_ablk_start;
	ctx->dev->update = rk_ablk_rx;
	ctx->dev->complete = rk_crypto_complete;
	ctx->dev->addr_vir = (char *)__get_free_page(GFP_KERNEL);

	return ctx->dev->addr_vir ? ctx->dev->enable_clk(ctx->dev) : -ENOMEM;
}

static void rk_ablk_exit_tfm(struct crypto_skcipher *tfm)
{
	struct rk_cipher_ctx *ctx = crypto_skcipher_ctx(tfm);

	free_page((unsigned long)ctx->dev->addr_vir);
	ctx->dev->disable_clk(ctx->dev);
}

struct rk_crypto_tmp rk_ecb_aes_alg = {
	.type = ALG_TYPE_CIPHER,
>>>>>>> b7ba80a49124 (Commit)
	.alg.skcipher = {
		.base.cra_name		= "ecb(aes)",
		.base.cra_driver_name	= "ecb-aes-rk",
		.base.cra_priority	= 300,
<<<<<<< HEAD
		.base.cra_flags		= CRYPTO_ALG_ASYNC | CRYPTO_ALG_NEED_FALLBACK,
=======
		.base.cra_flags		= CRYPTO_ALG_ASYNC,
>>>>>>> b7ba80a49124 (Commit)
		.base.cra_blocksize	= AES_BLOCK_SIZE,
		.base.cra_ctxsize	= sizeof(struct rk_cipher_ctx),
		.base.cra_alignmask	= 0x0f,
		.base.cra_module	= THIS_MODULE,

<<<<<<< HEAD
		.init			= rk_cipher_tfm_init,
		.exit			= rk_cipher_tfm_exit,
=======
		.init			= rk_ablk_init_tfm,
		.exit			= rk_ablk_exit_tfm,
>>>>>>> b7ba80a49124 (Commit)
		.min_keysize		= AES_MIN_KEY_SIZE,
		.max_keysize		= AES_MAX_KEY_SIZE,
		.setkey			= rk_aes_setkey,
		.encrypt		= rk_aes_ecb_encrypt,
		.decrypt		= rk_aes_ecb_decrypt,
	}
};

struct rk_crypto_tmp rk_cbc_aes_alg = {
<<<<<<< HEAD
	.type = CRYPTO_ALG_TYPE_SKCIPHER,
=======
	.type = ALG_TYPE_CIPHER,
>>>>>>> b7ba80a49124 (Commit)
	.alg.skcipher = {
		.base.cra_name		= "cbc(aes)",
		.base.cra_driver_name	= "cbc-aes-rk",
		.base.cra_priority	= 300,
<<<<<<< HEAD
		.base.cra_flags		= CRYPTO_ALG_ASYNC | CRYPTO_ALG_NEED_FALLBACK,
=======
		.base.cra_flags		= CRYPTO_ALG_ASYNC,
>>>>>>> b7ba80a49124 (Commit)
		.base.cra_blocksize	= AES_BLOCK_SIZE,
		.base.cra_ctxsize	= sizeof(struct rk_cipher_ctx),
		.base.cra_alignmask	= 0x0f,
		.base.cra_module	= THIS_MODULE,

<<<<<<< HEAD
		.init			= rk_cipher_tfm_init,
		.exit			= rk_cipher_tfm_exit,
=======
		.init			= rk_ablk_init_tfm,
		.exit			= rk_ablk_exit_tfm,
>>>>>>> b7ba80a49124 (Commit)
		.min_keysize		= AES_MIN_KEY_SIZE,
		.max_keysize		= AES_MAX_KEY_SIZE,
		.ivsize			= AES_BLOCK_SIZE,
		.setkey			= rk_aes_setkey,
		.encrypt		= rk_aes_cbc_encrypt,
		.decrypt		= rk_aes_cbc_decrypt,
	}
};

struct rk_crypto_tmp rk_ecb_des_alg = {
<<<<<<< HEAD
	.type = CRYPTO_ALG_TYPE_SKCIPHER,
=======
	.type = ALG_TYPE_CIPHER,
>>>>>>> b7ba80a49124 (Commit)
	.alg.skcipher = {
		.base.cra_name		= "ecb(des)",
		.base.cra_driver_name	= "ecb-des-rk",
		.base.cra_priority	= 300,
<<<<<<< HEAD
		.base.cra_flags		= CRYPTO_ALG_ASYNC | CRYPTO_ALG_NEED_FALLBACK,
=======
		.base.cra_flags		= CRYPTO_ALG_ASYNC,
>>>>>>> b7ba80a49124 (Commit)
		.base.cra_blocksize	= DES_BLOCK_SIZE,
		.base.cra_ctxsize	= sizeof(struct rk_cipher_ctx),
		.base.cra_alignmask	= 0x07,
		.base.cra_module	= THIS_MODULE,

<<<<<<< HEAD
		.init			= rk_cipher_tfm_init,
		.exit			= rk_cipher_tfm_exit,
=======
		.init			= rk_ablk_init_tfm,
		.exit			= rk_ablk_exit_tfm,
>>>>>>> b7ba80a49124 (Commit)
		.min_keysize		= DES_KEY_SIZE,
		.max_keysize		= DES_KEY_SIZE,
		.setkey			= rk_des_setkey,
		.encrypt		= rk_des_ecb_encrypt,
		.decrypt		= rk_des_ecb_decrypt,
	}
};

struct rk_crypto_tmp rk_cbc_des_alg = {
<<<<<<< HEAD
	.type = CRYPTO_ALG_TYPE_SKCIPHER,
=======
	.type = ALG_TYPE_CIPHER,
>>>>>>> b7ba80a49124 (Commit)
	.alg.skcipher = {
		.base.cra_name		= "cbc(des)",
		.base.cra_driver_name	= "cbc-des-rk",
		.base.cra_priority	= 300,
<<<<<<< HEAD
		.base.cra_flags		= CRYPTO_ALG_ASYNC | CRYPTO_ALG_NEED_FALLBACK,
=======
		.base.cra_flags		= CRYPTO_ALG_ASYNC,
>>>>>>> b7ba80a49124 (Commit)
		.base.cra_blocksize	= DES_BLOCK_SIZE,
		.base.cra_ctxsize	= sizeof(struct rk_cipher_ctx),
		.base.cra_alignmask	= 0x07,
		.base.cra_module	= THIS_MODULE,

<<<<<<< HEAD
		.init			= rk_cipher_tfm_init,
		.exit			= rk_cipher_tfm_exit,
=======
		.init			= rk_ablk_init_tfm,
		.exit			= rk_ablk_exit_tfm,
>>>>>>> b7ba80a49124 (Commit)
		.min_keysize		= DES_KEY_SIZE,
		.max_keysize		= DES_KEY_SIZE,
		.ivsize			= DES_BLOCK_SIZE,
		.setkey			= rk_des_setkey,
		.encrypt		= rk_des_cbc_encrypt,
		.decrypt		= rk_des_cbc_decrypt,
	}
};

struct rk_crypto_tmp rk_ecb_des3_ede_alg = {
<<<<<<< HEAD
	.type = CRYPTO_ALG_TYPE_SKCIPHER,
=======
	.type = ALG_TYPE_CIPHER,
>>>>>>> b7ba80a49124 (Commit)
	.alg.skcipher = {
		.base.cra_name		= "ecb(des3_ede)",
		.base.cra_driver_name	= "ecb-des3-ede-rk",
		.base.cra_priority	= 300,
<<<<<<< HEAD
		.base.cra_flags		= CRYPTO_ALG_ASYNC | CRYPTO_ALG_NEED_FALLBACK,
=======
		.base.cra_flags		= CRYPTO_ALG_ASYNC,
>>>>>>> b7ba80a49124 (Commit)
		.base.cra_blocksize	= DES_BLOCK_SIZE,
		.base.cra_ctxsize	= sizeof(struct rk_cipher_ctx),
		.base.cra_alignmask	= 0x07,
		.base.cra_module	= THIS_MODULE,

<<<<<<< HEAD
		.init			= rk_cipher_tfm_init,
		.exit			= rk_cipher_tfm_exit,
=======
		.init			= rk_ablk_init_tfm,
		.exit			= rk_ablk_exit_tfm,
>>>>>>> b7ba80a49124 (Commit)
		.min_keysize		= DES3_EDE_KEY_SIZE,
		.max_keysize		= DES3_EDE_KEY_SIZE,
		.setkey			= rk_tdes_setkey,
		.encrypt		= rk_des3_ede_ecb_encrypt,
		.decrypt		= rk_des3_ede_ecb_decrypt,
	}
};

struct rk_crypto_tmp rk_cbc_des3_ede_alg = {
<<<<<<< HEAD
	.type = CRYPTO_ALG_TYPE_SKCIPHER,
=======
	.type = ALG_TYPE_CIPHER,
>>>>>>> b7ba80a49124 (Commit)
	.alg.skcipher = {
		.base.cra_name		= "cbc(des3_ede)",
		.base.cra_driver_name	= "cbc-des3-ede-rk",
		.base.cra_priority	= 300,
<<<<<<< HEAD
		.base.cra_flags		= CRYPTO_ALG_ASYNC | CRYPTO_ALG_NEED_FALLBACK,
=======
		.base.cra_flags		= CRYPTO_ALG_ASYNC,
>>>>>>> b7ba80a49124 (Commit)
		.base.cra_blocksize	= DES_BLOCK_SIZE,
		.base.cra_ctxsize	= sizeof(struct rk_cipher_ctx),
		.base.cra_alignmask	= 0x07,
		.base.cra_module	= THIS_MODULE,

<<<<<<< HEAD
		.init			= rk_cipher_tfm_init,
		.exit			= rk_cipher_tfm_exit,
=======
		.init			= rk_ablk_init_tfm,
		.exit			= rk_ablk_exit_tfm,
>>>>>>> b7ba80a49124 (Commit)
		.min_keysize		= DES3_EDE_KEY_SIZE,
		.max_keysize		= DES3_EDE_KEY_SIZE,
		.ivsize			= DES_BLOCK_SIZE,
		.setkey			= rk_tdes_setkey,
		.encrypt		= rk_des3_ede_cbc_encrypt,
		.decrypt		= rk_des3_ede_cbc_decrypt,
	}
};
