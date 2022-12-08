// SPDX-License-Identifier: GPL-2.0+
/*
 * AMD ALSA SoC Pink Sardine PDM Driver
 *
 * Copyright 2022 Advanced Micro Devices, Inc.
 */

#include <linux/platform_device.h>
#include <linux/module.h>
<<<<<<< HEAD
#include <linux/bitfield.h>
=======
>>>>>>> b7ba80a49124 (Commit)
#include <linux/err.h>
#include <linux/io.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dai.h>
#include <linux/pm_runtime.h>

<<<<<<< HEAD
#include "acp63.h"

#define DRV_NAME "acp_ps_pdm_dma"

static int pdm_gain = 3;
module_param(pdm_gain, int, 0644);
MODULE_PARM_DESC(pdm_gain, "Gain control (0-3)");

static const struct snd_pcm_hardware acp63_pdm_hardware_capture = {
=======
#include "acp62.h"

#define DRV_NAME "acp_ps_pdm_dma"

static const struct snd_pcm_hardware acp62_pdm_hardware_capture = {
>>>>>>> b7ba80a49124 (Commit)
	.info = SNDRV_PCM_INFO_INTERLEAVED |
		SNDRV_PCM_INFO_BLOCK_TRANSFER |
		SNDRV_PCM_INFO_MMAP |
		SNDRV_PCM_INFO_MMAP_VALID |
		SNDRV_PCM_INFO_PAUSE | SNDRV_PCM_INFO_RESUME,
	.formats = SNDRV_PCM_FMTBIT_S32_LE,
	.channels_min = 2,
	.channels_max = 2,
	.rates = SNDRV_PCM_RATE_48000,
	.rate_min = 48000,
	.rate_max = 48000,
	.buffer_bytes_max = CAPTURE_MAX_NUM_PERIODS * CAPTURE_MAX_PERIOD_SIZE,
	.period_bytes_min = CAPTURE_MIN_PERIOD_SIZE,
	.period_bytes_max = CAPTURE_MAX_PERIOD_SIZE,
	.periods_min = CAPTURE_MIN_NUM_PERIODS,
	.periods_max = CAPTURE_MAX_NUM_PERIODS,
};

<<<<<<< HEAD
static void acp63_init_pdm_ring_buffer(u32 physical_addr, u32 buffer_size,
				       u32 watermark_size, void __iomem *acp_base)
{
	acp63_writel(physical_addr, acp_base + ACP_WOV_RX_RINGBUFADDR);
	acp63_writel(buffer_size, acp_base + ACP_WOV_RX_RINGBUFSIZE);
	acp63_writel(watermark_size, acp_base + ACP_WOV_RX_INTR_WATERMARK_SIZE);
	acp63_writel(0x01, acp_base + ACPAXI2AXI_ATU_CTRL);
}

static void acp63_enable_pdm_clock(void __iomem *acp_base)
=======
static void acp62_init_pdm_ring_buffer(u32 physical_addr, u32 buffer_size,
				       u32 watermark_size, void __iomem *acp_base)
{
	acp62_writel(physical_addr, acp_base + ACP_WOV_RX_RINGBUFADDR);
	acp62_writel(buffer_size, acp_base + ACP_WOV_RX_RINGBUFSIZE);
	acp62_writel(watermark_size, acp_base + ACP_WOV_RX_INTR_WATERMARK_SIZE);
	acp62_writel(0x01, acp_base + ACPAXI2AXI_ATU_CTRL);
}

static void acp62_enable_pdm_clock(void __iomem *acp_base)
>>>>>>> b7ba80a49124 (Commit)
{
	u32 pdm_clk_enable, pdm_ctrl;

	pdm_clk_enable = ACP_PDM_CLK_FREQ_MASK;
	pdm_ctrl = 0x00;

<<<<<<< HEAD
	acp63_writel(pdm_clk_enable, acp_base + ACP_WOV_CLK_CTRL);
	pdm_ctrl = acp63_readl(acp_base + ACP_WOV_MISC_CTRL);
	pdm_ctrl &= ~ACP_WOV_GAIN_CONTROL;
	pdm_ctrl |= FIELD_PREP(ACP_WOV_GAIN_CONTROL, clamp(pdm_gain, 0, 3));
	acp63_writel(pdm_ctrl, acp_base + ACP_WOV_MISC_CTRL);
}

static void acp63_enable_pdm_interrupts(struct pdm_dev_data *adata)
{
	u32 ext_int_ctrl;

	mutex_lock(adata->acp_lock);
	ext_int_ctrl = acp63_readl(adata->acp63_base + ACP_EXTERNAL_INTR_CNTL);
	ext_int_ctrl |= PDM_DMA_INTR_MASK;
	acp63_writel(ext_int_ctrl, adata->acp63_base + ACP_EXTERNAL_INTR_CNTL);
	mutex_unlock(adata->acp_lock);
}

static void acp63_disable_pdm_interrupts(struct pdm_dev_data *adata)
{
	u32 ext_int_ctrl;

	mutex_lock(adata->acp_lock);
	ext_int_ctrl = acp63_readl(adata->acp63_base + ACP_EXTERNAL_INTR_CNTL);
	ext_int_ctrl &= ~PDM_DMA_INTR_MASK;
	acp63_writel(ext_int_ctrl, adata->acp63_base + ACP_EXTERNAL_INTR_CNTL);
	mutex_unlock(adata->acp_lock);
}

static bool acp63_check_pdm_dma_status(void __iomem *acp_base)
=======
	acp62_writel(pdm_clk_enable, acp_base + ACP_WOV_CLK_CTRL);
	pdm_ctrl = acp62_readl(acp_base + ACP_WOV_MISC_CTRL);
	pdm_ctrl |= ACP_WOV_MISC_CTRL_MASK;
	acp62_writel(pdm_ctrl, acp_base + ACP_WOV_MISC_CTRL);
}

static void acp62_enable_pdm_interrupts(void __iomem *acp_base)
{
	u32 ext_int_ctrl;

	ext_int_ctrl = acp62_readl(acp_base + ACP_EXTERNAL_INTR_CNTL);
	ext_int_ctrl |= PDM_DMA_INTR_MASK;
	acp62_writel(ext_int_ctrl, acp_base + ACP_EXTERNAL_INTR_CNTL);
}

static void acp62_disable_pdm_interrupts(void __iomem *acp_base)
{
	u32 ext_int_ctrl;

	ext_int_ctrl = acp62_readl(acp_base + ACP_EXTERNAL_INTR_CNTL);
	ext_int_ctrl &= ~PDM_DMA_INTR_MASK;
	acp62_writel(ext_int_ctrl, acp_base + ACP_EXTERNAL_INTR_CNTL);
}

static bool acp62_check_pdm_dma_status(void __iomem *acp_base)
>>>>>>> b7ba80a49124 (Commit)
{
	bool pdm_dma_status;
	u32 pdm_enable, pdm_dma_enable;

	pdm_dma_status = false;
<<<<<<< HEAD
	pdm_enable = acp63_readl(acp_base + ACP_WOV_PDM_ENABLE);
	pdm_dma_enable = acp63_readl(acp_base + ACP_WOV_PDM_DMA_ENABLE);
=======
	pdm_enable = acp62_readl(acp_base + ACP_WOV_PDM_ENABLE);
	pdm_dma_enable = acp62_readl(acp_base + ACP_WOV_PDM_DMA_ENABLE);
>>>>>>> b7ba80a49124 (Commit)
	if ((pdm_enable & ACP_PDM_ENABLE) && (pdm_dma_enable & ACP_PDM_DMA_EN_STATUS))
		pdm_dma_status = true;

	return pdm_dma_status;
}

<<<<<<< HEAD
static int acp63_start_pdm_dma(void __iomem *acp_base)
=======
static int acp62_start_pdm_dma(void __iomem *acp_base)
>>>>>>> b7ba80a49124 (Commit)
{
	u32 pdm_enable;
	u32 pdm_dma_enable;
	int timeout;

	pdm_enable = 0x01;
	pdm_dma_enable  = 0x01;

<<<<<<< HEAD
	acp63_enable_pdm_clock(acp_base);
	acp63_writel(pdm_enable, acp_base + ACP_WOV_PDM_ENABLE);
	acp63_writel(pdm_dma_enable, acp_base + ACP_WOV_PDM_DMA_ENABLE);
	timeout = 0;
	while (++timeout < ACP_COUNTER) {
		pdm_dma_enable = acp63_readl(acp_base + ACP_WOV_PDM_DMA_ENABLE);
=======
	acp62_enable_pdm_clock(acp_base);
	acp62_writel(pdm_enable, acp_base + ACP_WOV_PDM_ENABLE);
	acp62_writel(pdm_dma_enable, acp_base + ACP_WOV_PDM_DMA_ENABLE);
	timeout = 0;
	while (++timeout < ACP_COUNTER) {
		pdm_dma_enable = acp62_readl(acp_base + ACP_WOV_PDM_DMA_ENABLE);
>>>>>>> b7ba80a49124 (Commit)
		if ((pdm_dma_enable & 0x02) == ACP_PDM_DMA_EN_STATUS)
			return 0;
		udelay(DELAY_US);
	}
	return -ETIMEDOUT;
}

<<<<<<< HEAD
static int acp63_stop_pdm_dma(void __iomem *acp_base)
=======
static int acp62_stop_pdm_dma(void __iomem *acp_base)
>>>>>>> b7ba80a49124 (Commit)
{
	u32 pdm_enable, pdm_dma_enable;
	int timeout;

	pdm_enable = 0x00;
	pdm_dma_enable  = 0x00;

<<<<<<< HEAD
	pdm_enable = acp63_readl(acp_base + ACP_WOV_PDM_ENABLE);
	pdm_dma_enable = acp63_readl(acp_base + ACP_WOV_PDM_DMA_ENABLE);
	if (pdm_dma_enable & 0x01) {
		pdm_dma_enable = 0x02;
		acp63_writel(pdm_dma_enable, acp_base + ACP_WOV_PDM_DMA_ENABLE);
		timeout = 0;
		while (++timeout < ACP_COUNTER) {
			pdm_dma_enable = acp63_readl(acp_base + ACP_WOV_PDM_DMA_ENABLE);
=======
	pdm_enable = acp62_readl(acp_base + ACP_WOV_PDM_ENABLE);
	pdm_dma_enable = acp62_readl(acp_base + ACP_WOV_PDM_DMA_ENABLE);
	if (pdm_dma_enable & 0x01) {
		pdm_dma_enable = 0x02;
		acp62_writel(pdm_dma_enable, acp_base + ACP_WOV_PDM_DMA_ENABLE);
		timeout = 0;
		while (++timeout < ACP_COUNTER) {
			pdm_dma_enable = acp62_readl(acp_base + ACP_WOV_PDM_DMA_ENABLE);
>>>>>>> b7ba80a49124 (Commit)
			if ((pdm_dma_enable & 0x02) == 0x00)
				break;
			udelay(DELAY_US);
		}
		if (timeout == ACP_COUNTER)
			return -ETIMEDOUT;
	}
	if (pdm_enable == ACP_PDM_ENABLE) {
		pdm_enable = ACP_PDM_DISABLE;
<<<<<<< HEAD
		acp63_writel(pdm_enable, acp_base + ACP_WOV_PDM_ENABLE);
	}
	acp63_writel(0x01, acp_base + ACP_WOV_PDM_FIFO_FLUSH);
	return 0;
}

static void acp63_config_dma(struct pdm_stream_instance *rtd, int direction)
=======
		acp62_writel(pdm_enable, acp_base + ACP_WOV_PDM_ENABLE);
	}
	acp62_writel(0x01, acp_base + ACP_WOV_PDM_FIFO_FLUSH);
	return 0;
}

static void acp62_config_dma(struct pdm_stream_instance *rtd, int direction)
>>>>>>> b7ba80a49124 (Commit)
{
	u16 page_idx;
	u32 low, high, val;
	dma_addr_t addr;

	addr = rtd->dma_addr;
	val = PDM_PTE_OFFSET;

	/* Group Enable */
<<<<<<< HEAD
	acp63_writel(ACP_SRAM_PTE_OFFSET | BIT(31), rtd->acp63_base +
		     ACPAXI2AXI_ATU_BASE_ADDR_GRP_1);
	acp63_writel(PAGE_SIZE_4K_ENABLE, rtd->acp63_base +
=======
	acp62_writel(ACP_SRAM_PTE_OFFSET | BIT(31), rtd->acp62_base +
		     ACPAXI2AXI_ATU_BASE_ADDR_GRP_1);
	acp62_writel(PAGE_SIZE_4K_ENABLE, rtd->acp62_base +
>>>>>>> b7ba80a49124 (Commit)
		     ACPAXI2AXI_ATU_PAGE_SIZE_GRP_1);
	for (page_idx = 0; page_idx < rtd->num_pages; page_idx++) {
		/* Load the low address of page int ACP SRAM through SRBM */
		low = lower_32_bits(addr);
		high = upper_32_bits(addr);

<<<<<<< HEAD
		acp63_writel(low, rtd->acp63_base + ACP_SCRATCH_REG_0 + val);
		high |= BIT(31);
		acp63_writel(high, rtd->acp63_base + ACP_SCRATCH_REG_0 + val + 4);
=======
		acp62_writel(low, rtd->acp62_base + ACP_SCRATCH_REG_0 + val);
		high |= BIT(31);
		acp62_writel(high, rtd->acp62_base + ACP_SCRATCH_REG_0 + val + 4);
>>>>>>> b7ba80a49124 (Commit)
		val += 8;
		addr += PAGE_SIZE;
	}
}

<<<<<<< HEAD
static int acp63_pdm_dma_open(struct snd_soc_component *component,
=======
static int acp62_pdm_dma_open(struct snd_soc_component *component,
>>>>>>> b7ba80a49124 (Commit)
			      struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime;
	struct pdm_dev_data *adata;
	struct pdm_stream_instance *pdm_data;
	int ret;

	runtime = substream->runtime;
	adata = dev_get_drvdata(component->dev);
	pdm_data = kzalloc(sizeof(*pdm_data), GFP_KERNEL);
	if (!pdm_data)
		return -EINVAL;

	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
<<<<<<< HEAD
		runtime->hw = acp63_pdm_hardware_capture;
=======
		runtime->hw = acp62_pdm_hardware_capture;
>>>>>>> b7ba80a49124 (Commit)

	ret = snd_pcm_hw_constraint_integer(runtime,
					    SNDRV_PCM_HW_PARAM_PERIODS);
	if (ret < 0) {
		dev_err(component->dev, "set integer constraint failed\n");
		kfree(pdm_data);
		return ret;
	}

<<<<<<< HEAD
	acp63_enable_pdm_interrupts(adata);
=======
	acp62_enable_pdm_interrupts(adata->acp62_base);
>>>>>>> b7ba80a49124 (Commit)

	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
		adata->capture_stream = substream;

<<<<<<< HEAD
	pdm_data->acp63_base = adata->acp63_base;
=======
	pdm_data->acp62_base = adata->acp62_base;
>>>>>>> b7ba80a49124 (Commit)
	runtime->private_data = pdm_data;
	return ret;
}

<<<<<<< HEAD
static int acp63_pdm_dma_hw_params(struct snd_soc_component *component,
=======
static int acp62_pdm_dma_hw_params(struct snd_soc_component *component,
>>>>>>> b7ba80a49124 (Commit)
				   struct snd_pcm_substream *substream,
				   struct snd_pcm_hw_params *params)
{
	struct pdm_stream_instance *rtd;
	size_t size, period_bytes;

	rtd = substream->runtime->private_data;
	if (!rtd)
		return -EINVAL;
	size = params_buffer_bytes(params);
	period_bytes = params_period_bytes(params);
	rtd->dma_addr = substream->runtime->dma_addr;
	rtd->num_pages = (PAGE_ALIGN(size) >> PAGE_SHIFT);
<<<<<<< HEAD
	acp63_config_dma(rtd, substream->stream);
	acp63_init_pdm_ring_buffer(PDM_MEM_WINDOW_START, size,
				   period_bytes, rtd->acp63_base);
	return 0;
}

static u64 acp63_pdm_get_byte_count(struct pdm_stream_instance *rtd,
=======
	acp62_config_dma(rtd, substream->stream);
	acp62_init_pdm_ring_buffer(PDM_MEM_WINDOW_START, size,
				   period_bytes, rtd->acp62_base);
	return 0;
}

static u64 acp62_pdm_get_byte_count(struct pdm_stream_instance *rtd,
>>>>>>> b7ba80a49124 (Commit)
				    int direction)
{
	u32 high, low;
	u64 byte_count;

<<<<<<< HEAD
	high = acp63_readl(rtd->acp63_base + ACP_WOV_RX_LINEARPOSITIONCNTR_HIGH);
	byte_count = high;
	low = acp63_readl(rtd->acp63_base + ACP_WOV_RX_LINEARPOSITIONCNTR_LOW);
=======
	high = acp62_readl(rtd->acp62_base + ACP_WOV_RX_LINEARPOSITIONCNTR_HIGH);
	byte_count = high;
	low = acp62_readl(rtd->acp62_base + ACP_WOV_RX_LINEARPOSITIONCNTR_LOW);
>>>>>>> b7ba80a49124 (Commit)
	byte_count = (byte_count << 32) | low;
	return byte_count;
}

<<<<<<< HEAD
static snd_pcm_uframes_t acp63_pdm_dma_pointer(struct snd_soc_component *comp,
=======
static snd_pcm_uframes_t acp62_pdm_dma_pointer(struct snd_soc_component *comp,
>>>>>>> b7ba80a49124 (Commit)
					       struct snd_pcm_substream *stream)
{
	struct pdm_stream_instance *rtd;
	u32 pos, buffersize;
	u64 bytescount;

	rtd = stream->runtime->private_data;
	buffersize = frames_to_bytes(stream->runtime,
				     stream->runtime->buffer_size);
<<<<<<< HEAD
	bytescount = acp63_pdm_get_byte_count(rtd, stream->stream);
=======
	bytescount = acp62_pdm_get_byte_count(rtd, stream->stream);
>>>>>>> b7ba80a49124 (Commit)
	if (bytescount > rtd->bytescount)
		bytescount -= rtd->bytescount;
	pos = do_div(bytescount, buffersize);
	return bytes_to_frames(stream->runtime, pos);
}

<<<<<<< HEAD
static int acp63_pdm_dma_new(struct snd_soc_component *component,
=======
static int acp62_pdm_dma_new(struct snd_soc_component *component,
>>>>>>> b7ba80a49124 (Commit)
			     struct snd_soc_pcm_runtime *rtd)
{
	struct device *parent = component->dev->parent;

	snd_pcm_set_managed_buffer_all(rtd->pcm, SNDRV_DMA_TYPE_DEV,
				       parent, MIN_BUFFER, MAX_BUFFER);
	return 0;
}

<<<<<<< HEAD
static int acp63_pdm_dma_close(struct snd_soc_component *component,
=======
static int acp62_pdm_dma_close(struct snd_soc_component *component,
>>>>>>> b7ba80a49124 (Commit)
			       struct snd_pcm_substream *substream)
{
	struct pdm_dev_data *adata = dev_get_drvdata(component->dev);
	struct snd_pcm_runtime *runtime = substream->runtime;

<<<<<<< HEAD
	acp63_disable_pdm_interrupts(adata);
=======
	acp62_disable_pdm_interrupts(adata->acp62_base);
>>>>>>> b7ba80a49124 (Commit)
	adata->capture_stream = NULL;
	kfree(runtime->private_data);
	return 0;
}

<<<<<<< HEAD
static int acp63_pdm_dai_trigger(struct snd_pcm_substream *substream,
=======
static int acp62_pdm_dai_trigger(struct snd_pcm_substream *substream,
>>>>>>> b7ba80a49124 (Commit)
				 int cmd, struct snd_soc_dai *dai)
{
	struct pdm_stream_instance *rtd;
	int ret;
	bool pdm_status;
	unsigned int ch_mask;

	rtd = substream->runtime->private_data;
	ret = 0;
	switch (substream->runtime->channels) {
	case TWO_CH:
		ch_mask = 0x00;
		break;
	default:
		return -EINVAL;
	}
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
<<<<<<< HEAD
		acp63_writel(ch_mask, rtd->acp63_base + ACP_WOV_PDM_NO_OF_CHANNELS);
		acp63_writel(PDM_DECIMATION_FACTOR, rtd->acp63_base +
			     ACP_WOV_PDM_DECIMATION_FACTOR);
		rtd->bytescount = acp63_pdm_get_byte_count(rtd, substream->stream);
		pdm_status = acp63_check_pdm_dma_status(rtd->acp63_base);
		if (!pdm_status)
			ret = acp63_start_pdm_dma(rtd->acp63_base);
=======
		acp62_writel(ch_mask, rtd->acp62_base + ACP_WOV_PDM_NO_OF_CHANNELS);
		acp62_writel(PDM_DECIMATION_FACTOR, rtd->acp62_base +
			     ACP_WOV_PDM_DECIMATION_FACTOR);
		rtd->bytescount = acp62_pdm_get_byte_count(rtd, substream->stream);
		pdm_status = acp62_check_pdm_dma_status(rtd->acp62_base);
		if (!pdm_status)
			ret = acp62_start_pdm_dma(rtd->acp62_base);
>>>>>>> b7ba80a49124 (Commit)
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
<<<<<<< HEAD
		pdm_status = acp63_check_pdm_dma_status(rtd->acp63_base);
		if (pdm_status)
			ret = acp63_stop_pdm_dma(rtd->acp63_base);
=======
		pdm_status = acp62_check_pdm_dma_status(rtd->acp62_base);
		if (pdm_status)
			ret = acp62_stop_pdm_dma(rtd->acp62_base);
>>>>>>> b7ba80a49124 (Commit)
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

<<<<<<< HEAD
static const struct snd_soc_dai_ops acp63_pdm_dai_ops = {
	.trigger   = acp63_pdm_dai_trigger,
};

static struct snd_soc_dai_driver acp63_pdm_dai_driver = {
=======
static const struct snd_soc_dai_ops acp62_pdm_dai_ops = {
	.trigger   = acp62_pdm_dai_trigger,
};

static struct snd_soc_dai_driver acp62_pdm_dai_driver = {
>>>>>>> b7ba80a49124 (Commit)
	.name = "acp_ps_pdm_dma.0",
	.capture = {
		.rates = SNDRV_PCM_RATE_48000,
		.formats = SNDRV_PCM_FMTBIT_S32_LE,
		.channels_min = 2,
		.channels_max = 2,
		.rate_min = 48000,
		.rate_max = 48000,
	},
<<<<<<< HEAD
	.ops = &acp63_pdm_dai_ops,
};

static const struct snd_soc_component_driver acp63_pdm_component = {
	.name		= DRV_NAME,
	.open		= acp63_pdm_dma_open,
	.close		= acp63_pdm_dma_close,
	.hw_params	= acp63_pdm_dma_hw_params,
	.pointer	= acp63_pdm_dma_pointer,
	.pcm_construct	= acp63_pdm_dma_new,
};

static int acp63_pdm_audio_probe(struct platform_device *pdev)
=======
	.ops = &acp62_pdm_dai_ops,
};

static const struct snd_soc_component_driver acp62_pdm_component = {
	.name		= DRV_NAME,
	.open		= acp62_pdm_dma_open,
	.close		= acp62_pdm_dma_close,
	.hw_params	= acp62_pdm_dma_hw_params,
	.pointer	= acp62_pdm_dma_pointer,
	.pcm_construct	= acp62_pdm_dma_new,
};

static int acp62_pdm_audio_probe(struct platform_device *pdev)
>>>>>>> b7ba80a49124 (Commit)
{
	struct resource *res;
	struct pdm_dev_data *adata;
	int status;

<<<<<<< HEAD
	if (!pdev->dev.platform_data) {
		dev_err(&pdev->dev, "platform_data not retrieved\n");
		return -ENODEV;
	}
=======
>>>>>>> b7ba80a49124 (Commit)
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "IORESOURCE_MEM FAILED\n");
		return -ENODEV;
	}

	adata = devm_kzalloc(&pdev->dev, sizeof(*adata), GFP_KERNEL);
	if (!adata)
		return -ENOMEM;

<<<<<<< HEAD
	adata->acp63_base = devm_ioremap(&pdev->dev, res->start, resource_size(res));
	if (!adata->acp63_base)
		return -ENOMEM;

	adata->capture_stream = NULL;
	adata->acp_lock = pdev->dev.platform_data;
	dev_set_drvdata(&pdev->dev, adata);
	status = devm_snd_soc_register_component(&pdev->dev,
						 &acp63_pdm_component,
						 &acp63_pdm_dai_driver, 1);
=======
	adata->acp62_base = devm_ioremap(&pdev->dev, res->start, resource_size(res));
	if (!adata->acp62_base)
		return -ENOMEM;

	adata->capture_stream = NULL;

	dev_set_drvdata(&pdev->dev, adata);
	status = devm_snd_soc_register_component(&pdev->dev,
						 &acp62_pdm_component,
						 &acp62_pdm_dai_driver, 1);
>>>>>>> b7ba80a49124 (Commit)
	if (status) {
		dev_err(&pdev->dev, "Fail to register acp pdm dai\n");

		return -ENODEV;
	}
	pm_runtime_set_autosuspend_delay(&pdev->dev, ACP_SUSPEND_DELAY_MS);
	pm_runtime_use_autosuspend(&pdev->dev);
	pm_runtime_enable(&pdev->dev);
	pm_runtime_allow(&pdev->dev);
	return 0;
}

<<<<<<< HEAD
static void acp63_pdm_audio_remove(struct platform_device *pdev)
{
	pm_runtime_disable(&pdev->dev);
}

static int __maybe_unused acp63_pdm_resume(struct device *dev)
=======
static int acp62_pdm_audio_remove(struct platform_device *pdev)
{
	pm_runtime_disable(&pdev->dev);
	return 0;
}

static int __maybe_unused acp62_pdm_resume(struct device *dev)
>>>>>>> b7ba80a49124 (Commit)
{
	struct pdm_dev_data *adata;
	struct snd_pcm_runtime *runtime;
	struct pdm_stream_instance *rtd;
	u32 period_bytes, buffer_len;

	adata = dev_get_drvdata(dev);
	if (adata->capture_stream && adata->capture_stream->runtime) {
		runtime = adata->capture_stream->runtime;
		rtd = runtime->private_data;
		period_bytes = frames_to_bytes(runtime, runtime->period_size);
		buffer_len = frames_to_bytes(runtime, runtime->buffer_size);
<<<<<<< HEAD
		acp63_config_dma(rtd, SNDRV_PCM_STREAM_CAPTURE);
		acp63_init_pdm_ring_buffer(PDM_MEM_WINDOW_START, buffer_len,
					   period_bytes, adata->acp63_base);
	}
	acp63_enable_pdm_interrupts(adata);
	return 0;
}

static int __maybe_unused acp63_pdm_suspend(struct device *dev)
=======
		acp62_config_dma(rtd, SNDRV_PCM_STREAM_CAPTURE);
		acp62_init_pdm_ring_buffer(PDM_MEM_WINDOW_START, buffer_len,
					   period_bytes, adata->acp62_base);
	}
	acp62_enable_pdm_interrupts(adata->acp62_base);
	return 0;
}

static int __maybe_unused acp62_pdm_suspend(struct device *dev)
>>>>>>> b7ba80a49124 (Commit)
{
	struct pdm_dev_data *adata;

	adata = dev_get_drvdata(dev);
<<<<<<< HEAD
	acp63_disable_pdm_interrupts(adata);
	return 0;
}

static int __maybe_unused acp63_pdm_runtime_resume(struct device *dev)
=======
	acp62_disable_pdm_interrupts(adata->acp62_base);
	return 0;
}

static int __maybe_unused acp62_pdm_runtime_resume(struct device *dev)
>>>>>>> b7ba80a49124 (Commit)
{
	struct pdm_dev_data *adata;

	adata = dev_get_drvdata(dev);
<<<<<<< HEAD
	acp63_enable_pdm_interrupts(adata);
	return 0;
}

static const struct dev_pm_ops acp63_pdm_pm_ops = {
	SET_RUNTIME_PM_OPS(acp63_pdm_suspend, acp63_pdm_runtime_resume, NULL)
	SET_SYSTEM_SLEEP_PM_OPS(acp63_pdm_suspend, acp63_pdm_resume)
};

static struct platform_driver acp63_pdm_dma_driver = {
	.probe = acp63_pdm_audio_probe,
	.remove_new = acp63_pdm_audio_remove,
	.driver = {
		.name = "acp_ps_pdm_dma",
		.pm = &acp63_pdm_pm_ops,
	},
};

module_platform_driver(acp63_pdm_dma_driver);
=======
	acp62_enable_pdm_interrupts(adata->acp62_base);
	return 0;
}

static const struct dev_pm_ops acp62_pdm_pm_ops = {
	SET_RUNTIME_PM_OPS(acp62_pdm_suspend, acp62_pdm_runtime_resume, NULL)
	SET_SYSTEM_SLEEP_PM_OPS(acp62_pdm_suspend, acp62_pdm_resume)
};

static struct platform_driver acp62_pdm_dma_driver = {
	.probe = acp62_pdm_audio_probe,
	.remove = acp62_pdm_audio_remove,
	.driver = {
		.name = "acp_ps_pdm_dma",
		.pm = &acp62_pdm_pm_ops,
	},
};

module_platform_driver(acp62_pdm_dma_driver);
>>>>>>> b7ba80a49124 (Commit)

MODULE_AUTHOR("Syed.SabaKareem@amd.com");
MODULE_DESCRIPTION("AMD PINK SARDINE PDM Driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRV_NAME);
