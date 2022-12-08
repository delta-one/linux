// SPDX-License-Identifier: GPL-2.0
/*
 * Support for OmniVision OV2680 1080p HD camera sensor.
 *
 * Copyright (c) 2013 Intel Corporation. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

<<<<<<< HEAD
#include <linux/acpi.h>
#include <linux/device.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/machine.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/pm_runtime.h>
#include <linux/types.h>

#include <media/ov_16bit_addr_reg_helpers.h>
#include <media/v4l2-device.h>

=======
#include <asm/unaligned.h>

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/moduleparam.h>
#include <media/v4l2-device.h>
#include <linux/io.h>
#include <linux/acpi.h>
>>>>>>> b7ba80a49124 (Commit)
#include "../include/linux/atomisp_gmin_platform.h"

#include "ov2680.h"

<<<<<<< HEAD
=======
static int h_flag;
static int v_flag;
>>>>>>> b7ba80a49124 (Commit)
static enum atomisp_bayer_order ov2680_bayer_order_mapping[] = {
	atomisp_bayer_order_bggr,
	atomisp_bayer_order_grbg,
	atomisp_bayer_order_gbrg,
	atomisp_bayer_order_rggb,
};

<<<<<<< HEAD
=======
/* i2c read/write stuff */
static int ov2680_read_reg(struct i2c_client *client,
			   int len, u16 reg, u32 *val)
{
	struct i2c_msg msgs[2];
	u8 addr_buf[2] = { reg >> 8, reg & 0xff };
	u8 data_buf[4] = { 0, };
	int ret;

	if (len > 4)
		return -EINVAL;

	msgs[0].addr = client->addr;
	msgs[0].flags = 0;
	msgs[0].len = ARRAY_SIZE(addr_buf);
	msgs[0].buf = addr_buf;

	msgs[1].addr = client->addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = len;
	msgs[1].buf = &data_buf[4 - len];

	ret = i2c_transfer(client->adapter, msgs, ARRAY_SIZE(msgs));
	if (ret != ARRAY_SIZE(msgs)) {
		dev_err(&client->dev, "read error: reg=0x%4x: %d\n", reg, ret);
		return -EIO;
	}

	*val = get_unaligned_be32(data_buf);

	return 0;
}

static int ov2680_write_reg(struct i2c_client *client, unsigned int len,
			    u16 reg, u16 val)
{
	u8 buf[6];
	int ret;

	if (len == 2)
		put_unaligned_be16(val, buf + 2);
	else if (len == 1)
		buf[2] = val;
	else
		return -EINVAL;

	put_unaligned_be16(reg, buf);

	ret = i2c_master_send(client, buf, len + 2);
	if (ret != len + 2) {
		dev_err(&client->dev, "write error %d reg 0x%04x, val 0x%02x: buf sent: %*ph\n",
			ret, reg, val, len + 2, &buf);
		return -EIO;
	}

	return 0;
}

>>>>>>> b7ba80a49124 (Commit)
static int ov2680_write_reg_array(struct i2c_client *client,
				  const struct ov2680_reg *reglist)
{
	const struct ov2680_reg *next = reglist;
	int ret;

	for (; next->reg != 0; next++) {
<<<<<<< HEAD
		ret = ov_write_reg8(client, next->reg, next->val);
=======
		ret = ov2680_write_reg(client, 1, next->reg, next->val);
>>>>>>> b7ba80a49124 (Commit)
		if (ret)
			return ret;
	}

	return 0;
}

<<<<<<< HEAD
static void ov2680_set_bayer_order(struct ov2680_device *sensor, struct v4l2_mbus_framefmt *fmt)
{
	static const int ov2680_hv_flip_bayer_order[] = {
		MEDIA_BUS_FMT_SBGGR10_1X10,
		MEDIA_BUS_FMT_SGRBG10_1X10,
		MEDIA_BUS_FMT_SGBRG10_1X10,
		MEDIA_BUS_FMT_SRGGB10_1X10,
	};
	struct camera_mipi_info *ov2680_info;
	int hv_flip = 0;

	if (sensor->ctrls.vflip->val)
		hv_flip += 1;

	if (sensor->ctrls.hflip->val)
		hv_flip += 2;

	fmt->code = ov2680_hv_flip_bayer_order[hv_flip];

	/* TODO atomisp specific custom API, should be removed */
	ov2680_info = v4l2_get_subdev_hostdata(&sensor->sd);
	if (ov2680_info)
		ov2680_info->raw_bayer_order = ov2680_bayer_order_mapping[hv_flip];
}

static int ov2680_set_vflip(struct ov2680_device *sensor, s32 val)
{
	int ret;

	if (sensor->is_streaming)
		return -EBUSY;

	ret = ov_update_reg(sensor->client, OV2680_REG_FORMAT1, BIT(2), val ? BIT(2) : 0);
	if (ret < 0)
		return ret;

	ov2680_set_bayer_order(sensor, &sensor->mode.fmt);
	return 0;
}

static int ov2680_set_hflip(struct ov2680_device *sensor, s32 val)
{
	int ret;

	if (sensor->is_streaming)
		return -EBUSY;

	ret = ov_update_reg(sensor->client, OV2680_REG_FORMAT2, BIT(2), val ? BIT(2) : 0);
	if (ret < 0)
		return ret;

	ov2680_set_bayer_order(sensor, &sensor->mode.fmt);
	return 0;
}

static int ov2680_exposure_set(struct ov2680_device *sensor, u32 exp)
{
	return ov_write_reg24(sensor->client, OV2680_REG_EXPOSURE_PK_HIGH, exp << 4);
}

static int ov2680_gain_set(struct ov2680_device *sensor, u32 gain)
{
	return ov_write_reg16(sensor->client, OV2680_REG_GAIN_PK, gain);
}

static int ov2680_test_pattern_set(struct ov2680_device *sensor, int value)
{
	int ret;

	if (!value)
		return ov_update_reg(sensor->client, OV2680_REG_ISP_CTRL00, BIT(7), 0);

	ret = ov_update_reg(sensor->client, OV2680_REG_ISP_CTRL00, 0x03, value - 1);
	if (ret < 0)
		return ret;

	ret = ov_update_reg(sensor->client, OV2680_REG_ISP_CTRL00, BIT(7), BIT(7));
	if (ret < 0)
		return ret;
=======
static int ov2680_g_focal(struct v4l2_subdev *sd, s32 *val)
{
	*val = (OV2680_FOCAL_LENGTH_NUM << 16) | OV2680_FOCAL_LENGTH_DEM;
	return 0;
}

static int ov2680_g_fnumber(struct v4l2_subdev *sd, s32 *val)
{
	/* const f number for ov2680 */

	*val = (OV2680_F_NUMBER_DEFAULT_NUM << 16) | OV2680_F_NUMBER_DEM;
	return 0;
}

static int ov2680_g_fnumber_range(struct v4l2_subdev *sd, s32 *val)
{
	*val = (OV2680_F_NUMBER_DEFAULT_NUM << 24) |
	       (OV2680_F_NUMBER_DEM << 16) |
	       (OV2680_F_NUMBER_DEFAULT_NUM << 8) | OV2680_F_NUMBER_DEM;
	return 0;
}

static int ov2680_g_bin_factor_x(struct v4l2_subdev *sd, s32 *val)
{
	struct ov2680_device *dev = to_ov2680_sensor(sd);
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	dev_dbg(&client->dev,  "++++ov2680_g_bin_factor_x\n");
	*val = dev->res->bin_factor_x;
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

<<<<<<< HEAD
static int ov2680_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct v4l2_subdev *sd = ctrl_to_sd(ctrl);
	struct ov2680_device *sensor = to_ov2680_sensor(sd);
	int ret;

	/* Only apply changes to the controls if the device is powered up */
	if (!pm_runtime_get_if_in_use(sensor->sd.dev)) {
		ov2680_set_bayer_order(sensor, &sensor->mode.fmt);
		return 0;
	}

	switch (ctrl->id) {
	case V4L2_CID_VFLIP:
		ret = ov2680_set_vflip(sensor, ctrl->val);
		break;
	case V4L2_CID_HFLIP:
		ret = ov2680_set_hflip(sensor, ctrl->val);
		break;
	case V4L2_CID_EXPOSURE:
		ret = ov2680_exposure_set(sensor, ctrl->val);
		break;
	case V4L2_CID_GAIN:
		ret = ov2680_gain_set(sensor, ctrl->val);
		break;
	case V4L2_CID_TEST_PATTERN:
		ret = ov2680_test_pattern_set(sensor, ctrl->val);
=======
static int ov2680_g_bin_factor_y(struct v4l2_subdev *sd, s32 *val)
{
	struct ov2680_device *dev = to_ov2680_sensor(sd);
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	*val = dev->res->bin_factor_y;
	dev_dbg(&client->dev,  "++++ov2680_g_bin_factor_y\n");
	return 0;
}

static int ov2680_get_intg_factor(struct i2c_client *client,
				  struct camera_mipi_info *info,
				  const struct ov2680_resolution *res)
{
	struct atomisp_sensor_mode_data *buf = &info->data;
	unsigned int pix_clk_freq_hz;
	u32 reg_val;
	int ret;

	dev_dbg(&client->dev,  "++++ov2680_get_intg_factor\n");
	if (!info)
		return -EINVAL;

	/* pixel clock */
	pix_clk_freq_hz = res->pix_clk_freq * 1000000;

	buf->vt_pix_clk_freq_mhz = pix_clk_freq_hz;

	/* get integration time */
	buf->coarse_integration_time_min = OV2680_COARSE_INTG_TIME_MIN;
	buf->coarse_integration_time_max_margin =
	    OV2680_COARSE_INTG_TIME_MAX_MARGIN;

	buf->fine_integration_time_min = OV2680_FINE_INTG_TIME_MIN;
	buf->fine_integration_time_max_margin =
	    OV2680_FINE_INTG_TIME_MAX_MARGIN;

	buf->fine_integration_time_def = OV2680_FINE_INTG_TIME_MIN;
	buf->frame_length_lines = res->lines_per_frame;
	buf->line_length_pck = res->pixels_per_line;
	buf->read_mode = res->bin_mode;

	/* get the cropping and output resolution to ISP for this mode. */
	ret =  ov2680_read_reg(client, 2,
			       OV2680_HORIZONTAL_START_H, &reg_val);
	if (ret)
		return ret;
	buf->crop_horizontal_start = reg_val;

	ret =  ov2680_read_reg(client, 2,
			       OV2680_VERTICAL_START_H, &reg_val);
	if (ret)
		return ret;
	buf->crop_vertical_start = reg_val;

	ret = ov2680_read_reg(client, 2,
			      OV2680_HORIZONTAL_END_H, &reg_val);
	if (ret)
		return ret;
	buf->crop_horizontal_end = reg_val;

	ret = ov2680_read_reg(client, 2,
			      OV2680_VERTICAL_END_H, &reg_val);
	if (ret)
		return ret;
	buf->crop_vertical_end = reg_val;

	ret = ov2680_read_reg(client, 2,
			      OV2680_HORIZONTAL_OUTPUT_SIZE_H, &reg_val);
	if (ret)
		return ret;
	buf->output_width = reg_val;

	ret = ov2680_read_reg(client, 2,
			      OV2680_VERTICAL_OUTPUT_SIZE_H, &reg_val);
	if (ret)
		return ret;
	buf->output_height = reg_val;

	buf->binning_factor_x = res->bin_factor_x ?
				(res->bin_factor_x * 2) : 1;
	buf->binning_factor_y = res->bin_factor_y ?
				(res->bin_factor_y * 2) : 1;
	return 0;
}

static long __ov2680_set_exposure(struct v4l2_subdev *sd, int coarse_itg,
				  int gain, int digitgain)

{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct ov2680_device *dev = to_ov2680_sensor(sd);
	u16 vts;
	int ret, exp_val;

	dev_dbg(&client->dev,
		"+++++++__ov2680_set_exposure coarse_itg %d, gain %d, digitgain %d++\n",
		coarse_itg, gain, digitgain);

	vts = dev->res->lines_per_frame;

	/* group hold */
	ret = ov2680_write_reg(client, 1,
			       OV2680_GROUP_ACCESS, 0x00);
	if (ret) {
		dev_err(&client->dev, "%s: write 0x%02x: error, aborted\n",
			__func__, OV2680_GROUP_ACCESS);
		return ret;
	}

	/* Increase the VTS to match exposure + MARGIN */
	if (coarse_itg > vts - OV2680_INTEGRATION_TIME_MARGIN)
		vts = (u16)coarse_itg + OV2680_INTEGRATION_TIME_MARGIN;

	ret = ov2680_write_reg(client, 2, OV2680_TIMING_VTS_H, vts);
	if (ret) {
		dev_err(&client->dev, "%s: write 0x%02x: error, aborted\n",
			__func__, OV2680_TIMING_VTS_H);
		return ret;
	}

	/* set exposure */

	/* Lower four bit should be 0*/
	exp_val = coarse_itg << 4;
	ret = ov2680_write_reg(client, 1,
			       OV2680_EXPOSURE_L, exp_val & 0xFF);
	if (ret) {
		dev_err(&client->dev, "%s: write 0x%02x: error, aborted\n",
			__func__, OV2680_EXPOSURE_L);
		return ret;
	}

	ret = ov2680_write_reg(client, 1,
			       OV2680_EXPOSURE_M, (exp_val >> 8) & 0xFF);
	if (ret) {
		dev_err(&client->dev, "%s: write 0x%02x: error, aborted\n",
			__func__, OV2680_EXPOSURE_M);
		return ret;
	}

	ret = ov2680_write_reg(client, 1,
			       OV2680_EXPOSURE_H, (exp_val >> 16) & 0x0F);
	if (ret) {
		dev_err(&client->dev, "%s: write 0x%02x: error, aborted\n",
			__func__, OV2680_EXPOSURE_H);
		return ret;
	}

	/* Analog gain */
	ret = ov2680_write_reg(client, 2, OV2680_AGC_H, gain);
	if (ret) {
		dev_err(&client->dev, "%s: write 0x%02x: error, aborted\n",
			__func__, OV2680_AGC_H);
		return ret;
	}
	/* Digital gain */
	if (digitgain) {
		ret = ov2680_write_reg(client, 2,
				       OV2680_MWB_RED_GAIN_H, digitgain);
		if (ret) {
			dev_err(&client->dev,
				"%s: write 0x%02x: error, aborted\n",
				__func__, OV2680_MWB_RED_GAIN_H);
			return ret;
		}

		ret = ov2680_write_reg(client, 2,
				       OV2680_MWB_GREEN_GAIN_H, digitgain);
		if (ret) {
			dev_err(&client->dev,
				"%s: write 0x%02x: error, aborted\n",
				__func__, OV2680_MWB_RED_GAIN_H);
			return ret;
		}

		ret = ov2680_write_reg(client, 2,
				       OV2680_MWB_BLUE_GAIN_H, digitgain);
		if (ret) {
			dev_err(&client->dev,
				"%s: write 0x%02x: error, aborted\n",
				__func__, OV2680_MWB_RED_GAIN_H);
			return ret;
		}
	}

	/* End group */
	ret = ov2680_write_reg(client, 1,
			       OV2680_GROUP_ACCESS, 0x10);
	if (ret)
		return ret;

	/* Delay launch group */
	ret = ov2680_write_reg(client, 1,
			       OV2680_GROUP_ACCESS, 0xa0);
	if (ret)
		return ret;
	return ret;
}

static int ov2680_set_exposure(struct v4l2_subdev *sd, int exposure,
			       int gain, int digitgain)
{
	struct ov2680_device *dev = to_ov2680_sensor(sd);
	int ret = 0;

	mutex_lock(&dev->input_lock);

	dev->exposure = exposure;
	dev->gain = gain;
	dev->digitgain = digitgain;

	if (dev->power_on)
		ret = __ov2680_set_exposure(sd, exposure, gain, digitgain);

	mutex_unlock(&dev->input_lock);

	return ret;
}

static long ov2680_s_exposure(struct v4l2_subdev *sd,
			      struct atomisp_exposure *exposure)
{
	u16 coarse_itg = exposure->integration_time[0];
	u16 analog_gain = exposure->gain[0];
	u16 digital_gain = exposure->gain[1];

	/* we should not accept the invalid value below */
	if (analog_gain == 0) {
		struct i2c_client *client = v4l2_get_subdevdata(sd);

		v4l2_err(client, "%s: invalid value\n", __func__);
		return -EINVAL;
	}

	return ov2680_set_exposure(sd, coarse_itg, analog_gain, digital_gain);
}

static long ov2680_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
	switch (cmd) {
	case ATOMISP_IOC_S_EXPOSURE:
		return ov2680_s_exposure(sd, arg);

	default:
		return -EINVAL;
	}
	return 0;
}

/*
 * This returns the exposure time being used. This should only be used
 * for filling in EXIF data, not for actual image processing.
 */
static int ov2680_q_exposure(struct v4l2_subdev *sd, s32 *value)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	u32 reg_val;
	int ret;

	/* get exposure */
	ret = ov2680_read_reg(client, 3, OV2680_EXPOSURE_H, &reg_val);
	if (ret)
		return ret;

	/* Lower four bits are not part of the exposure val (always 0) */
	*value = reg_val >> 4;
	return 0;
}

static int ov2680_v_flip(struct v4l2_subdev *sd, s32 value)
{
	struct camera_mipi_info *ov2680_info = NULL;
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret;
	u32 val;
	u8 index;

	dev_dbg(&client->dev, "@%s: value:%d\n", __func__, value);
	ret = ov2680_read_reg(client, 1, OV2680_FLIP_REG, &val);
	if (ret)
		return ret;
	if (value)
		val |= OV2680_FLIP_MIRROR_BIT_ENABLE;
	else
		val &= ~OV2680_FLIP_MIRROR_BIT_ENABLE;

	ret = ov2680_write_reg(client, 1,
			       OV2680_FLIP_REG, val);
	if (ret)
		return ret;
	index = (v_flag > 0 ? OV2680_FLIP_BIT : 0) | (h_flag > 0 ? OV2680_MIRROR_BIT :
		0);
	ov2680_info = v4l2_get_subdev_hostdata(sd);
	if (ov2680_info) {
		ov2680_info->raw_bayer_order = ov2680_bayer_order_mapping[index];
	}
	return ret;
}

static int ov2680_h_flip(struct v4l2_subdev *sd, s32 value)
{
	struct camera_mipi_info *ov2680_info = NULL;
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret;
	u32 val;
	u8 index;

	dev_dbg(&client->dev, "@%s: value:%d\n", __func__, value);

	ret = ov2680_read_reg(client, 1, OV2680_MIRROR_REG, &val);
	if (ret)
		return ret;
	if (value)
		val |= OV2680_FLIP_MIRROR_BIT_ENABLE;
	else
		val &= ~OV2680_FLIP_MIRROR_BIT_ENABLE;

	ret = ov2680_write_reg(client, 1,
			       OV2680_MIRROR_REG, val);
	if (ret)
		return ret;
	index = (v_flag > 0 ? OV2680_FLIP_BIT : 0) | (h_flag > 0 ? OV2680_MIRROR_BIT :
		0);
	ov2680_info = v4l2_get_subdev_hostdata(sd);
	if (ov2680_info) {
		ov2680_info->raw_bayer_order = ov2680_bayer_order_mapping[index];
	}
	return ret;
}

static int ov2680_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct ov2680_device *dev =
	    container_of(ctrl->handler, struct ov2680_device, ctrl_handler);
	struct i2c_client *client = v4l2_get_subdevdata(&dev->sd);
	int ret = 0;

	switch (ctrl->id) {
	case V4L2_CID_VFLIP:
		dev_dbg(&client->dev, "%s: CID_VFLIP:%d.\n",
			__func__, ctrl->val);
		ret = ov2680_v_flip(&dev->sd, ctrl->val);
		break;
	case V4L2_CID_HFLIP:
		dev_dbg(&client->dev, "%s: CID_HFLIP:%d.\n",
			__func__, ctrl->val);
		ret = ov2680_h_flip(&dev->sd, ctrl->val);
		break;
	default:
		ret = -EINVAL;
	}
	return ret;
}

static int ov2680_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	struct ov2680_device *dev =
	    container_of(ctrl->handler, struct ov2680_device, ctrl_handler);
	int ret = 0;

	switch (ctrl->id) {
	case V4L2_CID_EXPOSURE_ABSOLUTE:
		ret = ov2680_q_exposure(&dev->sd, &ctrl->val);
		break;
	case V4L2_CID_FOCAL_ABSOLUTE:
		ret = ov2680_g_focal(&dev->sd, &ctrl->val);
		break;
	case V4L2_CID_FNUMBER_ABSOLUTE:
		ret = ov2680_g_fnumber(&dev->sd, &ctrl->val);
		break;
	case V4L2_CID_FNUMBER_RANGE:
		ret = ov2680_g_fnumber_range(&dev->sd, &ctrl->val);
		break;
	case V4L2_CID_BIN_FACTOR_HORZ:
		ret = ov2680_g_bin_factor_x(&dev->sd, &ctrl->val);
		break;
	case V4L2_CID_BIN_FACTOR_VERT:
		ret = ov2680_g_bin_factor_y(&dev->sd, &ctrl->val);
>>>>>>> b7ba80a49124 (Commit)
		break;
	default:
		ret = -EINVAL;
	}

<<<<<<< HEAD
	pm_runtime_put(sensor->sd.dev);
	return ret;
}

static const struct v4l2_ctrl_ops ov2680_ctrl_ops = {
	.s_ctrl = ov2680_s_ctrl,
=======
	return ret;
}

static const struct v4l2_ctrl_ops ctrl_ops = {
	.s_ctrl = ov2680_s_ctrl,
	.g_volatile_ctrl = ov2680_g_volatile_ctrl
};

static const struct v4l2_ctrl_config ov2680_controls[] = {
	{
		.ops = &ctrl_ops,
		.id = V4L2_CID_EXPOSURE_ABSOLUTE,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "exposure",
		.min = 0x0,
		.max = 0xffff,
		.step = 0x01,
		.def = 0x00,
		.flags = 0,
	},
	{
		.ops = &ctrl_ops,
		.id = V4L2_CID_FOCAL_ABSOLUTE,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "focal length",
		.min = OV2680_FOCAL_LENGTH_DEFAULT,
		.max = OV2680_FOCAL_LENGTH_DEFAULT,
		.step = 0x01,
		.def = OV2680_FOCAL_LENGTH_DEFAULT,
		.flags = 0,
	},
	{
		.ops = &ctrl_ops,
		.id = V4L2_CID_FNUMBER_ABSOLUTE,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "f-number",
		.min = OV2680_F_NUMBER_DEFAULT,
		.max = OV2680_F_NUMBER_DEFAULT,
		.step = 0x01,
		.def = OV2680_F_NUMBER_DEFAULT,
		.flags = 0,
	},
	{
		.ops = &ctrl_ops,
		.id = V4L2_CID_FNUMBER_RANGE,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "f-number range",
		.min = OV2680_F_NUMBER_RANGE,
		.max = OV2680_F_NUMBER_RANGE,
		.step = 0x01,
		.def = OV2680_F_NUMBER_RANGE,
		.flags = 0,
	},
	{
		.ops = &ctrl_ops,
		.id = V4L2_CID_BIN_FACTOR_HORZ,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "horizontal binning factor",
		.min = 0,
		.max = OV2680_BIN_FACTOR_MAX,
		.step = 1,
		.def = 0,
		.flags = 0,
	},
	{
		.ops = &ctrl_ops,
		.id = V4L2_CID_BIN_FACTOR_VERT,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "vertical binning factor",
		.min = 0,
		.max = OV2680_BIN_FACTOR_MAX,
		.step = 1,
		.def = 0,
		.flags = 0,
	},
	{
		.ops = &ctrl_ops,
		.id = V4L2_CID_VFLIP,
		.type = V4L2_CTRL_TYPE_BOOLEAN,
		.name = "Flip",
		.min = 0,
		.max = 1,
		.step = 1,
		.def = 0,
	},
	{
		.ops = &ctrl_ops,
		.id = V4L2_CID_HFLIP,
		.type = V4L2_CTRL_TYPE_BOOLEAN,
		.name = "Mirror",
		.min = 0,
		.max = 1,
		.step = 1,
		.def = 0,
	},
>>>>>>> b7ba80a49124 (Commit)
};

static int ov2680_init_registers(struct v4l2_subdev *sd)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret;

<<<<<<< HEAD
	ret = ov_write_reg8(client, OV2680_SW_RESET, 0x01);
=======
	ret = ov2680_write_reg(client, 1, OV2680_SW_RESET, 0x01);
>>>>>>> b7ba80a49124 (Commit)
	ret |= ov2680_write_reg_array(client, ov2680_global_setting);

	return ret;
}

<<<<<<< HEAD
static struct v4l2_mbus_framefmt *
__ov2680_get_pad_format(struct ov2680_device *sensor,
			struct v4l2_subdev_state *state,
			unsigned int pad, enum v4l2_subdev_format_whence which)
{
	if (which == V4L2_SUBDEV_FORMAT_TRY)
		return v4l2_subdev_get_try_format(&sensor->sd, state, pad);

	return &sensor->mode.fmt;
}

static void ov2680_fill_format(struct ov2680_device *sensor,
			       struct v4l2_mbus_framefmt *fmt,
			       unsigned int width, unsigned int height)
{
	memset(fmt, 0, sizeof(*fmt));
	fmt->width = width;
	fmt->height = height;
	fmt->field = V4L2_FIELD_NONE;
	ov2680_set_bayer_order(sensor, fmt);
}

static void ov2680_calc_mode(struct ov2680_device *sensor, int width, int height)
{
	int orig_width = width;
	int orig_height = height;

	if (width  <= (OV2680_NATIVE_WIDTH / 2) &&
	    height <= (OV2680_NATIVE_HEIGHT / 2)) {
		sensor->mode.binning = true;
		width *= 2;
		height *= 2;
	} else {
		sensor->mode.binning = false;
	}

	sensor->mode.h_start = ((OV2680_NATIVE_WIDTH - width) / 2) & ~1;
	sensor->mode.v_start = ((OV2680_NATIVE_HEIGHT - height) / 2) & ~1;
	sensor->mode.h_end = min(sensor->mode.h_start + width + OV2680_END_MARGIN - 1,
				 OV2680_NATIVE_WIDTH - 1);
	sensor->mode.v_end = min(sensor->mode.v_start + height + OV2680_END_MARGIN - 1,
				 OV2680_NATIVE_HEIGHT - 1);
	sensor->mode.h_output_size = orig_width;
	sensor->mode.v_output_size = orig_height;
	sensor->mode.hts = OV2680_PIXELS_PER_LINE;
	sensor->mode.vts = OV2680_LINES_PER_FRAME;
}

static int ov2680_set_mode(struct ov2680_device *sensor)
{
	struct i2c_client *client = sensor->client;
	u8 pll_div, unknown, inc, fmt1, fmt2;
	int ret;

	if (sensor->mode.binning) {
		pll_div = 1;
		unknown = 0x23;
		inc = 0x31;
		fmt1 = 0xc2;
		fmt2 = 0x01;
	} else {
		pll_div = 0;
		unknown = 0x21;
		inc = 0x11;
		fmt1 = 0xc0;
		fmt2 = 0x00;
	}

	ret = ov_write_reg8(client, 0x3086, pll_div);
	if (ret)
		return ret;

	ret = ov_write_reg8(client, 0x370a, unknown);
	if (ret)
		return ret;

	ret = ov_write_reg16(client, OV2680_HORIZONTAL_START_H, sensor->mode.h_start);
	if (ret)
		return ret;

	ret = ov_write_reg16(client, OV2680_VERTICAL_START_H, sensor->mode.v_start);
	if (ret)
		return ret;

	ret = ov_write_reg16(client, OV2680_HORIZONTAL_END_H, sensor->mode.h_end);
	if (ret)
		return ret;

	ret = ov_write_reg16(client, OV2680_VERTICAL_END_H, sensor->mode.v_end);
	if (ret)
		return ret;

	ret = ov_write_reg16(client, OV2680_HORIZONTAL_OUTPUT_SIZE_H,
				 sensor->mode.h_output_size);
	if (ret)
		return ret;

	ret = ov_write_reg16(client, OV2680_VERTICAL_OUTPUT_SIZE_H,
				 sensor->mode.v_output_size);
	if (ret)
		return ret;

	ret = ov_write_reg16(client, OV2680_HTS, sensor->mode.hts);
	if (ret)
		return ret;

	ret = ov_write_reg16(client, OV2680_VTS, sensor->mode.vts);
	if (ret)
		return ret;

	ret = ov_write_reg16(client, OV2680_ISP_X_WIN, 0);
	if (ret)
		return ret;

	ret = ov_write_reg16(client, OV2680_ISP_Y_WIN, 0);
	if (ret)
		return ret;

	ret = ov_write_reg8(client, OV2680_X_INC, inc);
	if (ret)
		return ret;

	ret = ov_write_reg8(client, OV2680_Y_INC, inc);
	if (ret)
		return ret;

	ret = ov_write_reg16(client, OV2680_X_WIN, sensor->mode.h_output_size);
	if (ret)
		return ret;

	ret = ov_write_reg16(client, OV2680_Y_WIN, sensor->mode.v_output_size);
	if (ret)
		return ret;

	ret = ov_write_reg8(client, OV2680_REG_FORMAT1, fmt1);
	if (ret)
		return ret;

	ret = ov_write_reg8(client, OV2680_REG_FORMAT2, fmt2);
	if (ret)
		return ret;

	return 0;
=======
static int power_ctrl(struct v4l2_subdev *sd, bool flag)
{
	int ret = 0;
	struct ov2680_device *dev = to_ov2680_sensor(sd);
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	if (!dev || !dev->platform_data)
		return -ENODEV;

	dev_dbg(&client->dev, "%s: %s", __func__, flag ? "on" : "off");

	if (flag) {
		ret |= dev->platform_data->v1p8_ctrl(sd, 1);
		ret |= dev->platform_data->v2p8_ctrl(sd, 1);
		usleep_range(10000, 15000);
	}

	if (!flag || ret) {
		ret |= dev->platform_data->v1p8_ctrl(sd, 0);
		ret |= dev->platform_data->v2p8_ctrl(sd, 0);
	}
	return ret;
}

static int gpio_ctrl(struct v4l2_subdev *sd, bool flag)
{
	int ret;
	struct ov2680_device *dev = to_ov2680_sensor(sd);

	if (!dev || !dev->platform_data)
		return -ENODEV;

	/*
	 * The OV2680 documents only one GPIO input (#XSHUTDN), but
	 * existing integrations often wire two (reset/power_down)
	 * because that is the way other sensors work.  There is no
	 * way to tell how it is wired internally, so existing
	 * firmwares expose both and we drive them symmetrically.
	 */
	if (flag) {
		ret = dev->platform_data->gpio0_ctrl(sd, 1);
		usleep_range(10000, 15000);
		/* Ignore return from second gpio, it may not be there */
		dev->platform_data->gpio1_ctrl(sd, 1);
		usleep_range(10000, 15000);
	} else {
		dev->platform_data->gpio1_ctrl(sd, 0);
		ret = dev->platform_data->gpio0_ctrl(sd, 0);
	}
	return ret;
}

static int power_up(struct v4l2_subdev *sd)
{
	struct ov2680_device *dev = to_ov2680_sensor(sd);
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret;

	if (!dev->platform_data) {
		dev_err(&client->dev,
			"no camera_sensor_platform_data");
		return -ENODEV;
	}

	if (dev->power_on)
		return 0; /* Already on */

	/* power control */
	ret = power_ctrl(sd, 1);
	if (ret)
		goto fail_power;

	/* according to DS, at least 5ms is needed between DOVDD and PWDN */
	usleep_range(5000, 6000);

	/* gpio ctrl */
	ret = gpio_ctrl(sd, 1);
	if (ret) {
		ret = gpio_ctrl(sd, 1);
		if (ret)
			goto fail_power;
	}

	/* flis clock control */
	ret = dev->platform_data->flisclk_ctrl(sd, 1);
	if (ret)
		goto fail_clk;

	/* according to DS, 20ms is needed between PWDN and i2c access */
	msleep(20);

	ret = ov2680_init_registers(sd);
	if (ret)
		goto fail_init_registers;

	ret = __ov2680_set_exposure(sd, dev->exposure, dev->gain, dev->digitgain);
	if (ret)
		goto fail_init_registers;

	dev->power_on = true;
	return 0;

fail_init_registers:
	dev->platform_data->flisclk_ctrl(sd, 0);
fail_clk:
	gpio_ctrl(sd, 0);
fail_power:
	power_ctrl(sd, 0);
	dev_err(&client->dev, "sensor power-up failed\n");

	return ret;
}

static int power_down(struct v4l2_subdev *sd)
{
	struct ov2680_device *dev = to_ov2680_sensor(sd);
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret = 0;

	h_flag = 0;
	v_flag = 0;
	if (!dev->platform_data) {
		dev_err(&client->dev,
			"no camera_sensor_platform_data");
		return -ENODEV;
	}

	if (!dev->power_on)
		return 0; /* Already off */

	ret = dev->platform_data->flisclk_ctrl(sd, 0);
	if (ret)
		dev_err(&client->dev, "flisclk failed\n");

	/* gpio ctrl */
	ret = gpio_ctrl(sd, 0);
	if (ret) {
		ret = gpio_ctrl(sd, 0);
		if (ret)
			dev_err(&client->dev, "gpio failed 2\n");
	}

	/* power control */
	ret = power_ctrl(sd, 0);
	if (ret) {
		dev_err(&client->dev, "vprog failed.\n");
		return ret;
	}

	dev->power_on = false;
	return 0;
}

static int ov2680_s_power(struct v4l2_subdev *sd, int on)
{
	struct ov2680_device *dev = to_ov2680_sensor(sd);
	int ret;

	mutex_lock(&dev->input_lock);

	if (on == 0) {
		ret = power_down(sd);
	} else {
		ret = power_up(sd);
	}

	mutex_unlock(&dev->input_lock);

	return ret;
>>>>>>> b7ba80a49124 (Commit)
}

static int ov2680_set_fmt(struct v4l2_subdev *sd,
			  struct v4l2_subdev_state *sd_state,
			  struct v4l2_subdev_format *format)
{
<<<<<<< HEAD
	struct ov2680_device *sensor = to_ov2680_sensor(sd);
	struct v4l2_mbus_framefmt *fmt;
	unsigned int width, height;

	width = min_t(unsigned int, ALIGN(format->format.width, 2), OV2680_NATIVE_WIDTH);
	height = min_t(unsigned int, ALIGN(format->format.height, 2), OV2680_NATIVE_HEIGHT);

	fmt = __ov2680_get_pad_format(sensor, sd_state, format->pad, format->which);
	ov2680_fill_format(sensor, fmt, width, height);

	format->format = *fmt;

	if (format->which == V4L2_SUBDEV_FORMAT_TRY)
		return 0;

	mutex_lock(&sensor->input_lock);
	ov2680_calc_mode(sensor, fmt->width, fmt->height);
	mutex_unlock(&sensor->input_lock);
	return 0;
=======
	struct v4l2_mbus_framefmt *fmt = &format->format;
	struct ov2680_device *dev = to_ov2680_sensor(sd);
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct camera_mipi_info *ov2680_info = NULL;
	struct ov2680_resolution *res;
	int vts, ret = 0;

	dev_dbg(&client->dev, "%s: %s: pad: %d, fmt: %p\n",
		__func__,
		(format->which == V4L2_SUBDEV_FORMAT_TRY) ? "try" : "set",
		format->pad, fmt);

	if (format->pad)
		return -EINVAL;

	if (!fmt)
		return -EINVAL;

	ov2680_info = v4l2_get_subdev_hostdata(sd);
	if (!ov2680_info)
		return -EINVAL;

	mutex_lock(&dev->input_lock);

	res = v4l2_find_nearest_size(ov2680_res_preview,
				     ARRAY_SIZE(ov2680_res_preview), width,
				     height, fmt->width, fmt->height);
	if (!res)
		res = &ov2680_res_preview[N_RES_PREVIEW - 1];

	fmt->width = res->width;
	fmt->height = res->height;

	fmt->code = MEDIA_BUS_FMT_SBGGR10_1X10;
	if (format->which == V4L2_SUBDEV_FORMAT_TRY) {
		sd_state->pads->try_fmt = *fmt;
		mutex_unlock(&dev->input_lock);
		return 0;
	}

	dev_dbg(&client->dev, "%s: %dx%d\n",
		__func__, fmt->width, fmt->height);

	/* s_power has not been called yet for std v4l2 clients (camorama) */
	power_up(sd);
	ret = ov2680_write_reg_array(client, dev->res->regs);
	if (ret)
		dev_err(&client->dev,
			"ov2680 write resolution register err: %d\n", ret);

	vts = dev->res->lines_per_frame;

	/* If necessary increase the VTS to match exposure + MARGIN */
	if (dev->exposure > vts - OV2680_INTEGRATION_TIME_MARGIN)
		vts = dev->exposure + OV2680_INTEGRATION_TIME_MARGIN;

	ret = ov2680_write_reg(client, 2, OV2680_TIMING_VTS_H, vts);
	if (ret)
		dev_err(&client->dev, "ov2680 write vts err: %d\n", ret);

	ret = ov2680_get_intg_factor(client, ov2680_info, res);
	if (ret) {
		dev_err(&client->dev, "failed to get integration factor\n");
		goto err;
	}

	/*
	 * recall flip functions to avoid flip registers
	 * were overridden by default setting
	 */
	if (h_flag)
		ov2680_h_flip(sd, h_flag);
	if (v_flag)
		ov2680_v_flip(sd, v_flag);

	/*
	 * ret = startup(sd);
	 * if (ret)
	 * dev_err(&client->dev, "ov2680 startup err\n");
	 */
err:
	mutex_unlock(&dev->input_lock);
	return ret;
>>>>>>> b7ba80a49124 (Commit)
}

static int ov2680_get_fmt(struct v4l2_subdev *sd,
			  struct v4l2_subdev_state *sd_state,
			  struct v4l2_subdev_format *format)
{
<<<<<<< HEAD
	struct ov2680_device *sensor = to_ov2680_sensor(sd);
	struct v4l2_mbus_framefmt *fmt;

	fmt = __ov2680_get_pad_format(sensor, sd_state, format->pad, format->which);
	format->format = *fmt;
=======
	struct v4l2_mbus_framefmt *fmt = &format->format;
	struct ov2680_device *dev = to_ov2680_sensor(sd);

	if (format->pad)
		return -EINVAL;

	if (!fmt)
		return -EINVAL;

	fmt->width = dev->res->width;
	fmt->height = dev->res->height;
	fmt->code = MEDIA_BUS_FMT_SBGGR10_1X10;

>>>>>>> b7ba80a49124 (Commit)
	return 0;
}

static int ov2680_detect(struct i2c_client *client)
{
	struct i2c_adapter *adapter = client->adapter;
	u32 high, low;
	int ret;
	u16 id;
	u8 revision;

	if (!i2c_check_functionality(adapter, I2C_FUNC_I2C))
		return -ENODEV;

<<<<<<< HEAD
	ret = ov_read_reg8(client, OV2680_SC_CMMN_CHIP_ID_H, &high);
=======
	ret = ov2680_read_reg(client, 1,
			      OV2680_SC_CMMN_CHIP_ID_H, &high);
>>>>>>> b7ba80a49124 (Commit)
	if (ret) {
		dev_err(&client->dev, "sensor_id_high = 0x%x\n", high);
		return -ENODEV;
	}
<<<<<<< HEAD
	ret = ov_read_reg8(client, OV2680_SC_CMMN_CHIP_ID_L, &low);
=======
	ret = ov2680_read_reg(client, 1,
			      OV2680_SC_CMMN_CHIP_ID_L, &low);
>>>>>>> b7ba80a49124 (Commit)
	id = ((((u16)high) << 8) | (u16)low);

	if (id != OV2680_ID) {
		dev_err(&client->dev, "sensor ID error 0x%x\n", id);
		return -ENODEV;
	}

<<<<<<< HEAD
	ret = ov_read_reg8(client, OV2680_SC_CMMN_SUB_ID, &high);
=======
	ret = ov2680_read_reg(client, 1,
			      OV2680_SC_CMMN_SUB_ID, &high);
>>>>>>> b7ba80a49124 (Commit)
	revision = (u8)high & 0x0f;

	dev_info(&client->dev, "sensor_revision id = 0x%x, rev= %d\n",
		 id, revision);

	return 0;
}

static int ov2680_s_stream(struct v4l2_subdev *sd, int enable)
{
<<<<<<< HEAD
	struct ov2680_device *sensor = to_ov2680_sensor(sd);
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret = 0;

	mutex_lock(&sensor->input_lock);

	if (sensor->is_streaming == enable) {
		dev_warn(&client->dev, "stream already %s\n", enable ? "started" : "stopped");
		goto error_unlock;
	}

	if (enable) {
		ret = pm_runtime_get_sync(sensor->sd.dev);
		if (ret < 0)
			goto error_unlock;

		ret = ov2680_set_mode(sensor);
		if (ret)
			goto error_power_down;

		/* Restore value of all ctrls */
		ret = __v4l2_ctrl_handler_setup(&sensor->ctrls.handler);
		if (ret)
			goto error_power_down;

		ret = ov_write_reg8(client, OV2680_SW_STREAM, OV2680_START_STREAMING);
		if (ret)
			goto error_power_down;
	} else {
		ov_write_reg8(client, OV2680_SW_STREAM, OV2680_STOP_STREAMING);
		pm_runtime_put(sensor->sd.dev);
	}

	sensor->is_streaming = enable;
	v4l2_ctrl_activate(sensor->ctrls.vflip, !enable);
	v4l2_ctrl_activate(sensor->ctrls.hflip, !enable);

	mutex_unlock(&sensor->input_lock);
	return 0;

error_power_down:
	pm_runtime_put(sensor->sd.dev);
error_unlock:
	mutex_unlock(&sensor->input_lock);
	return ret;
}

static int ov2680_s_config(struct v4l2_subdev *sd)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret;

	ret = pm_runtime_get_sync(&client->dev);
	if (ret < 0) {
=======
	struct ov2680_device *dev = to_ov2680_sensor(sd);
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret;

	mutex_lock(&dev->input_lock);
	if (enable)
		dev_dbg(&client->dev, "ov2680_s_stream one\n");
	else
		dev_dbg(&client->dev, "ov2680_s_stream off\n");

	ret = ov2680_write_reg(client, 1, OV2680_SW_STREAM,
			       enable ? OV2680_START_STREAMING :
			       OV2680_STOP_STREAMING);

	//otp valid at stream on state
	//if(!dev->otp_data)
	//	dev->otp_data = ov2680_otp_read(sd);

	mutex_unlock(&dev->input_lock);

	return ret;
}

static int ov2680_s_config(struct v4l2_subdev *sd,
			   int irq, void *platform_data)
{
	struct ov2680_device *dev = to_ov2680_sensor(sd);
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret = 0;

	if (!platform_data)
		return -ENODEV;

	dev->platform_data =
	    (struct camera_sensor_platform_data *)platform_data;

	mutex_lock(&dev->input_lock);

	ret = power_up(sd);
	if (ret) {
>>>>>>> b7ba80a49124 (Commit)
		dev_err(&client->dev, "ov2680 power-up err.\n");
		goto fail_power_on;
	}

<<<<<<< HEAD
	/* config & detect sensor */
	ret = ov2680_detect(client);
	if (ret)
		dev_err(&client->dev, "ov2680_detect err s_config.\n");

fail_power_on:
	pm_runtime_put(&client->dev);
=======
	ret = dev->platform_data->csi_cfg(sd, 1);
	if (ret)
		goto fail_csi_cfg;

	/* config & detect sensor */
	ret = ov2680_detect(client);
	if (ret) {
		dev_err(&client->dev, "ov2680_detect err s_config.\n");
		goto fail_csi_cfg;
	}

	/* turn off sensor, after probed */
	ret = power_down(sd);
	if (ret) {
		dev_err(&client->dev, "ov2680 power-off err.\n");
		goto fail_csi_cfg;
	}
	mutex_unlock(&dev->input_lock);

	return 0;

fail_csi_cfg:
	dev->platform_data->csi_cfg(sd, 0);
fail_power_on:
	power_down(sd);
	dev_err(&client->dev, "sensor power-gating failed\n");
	mutex_unlock(&dev->input_lock);
>>>>>>> b7ba80a49124 (Commit)
	return ret;
}

static int ov2680_g_frame_interval(struct v4l2_subdev *sd,
				   struct v4l2_subdev_frame_interval *interval)
{
<<<<<<< HEAD
	interval->interval.numerator = 1;
	interval->interval.denominator = OV2680_FPS;
=======
	struct ov2680_device *dev = to_ov2680_sensor(sd);

	interval->interval.numerator = 1;
	interval->interval.denominator = dev->res->fps;

>>>>>>> b7ba80a49124 (Commit)
	return 0;
}

static int ov2680_enum_mbus_code(struct v4l2_subdev *sd,
				 struct v4l2_subdev_state *sd_state,
				 struct v4l2_subdev_mbus_code_enum *code)
{
<<<<<<< HEAD
	/* We support only a single format */
	if (code->index)
=======
	if (code->index >= MAX_FMTS)
>>>>>>> b7ba80a49124 (Commit)
		return -EINVAL;

	code->code = MEDIA_BUS_FMT_SBGGR10_1X10;
	return 0;
}

static int ov2680_enum_frame_size(struct v4l2_subdev *sd,
				  struct v4l2_subdev_state *sd_state,
				  struct v4l2_subdev_frame_size_enum *fse)
{
<<<<<<< HEAD
	static const struct v4l2_frmsize_discrete ov2680_frame_sizes[] = {
		{ 1616, 1216 },
		{ 1616, 1096 },
		{ 1616,  916 },
		{ 1456, 1096 },
		{ 1296,  976 },
		{ 1296,  736 },
		{  784,  592 },
		{  656,  496 },
	};
	int index = fse->index;

	if (index >= ARRAY_SIZE(ov2680_frame_sizes))
		return -EINVAL;

	fse->min_width = ov2680_frame_sizes[index].width;
	fse->min_height = ov2680_frame_sizes[index].height;
	fse->max_width = ov2680_frame_sizes[index].width;
	fse->max_height = ov2680_frame_sizes[index].height;
=======
	int index = fse->index;

	if (index >= N_RES_PREVIEW)
		return -EINVAL;

	fse->min_width = ov2680_res_preview[index].width;
	fse->min_height = ov2680_res_preview[index].height;
	fse->max_width = ov2680_res_preview[index].width;
	fse->max_height = ov2680_res_preview[index].height;
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

static int ov2680_enum_frame_interval(struct v4l2_subdev *sd,
				      struct v4l2_subdev_state *sd_state,
				      struct v4l2_subdev_frame_interval_enum *fie)
{
<<<<<<< HEAD
	/* Only 1 framerate */
	if (fie->index)
		return -EINVAL;

	fie->interval.numerator = 1;
	fie->interval.denominator = OV2680_FPS;
=======
	struct v4l2_fract fract;

	if (fie->index >= N_RES_PREVIEW ||
	    fie->width > ov2680_res_preview[0].width ||
	    fie->height > ov2680_res_preview[0].height ||
	    fie->which > V4L2_SUBDEV_FORMAT_ACTIVE)
		return -EINVAL;

	fract.denominator = ov2680_res_preview[fie->index].fps;
	fract.numerator = 1;

	fie->interval = fract;

>>>>>>> b7ba80a49124 (Commit)
	return 0;
}

static int ov2680_g_skip_frames(struct v4l2_subdev *sd, u32 *frames)
{
<<<<<<< HEAD
	*frames = OV2680_SKIP_FRAMES;
=======
	struct ov2680_device *dev = to_ov2680_sensor(sd);

	mutex_lock(&dev->input_lock);
	*frames = dev->res->skip_frames;
	mutex_unlock(&dev->input_lock);

>>>>>>> b7ba80a49124 (Commit)
	return 0;
}

static const struct v4l2_subdev_video_ops ov2680_video_ops = {
	.s_stream = ov2680_s_stream,
	.g_frame_interval = ov2680_g_frame_interval,
};

static const struct v4l2_subdev_sensor_ops ov2680_sensor_ops = {
	.g_skip_frames	= ov2680_g_skip_frames,
};

<<<<<<< HEAD
=======
static const struct v4l2_subdev_core_ops ov2680_core_ops = {
	.s_power = ov2680_s_power,
	.ioctl = ov2680_ioctl,
};

>>>>>>> b7ba80a49124 (Commit)
static const struct v4l2_subdev_pad_ops ov2680_pad_ops = {
	.enum_mbus_code = ov2680_enum_mbus_code,
	.enum_frame_size = ov2680_enum_frame_size,
	.enum_frame_interval = ov2680_enum_frame_interval,
	.get_fmt = ov2680_get_fmt,
	.set_fmt = ov2680_set_fmt,
};

static const struct v4l2_subdev_ops ov2680_ops = {
<<<<<<< HEAD
=======
	.core = &ov2680_core_ops,
>>>>>>> b7ba80a49124 (Commit)
	.video = &ov2680_video_ops,
	.pad = &ov2680_pad_ops,
	.sensor = &ov2680_sensor_ops,
};

<<<<<<< HEAD
static int ov2680_init_controls(struct ov2680_device *sensor)
{
	static const char * const test_pattern_menu[] = {
		"Disabled",
		"Color Bars",
		"Random Data",
		"Square",
		"Black Image",
	};
	const struct v4l2_ctrl_ops *ops = &ov2680_ctrl_ops;
	struct ov2680_ctrls *ctrls = &sensor->ctrls;
	struct v4l2_ctrl_handler *hdl = &ctrls->handler;
	int exp_max = OV2680_LINES_PER_FRAME - OV2680_INTEGRATION_TIME_MARGIN;

	v4l2_ctrl_handler_init(hdl, 4);

	hdl->lock = &sensor->input_lock;

	ctrls->hflip = v4l2_ctrl_new_std(hdl, ops, V4L2_CID_HFLIP, 0, 1, 1, 0);
	ctrls->vflip = v4l2_ctrl_new_std(hdl, ops, V4L2_CID_VFLIP, 0, 1, 1, 0);
	ctrls->exposure = v4l2_ctrl_new_std(hdl, ops, V4L2_CID_EXPOSURE,
					    0, exp_max, 1, exp_max);
	ctrls->gain = v4l2_ctrl_new_std(hdl, ops, V4L2_CID_GAIN, 0, 1023, 1, 250);
	ctrls->test_pattern =
		v4l2_ctrl_new_std_menu_items(hdl,
					     &ov2680_ctrl_ops, V4L2_CID_TEST_PATTERN,
					     ARRAY_SIZE(test_pattern_menu) - 1,
					     0, 0, test_pattern_menu);

	ctrls->hflip->flags |= V4L2_CTRL_FLAG_MODIFY_LAYOUT;
	ctrls->vflip->flags |= V4L2_CTRL_FLAG_MODIFY_LAYOUT;

	if (hdl->error)
		return hdl->error;

	sensor->sd.ctrl_handler = hdl;
	return 0;
}

static void ov2680_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct ov2680_device *sensor = to_ov2680_sensor(sd);

	dev_dbg(&client->dev, "ov2680_remove...\n");

	atomisp_unregister_subdev(sd);
	v4l2_device_unregister_subdev(sd);
	media_entity_cleanup(&sensor->sd.entity);
	v4l2_ctrl_handler_free(&sensor->ctrls.handler);
	pm_runtime_disable(&client->dev);
}

/*
 * Unlike other sensors which have both a rest and powerdown input pins,
 * the OV2680 only has a powerdown input. But some ACPI tables still list
 * 2 GPIOs for the OV2680 and it is unclear which to use. So try to get
 * up to 2 GPIOs (1 mandatory, 1 optional) and control them in sync.
 */
static const struct acpi_gpio_params ov2680_first_gpio = { 0, 0, true };
static const struct acpi_gpio_params ov2680_second_gpio = { 1, 0, true };

static const struct acpi_gpio_mapping ov2680_gpio_mapping[] = {
	{ "powerdown-gpios", &ov2680_first_gpio, 1 },
	{ "powerdown-alt-gpios", &ov2680_second_gpio, 1 },
	{ },
};

static int ov2680_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct ov2680_device *sensor;
	int ret;

	sensor = devm_kzalloc(dev, sizeof(*sensor), GFP_KERNEL);
	if (!sensor)
		return -ENOMEM;

	mutex_init(&sensor->input_lock);

	sensor->client = client;
	v4l2_i2c_subdev_init(&sensor->sd, client, &ov2680_ops);

	ret = devm_acpi_dev_add_driver_gpios(&client->dev, ov2680_gpio_mapping);
	if (ret)
		return ret;

	sensor->powerdown = devm_gpiod_get(dev, "powerdown", GPIOD_OUT_HIGH);
	if (IS_ERR(sensor->powerdown))
		return dev_err_probe(dev, PTR_ERR(sensor->powerdown), "getting powerdown GPIO\n");

	sensor->powerdown_alt = devm_gpiod_get_optional(dev, "powerdown-alt", GPIOD_OUT_HIGH);
	if (IS_ERR(sensor->powerdown_alt))
		return dev_err_probe(dev, PTR_ERR(sensor->powerdown_alt), "getting powerdown-alt GPIO\n");

	pm_runtime_set_suspended(dev);
	pm_runtime_enable(dev);
	pm_runtime_set_autosuspend_delay(dev, 1000);
	pm_runtime_use_autosuspend(dev);

	ret = ov2680_s_config(&sensor->sd);
	if (ret)
		return ret;

	sensor->sd.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	sensor->pad.flags = MEDIA_PAD_FL_SOURCE;
	sensor->sd.entity.function = MEDIA_ENT_F_CAM_SENSOR;

	ret = ov2680_init_controls(sensor);
=======
static void ov2680_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct ov2680_device *dev = to_ov2680_sensor(sd);

	dev_dbg(&client->dev, "ov2680_remove...\n");

	dev->platform_data->csi_cfg(sd, 0);

	v4l2_device_unregister_subdev(sd);
	media_entity_cleanup(&dev->sd.entity);
	v4l2_ctrl_handler_free(&dev->ctrl_handler);
	kfree(dev);
}

static int ov2680_probe(struct i2c_client *client)
{
	struct ov2680_device *dev;
	int ret;
	void *pdata;
	unsigned int i;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	mutex_init(&dev->input_lock);

	dev->res = &ov2680_res_preview[0];
	dev->exposure = dev->res->lines_per_frame - OV2680_INTEGRATION_TIME_MARGIN;
	dev->gain = 250; /* 0-2047 */
	v4l2_i2c_subdev_init(&dev->sd, client, &ov2680_ops);

	pdata = gmin_camera_platform_data(&dev->sd,
					  ATOMISP_INPUT_FORMAT_RAW_10,
					  atomisp_bayer_order_bggr);
	if (!pdata) {
		ret = -EINVAL;
		goto out_free;
	}

	ret = ov2680_s_config(&dev->sd, client->irq, pdata);
	if (ret)
		goto out_free;

	ret = atomisp_register_i2c_module(&dev->sd, pdata, RAW_CAMERA);
	if (ret)
		goto out_free;

	dev->sd.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	dev->pad.flags = MEDIA_PAD_FL_SOURCE;
	dev->sd.entity.function = MEDIA_ENT_F_CAM_SENSOR;
	ret =
	    v4l2_ctrl_handler_init(&dev->ctrl_handler,
				   ARRAY_SIZE(ov2680_controls));
>>>>>>> b7ba80a49124 (Commit)
	if (ret) {
		ov2680_remove(client);
		return ret;
	}

<<<<<<< HEAD
	ret = media_entity_pads_init(&sensor->sd.entity, 1, &sensor->pad);
	if (ret) {
		ov2680_remove(client);
		return ret;
	}

	ov2680_fill_format(sensor, &sensor->mode.fmt, OV2680_NATIVE_WIDTH, OV2680_NATIVE_HEIGHT);

	ret = atomisp_register_sensor_no_gmin(&sensor->sd, 1, ATOMISP_INPUT_FORMAT_RAW_10,
					      atomisp_bayer_order_bggr);
	if (ret) {
		ov2680_remove(client);
		return ret;
	}

	return 0;
}

static int ov2680_suspend(struct device *dev)
{
	struct v4l2_subdev *sd = dev_get_drvdata(dev);
	struct ov2680_device *sensor = to_ov2680_sensor(sd);

	gpiod_set_value_cansleep(sensor->powerdown, 1);
	gpiod_set_value_cansleep(sensor->powerdown_alt, 1);
	return 0;
}

static int ov2680_resume(struct device *dev)
{
	struct v4l2_subdev *sd = dev_get_drvdata(dev);
	struct ov2680_device *sensor = to_ov2680_sensor(sd);

	/* according to DS, at least 5ms is needed after DOVDD (enabled by ACPI) */
	usleep_range(5000, 6000);

	gpiod_set_value_cansleep(sensor->powerdown, 0);
	gpiod_set_value_cansleep(sensor->powerdown_alt, 0);

	/* according to DS, 20ms is needed between PWDN and i2c access */
	msleep(20);

	ov2680_init_registers(sd);
	return 0;
}

static DEFINE_RUNTIME_DEV_PM_OPS(ov2680_pm_ops, ov2680_suspend, ov2680_resume, NULL);

=======
	for (i = 0; i < ARRAY_SIZE(ov2680_controls); i++)
		v4l2_ctrl_new_custom(&dev->ctrl_handler, &ov2680_controls[i],
				     NULL);

	if (dev->ctrl_handler.error) {
		ov2680_remove(client);
		return dev->ctrl_handler.error;
	}

	/* Use same lock for controls as for everything else. */
	dev->ctrl_handler.lock = &dev->input_lock;
	dev->sd.ctrl_handler = &dev->ctrl_handler;

	ret = media_entity_pads_init(&dev->sd.entity, 1, &dev->pad);
	if (ret) {
		ov2680_remove(client);
		dev_dbg(&client->dev, "+++ remove ov2680\n");
	}
	return ret;
out_free:
	dev_dbg(&client->dev, "+++ out free\n");
	v4l2_device_unregister_subdev(&dev->sd);
	kfree(dev);
	return ret;
}

>>>>>>> b7ba80a49124 (Commit)
static const struct acpi_device_id ov2680_acpi_match[] = {
	{"XXOV2680"},
	{"OVTI2680"},
	{},
};
MODULE_DEVICE_TABLE(acpi, ov2680_acpi_match);

static struct i2c_driver ov2680_driver = {
	.driver = {
		.name = "ov2680",
<<<<<<< HEAD
		.pm = pm_sleep_ptr(&ov2680_pm_ops),
=======
>>>>>>> b7ba80a49124 (Commit)
		.acpi_match_table = ov2680_acpi_match,
	},
	.probe_new = ov2680_probe,
	.remove = ov2680_remove,
};
module_i2c_driver(ov2680_driver);

MODULE_AUTHOR("Jacky Wang <Jacky_wang@ovt.com>");
MODULE_DESCRIPTION("A low-level driver for OmniVision 2680 sensors");
MODULE_LICENSE("GPL");
