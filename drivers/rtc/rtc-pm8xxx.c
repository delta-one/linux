// SPDX-License-Identifier: GPL-2.0-only
<<<<<<< HEAD
/*
 * pm8xxx RTC driver
 *
 * Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 * Copyright (c) 2023, Linaro Limited
 */
#include <linux/of.h>
#include <linux/module.h>
#include <linux/nvmem-consumer.h>
=======
/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 */
#include <linux/of.h>
#include <linux/module.h>
>>>>>>> b7ba80a49124 (Commit)
#include <linux/init.h>
#include <linux/rtc.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/pm_wakeirq.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

<<<<<<< HEAD
#include <asm/unaligned.h>
=======
/* RTC Register offsets from RTC CTRL REG */
#define PM8XXX_ALARM_CTRL_OFFSET	0x01
#define PM8XXX_RTC_WRITE_OFFSET		0x02
#define PM8XXX_RTC_READ_OFFSET		0x06
#define PM8XXX_ALARM_RW_OFFSET		0x0A
>>>>>>> b7ba80a49124 (Commit)

/* RTC_CTRL register bit fields */
#define PM8xxx_RTC_ENABLE		BIT(7)
#define PM8xxx_RTC_ALARM_CLEAR		BIT(0)
#define PM8xxx_RTC_ALARM_ENABLE		BIT(7)

#define NUM_8_BIT_RTC_REGS		0x4

/**
 * struct pm8xxx_rtc_regs - describe RTC registers per PMIC versions
<<<<<<< HEAD
 * @ctrl:		address of control register
 * @write:		base address of write registers
 * @read:		base address of read registers
 * @alarm_ctrl:		address of alarm control register
 * @alarm_ctrl2:	address of alarm control2 register
 * @alarm_rw:		base address of alarm read-write registers
 * @alarm_en:		alarm enable mask
=======
 * @ctrl: base address of control register
 * @write: base address of write register
 * @read: base address of read register
 * @alarm_ctrl: base address of alarm control register
 * @alarm_ctrl2: base address of alarm control2 register
 * @alarm_rw: base address of alarm read-write register
 * @alarm_en: alarm enable mask
>>>>>>> b7ba80a49124 (Commit)
 */
struct pm8xxx_rtc_regs {
	unsigned int ctrl;
	unsigned int write;
	unsigned int read;
	unsigned int alarm_ctrl;
	unsigned int alarm_ctrl2;
	unsigned int alarm_rw;
	unsigned int alarm_en;
};

/**
<<<<<<< HEAD
 * struct pm8xxx_rtc -  RTC driver internal structure
 * @rtc:		RTC device
 * @regmap:		regmap used to access registers
 * @allow_set_time:	whether the time can be set
 * @alarm_irq:		alarm irq number
 * @regs:		register description
 * @dev:		device structure
 * @nvmem_cell:		nvmem cell for offset
 * @offset:		offset from epoch in seconds
=======
 * struct pm8xxx_rtc -  rtc driver internal structure
 * @rtc:		rtc device for this driver.
 * @regmap:		regmap used to access RTC registers
 * @allow_set_time:	indicates whether writing to the RTC is allowed
 * @rtc_alarm_irq:	rtc alarm irq number.
 * @regs:		rtc registers description.
 * @rtc_dev:		device structure.
 * @ctrl_reg_lock:	spinlock protecting access to ctrl_reg.
>>>>>>> b7ba80a49124 (Commit)
 */
struct pm8xxx_rtc {
	struct rtc_device *rtc;
	struct regmap *regmap;
	bool allow_set_time;
<<<<<<< HEAD
	int alarm_irq;
	const struct pm8xxx_rtc_regs *regs;
	struct device *dev;
	struct nvmem_cell *nvmem_cell;
	u32 offset;
};

static int pm8xxx_rtc_read_nvmem_offset(struct pm8xxx_rtc *rtc_dd)
{
	size_t len;
	void *buf;
	int rc;

	buf = nvmem_cell_read(rtc_dd->nvmem_cell, &len);
	if (IS_ERR(buf)) {
		rc = PTR_ERR(buf);
		dev_dbg(rtc_dd->dev, "failed to read nvmem offset: %d\n", rc);
		return rc;
	}

	if (len != sizeof(u32)) {
		dev_dbg(rtc_dd->dev, "unexpected nvmem cell size %zu\n", len);
		kfree(buf);
		return -EINVAL;
	}

	rtc_dd->offset = get_unaligned_le32(buf);

	kfree(buf);

	return 0;
}

static int pm8xxx_rtc_write_nvmem_offset(struct pm8xxx_rtc *rtc_dd, u32 offset)
{
	u8 buf[sizeof(u32)];
	int rc;

	put_unaligned_le32(offset, buf);

	rc = nvmem_cell_write(rtc_dd->nvmem_cell, buf, sizeof(buf));
	if (rc < 0) {
		dev_dbg(rtc_dd->dev, "failed to write nvmem offset: %d\n", rc);
		return rc;
	}

	return 0;
}

static int pm8xxx_rtc_read_offset(struct pm8xxx_rtc *rtc_dd)
{
	if (!rtc_dd->nvmem_cell)
		return 0;

	return pm8xxx_rtc_read_nvmem_offset(rtc_dd);
}

static int pm8xxx_rtc_read_raw(struct pm8xxx_rtc *rtc_dd, u32 *secs)
{
	const struct pm8xxx_rtc_regs *regs = rtc_dd->regs;
	u8 value[NUM_8_BIT_RTC_REGS];
	unsigned int reg;
	int rc;

	rc = regmap_bulk_read(rtc_dd->regmap, regs->read, value, sizeof(value));
	if (rc)
		return rc;

	/*
	 * Read the LSB again and check if there has been a carry over.
	 * If there has, redo the read operation.
	 */
	rc = regmap_read(rtc_dd->regmap, regs->read, &reg);
	if (rc < 0)
		return rc;

	if (reg < value[0]) {
		rc = regmap_bulk_read(rtc_dd->regmap, regs->read, value,
				      sizeof(value));
		if (rc)
			return rc;
	}

	*secs = get_unaligned_le32(value);

	return 0;
}

static int pm8xxx_rtc_update_offset(struct pm8xxx_rtc *rtc_dd, u32 secs)
{
	u32 raw_secs;
	u32 offset;
	int rc;

	if (!rtc_dd->nvmem_cell)
		return -ENODEV;

	rc = pm8xxx_rtc_read_raw(rtc_dd, &raw_secs);
	if (rc)
		return rc;

	offset = secs - raw_secs;

	if (offset == rtc_dd->offset)
		return 0;

	rc = pm8xxx_rtc_write_nvmem_offset(rtc_dd, offset);
	if (rc)
		return rc;

	rtc_dd->offset = offset;

	return 0;
}

=======
	int rtc_alarm_irq;
	const struct pm8xxx_rtc_regs *regs;
	struct device *rtc_dev;
	spinlock_t ctrl_reg_lock;
};

>>>>>>> b7ba80a49124 (Commit)
/*
 * Steps to write the RTC registers.
 * 1. Disable alarm if enabled.
 * 2. Disable rtc if enabled.
 * 3. Write 0x00 to LSB.
 * 4. Write Byte[1], Byte[2], Byte[3] then Byte[0].
 * 5. Enable rtc if disabled in step 2.
 * 6. Enable alarm if disabled in step 1.
 */
<<<<<<< HEAD
static int __pm8xxx_rtc_set_time(struct pm8xxx_rtc *rtc_dd, u32 secs)
{
	const struct pm8xxx_rtc_regs *regs = rtc_dd->regs;
	u8 value[NUM_8_BIT_RTC_REGS];
	bool alarm_enabled;
	int rc;

	put_unaligned_le32(secs, value);

	rc = regmap_update_bits_check(rtc_dd->regmap, regs->alarm_ctrl,
				      regs->alarm_en, 0, &alarm_enabled);
	if (rc)
		return rc;

	/* Disable RTC */
	rc = regmap_update_bits(rtc_dd->regmap, regs->ctrl, PM8xxx_RTC_ENABLE, 0);
	if (rc)
		return rc;

	/* Write 0 to Byte[0] */
	rc = regmap_write(rtc_dd->regmap, regs->write, 0);
	if (rc)
		return rc;
=======
static int pm8xxx_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	int rc, i;
	unsigned long secs, irq_flags;
	u8 value[NUM_8_BIT_RTC_REGS], alarm_enabled = 0, rtc_disabled = 0;
	unsigned int ctrl_reg, rtc_ctrl_reg;
	struct pm8xxx_rtc *rtc_dd = dev_get_drvdata(dev);
	const struct pm8xxx_rtc_regs *regs = rtc_dd->regs;

	if (!rtc_dd->allow_set_time)
		return -ENODEV;

	secs = rtc_tm_to_time64(tm);

	dev_dbg(dev, "Seconds value to be written to RTC = %lu\n", secs);

	for (i = 0; i < NUM_8_BIT_RTC_REGS; i++) {
		value[i] = secs & 0xFF;
		secs >>= 8;
	}

	spin_lock_irqsave(&rtc_dd->ctrl_reg_lock, irq_flags);

	rc = regmap_read(rtc_dd->regmap, regs->alarm_ctrl, &ctrl_reg);
	if (rc)
		goto rtc_rw_fail;

	if (ctrl_reg & regs->alarm_en) {
		alarm_enabled = 1;
		ctrl_reg &= ~regs->alarm_en;
		rc = regmap_write(rtc_dd->regmap, regs->alarm_ctrl, ctrl_reg);
		if (rc) {
			dev_err(dev, "Write to RTC Alarm control register failed\n");
			goto rtc_rw_fail;
		}
	}

	/* Disable RTC H/w before writing on RTC register */
	rc = regmap_read(rtc_dd->regmap, regs->ctrl, &rtc_ctrl_reg);
	if (rc)
		goto rtc_rw_fail;

	if (rtc_ctrl_reg & PM8xxx_RTC_ENABLE) {
		rtc_disabled = 1;
		rtc_ctrl_reg &= ~PM8xxx_RTC_ENABLE;
		rc = regmap_write(rtc_dd->regmap, regs->ctrl, rtc_ctrl_reg);
		if (rc) {
			dev_err(dev, "Write to RTC control register failed\n");
			goto rtc_rw_fail;
		}
	}

	/* Write 0 to Byte[0] */
	rc = regmap_write(rtc_dd->regmap, regs->write, 0);
	if (rc) {
		dev_err(dev, "Write to RTC write data register failed\n");
		goto rtc_rw_fail;
	}
>>>>>>> b7ba80a49124 (Commit)

	/* Write Byte[1], Byte[2], Byte[3] */
	rc = regmap_bulk_write(rtc_dd->regmap, regs->write + 1,
			       &value[1], sizeof(value) - 1);
<<<<<<< HEAD
	if (rc)
		return rc;

	/* Write Byte[0] */
	rc = regmap_write(rtc_dd->regmap, regs->write, value[0]);
	if (rc)
		return rc;

	/* Enable RTC */
	rc = regmap_update_bits(rtc_dd->regmap, regs->ctrl, PM8xxx_RTC_ENABLE,
				PM8xxx_RTC_ENABLE);
	if (rc)
		return rc;

	if (alarm_enabled) {
		rc = regmap_update_bits(rtc_dd->regmap, regs->alarm_ctrl,
					regs->alarm_en, regs->alarm_en);
		if (rc)
			return rc;
	}

	return 0;
}

static int pm8xxx_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	struct pm8xxx_rtc *rtc_dd = dev_get_drvdata(dev);
	u32 secs;
	int rc;

	secs = rtc_tm_to_time64(tm);

	if (rtc_dd->allow_set_time)
		rc = __pm8xxx_rtc_set_time(rtc_dd, secs);
	else
		rc = pm8xxx_rtc_update_offset(rtc_dd, secs);

	if (rc)
		return rc;

	dev_dbg(dev, "set time: %ptRd %ptRt (%u + %u)\n", tm, tm,
			secs - rtc_dd->offset, rtc_dd->offset);
	return 0;
=======
	if (rc) {
		dev_err(dev, "Write to RTC write data register failed\n");
		goto rtc_rw_fail;
	}

	/* Write Byte[0] */
	rc = regmap_write(rtc_dd->regmap, regs->write, value[0]);
	if (rc) {
		dev_err(dev, "Write to RTC write data register failed\n");
		goto rtc_rw_fail;
	}

	/* Enable RTC H/w after writing on RTC register */
	if (rtc_disabled) {
		rtc_ctrl_reg |= PM8xxx_RTC_ENABLE;
		rc = regmap_write(rtc_dd->regmap, regs->ctrl, rtc_ctrl_reg);
		if (rc) {
			dev_err(dev, "Write to RTC control register failed\n");
			goto rtc_rw_fail;
		}
	}

	if (alarm_enabled) {
		ctrl_reg |= regs->alarm_en;
		rc = regmap_write(rtc_dd->regmap, regs->alarm_ctrl, ctrl_reg);
		if (rc) {
			dev_err(dev, "Write to RTC Alarm control register failed\n");
			goto rtc_rw_fail;
		}
	}

rtc_rw_fail:
	spin_unlock_irqrestore(&rtc_dd->ctrl_reg_lock, irq_flags);

	return rc;
>>>>>>> b7ba80a49124 (Commit)
}

static int pm8xxx_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
<<<<<<< HEAD
	struct pm8xxx_rtc *rtc_dd = dev_get_drvdata(dev);
	u32 secs;
	int rc;

	rc = pm8xxx_rtc_read_raw(rtc_dd, &secs);
	if (rc)
		return rc;

	secs += rtc_dd->offset;
	rtc_time64_to_tm(secs, tm);

	dev_dbg(dev, "read time: %ptRd %ptRt (%u + %u)\n", tm, tm,
			secs - rtc_dd->offset, rtc_dd->offset);
=======
	int rc;
	u8 value[NUM_8_BIT_RTC_REGS];
	unsigned long secs;
	unsigned int reg;
	struct pm8xxx_rtc *rtc_dd = dev_get_drvdata(dev);
	const struct pm8xxx_rtc_regs *regs = rtc_dd->regs;

	rc = regmap_bulk_read(rtc_dd->regmap, regs->read, value, sizeof(value));
	if (rc) {
		dev_err(dev, "RTC read data register failed\n");
		return rc;
	}

	/*
	 * Read the LSB again and check if there has been a carry over.
	 * If there is, redo the read operation.
	 */
	rc = regmap_read(rtc_dd->regmap, regs->read, &reg);
	if (rc < 0) {
		dev_err(dev, "RTC read data register failed\n");
		return rc;
	}

	if (unlikely(reg < value[0])) {
		rc = regmap_bulk_read(rtc_dd->regmap, regs->read,
				      value, sizeof(value));
		if (rc) {
			dev_err(dev, "RTC read data register failed\n");
			return rc;
		}
	}

	secs = value[0] | (value[1] << 8) | (value[2] << 16) |
	       ((unsigned long)value[3] << 24);

	rtc_time64_to_tm(secs, tm);

	dev_dbg(dev, "secs = %lu, h:m:s == %ptRt, y-m-d = %ptRdr\n", secs, tm, tm);

>>>>>>> b7ba80a49124 (Commit)
	return 0;
}

static int pm8xxx_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
<<<<<<< HEAD
	struct pm8xxx_rtc *rtc_dd = dev_get_drvdata(dev);
	const struct pm8xxx_rtc_regs *regs = rtc_dd->regs;
	u8 value[NUM_8_BIT_RTC_REGS];
	u32 secs;
	int rc;

	secs = rtc_tm_to_time64(&alarm->time);
	secs -= rtc_dd->offset;
	put_unaligned_le32(secs, value);

	rc = regmap_update_bits(rtc_dd->regmap, regs->alarm_ctrl,
				regs->alarm_en, 0);
	if (rc)
		return rc;

	rc = regmap_bulk_write(rtc_dd->regmap, regs->alarm_rw, value,
			       sizeof(value));
	if (rc)
		return rc;

	if (alarm->enabled) {
		rc = regmap_update_bits(rtc_dd->regmap, regs->alarm_ctrl,
					regs->alarm_en, regs->alarm_en);
		if (rc)
			return rc;
	}

	dev_dbg(dev, "set alarm: %ptRd %ptRt\n", &alarm->time, &alarm->time);

	return 0;
=======
	int rc, i;
	u8 value[NUM_8_BIT_RTC_REGS];
	unsigned int ctrl_reg;
	unsigned long secs, irq_flags;
	struct pm8xxx_rtc *rtc_dd = dev_get_drvdata(dev);
	const struct pm8xxx_rtc_regs *regs = rtc_dd->regs;

	secs = rtc_tm_to_time64(&alarm->time);

	for (i = 0; i < NUM_8_BIT_RTC_REGS; i++) {
		value[i] = secs & 0xFF;
		secs >>= 8;
	}

	spin_lock_irqsave(&rtc_dd->ctrl_reg_lock, irq_flags);

	rc = regmap_bulk_write(rtc_dd->regmap, regs->alarm_rw, value,
			       sizeof(value));
	if (rc) {
		dev_err(dev, "Write to RTC ALARM register failed\n");
		goto rtc_rw_fail;
	}

	rc = regmap_read(rtc_dd->regmap, regs->alarm_ctrl, &ctrl_reg);
	if (rc)
		goto rtc_rw_fail;

	if (alarm->enabled)
		ctrl_reg |= regs->alarm_en;
	else
		ctrl_reg &= ~regs->alarm_en;

	rc = regmap_write(rtc_dd->regmap, regs->alarm_ctrl, ctrl_reg);
	if (rc) {
		dev_err(dev, "Write to RTC alarm control register failed\n");
		goto rtc_rw_fail;
	}

	dev_dbg(dev, "Alarm Set for h:m:s=%ptRt, y-m-d=%ptRdr\n",
		&alarm->time, &alarm->time);
rtc_rw_fail:
	spin_unlock_irqrestore(&rtc_dd->ctrl_reg_lock, irq_flags);
	return rc;
>>>>>>> b7ba80a49124 (Commit)
}

static int pm8xxx_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
<<<<<<< HEAD
	struct pm8xxx_rtc *rtc_dd = dev_get_drvdata(dev);
	const struct pm8xxx_rtc_regs *regs = rtc_dd->regs;
	u8 value[NUM_8_BIT_RTC_REGS];
	unsigned int ctrl_reg;
	u32 secs;
	int rc;

	rc = regmap_bulk_read(rtc_dd->regmap, regs->alarm_rw, value,
			      sizeof(value));
	if (rc)
		return rc;

	secs = get_unaligned_le32(value);
	secs += rtc_dd->offset;
	rtc_time64_to_tm(secs, &alarm->time);

	rc = regmap_read(rtc_dd->regmap, regs->alarm_ctrl, &ctrl_reg);
	if (rc)
		return rc;

	alarm->enabled = !!(ctrl_reg & PM8xxx_RTC_ALARM_ENABLE);

	dev_dbg(dev, "read alarm: %ptRd %ptRt\n", &alarm->time, &alarm->time);
=======
	int rc;
	unsigned int ctrl_reg;
	u8 value[NUM_8_BIT_RTC_REGS];
	unsigned long secs;
	struct pm8xxx_rtc *rtc_dd = dev_get_drvdata(dev);
	const struct pm8xxx_rtc_regs *regs = rtc_dd->regs;

	rc = regmap_bulk_read(rtc_dd->regmap, regs->alarm_rw, value,
			      sizeof(value));
	if (rc) {
		dev_err(dev, "RTC alarm time read failed\n");
		return rc;
	}

	secs = value[0] | (value[1] << 8) | (value[2] << 16) |
	       ((unsigned long)value[3] << 24);

	rtc_time64_to_tm(secs, &alarm->time);

	rc = regmap_read(rtc_dd->regmap, regs->alarm_ctrl, &ctrl_reg);
	if (rc) {
		dev_err(dev, "Read from RTC alarm control register failed\n");
		return rc;
	}
	alarm->enabled = !!(ctrl_reg & PM8xxx_RTC_ALARM_ENABLE);

	dev_dbg(dev, "Alarm set for - h:m:s=%ptRt, y-m-d=%ptRdr\n",
		&alarm->time, &alarm->time);
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

static int pm8xxx_rtc_alarm_irq_enable(struct device *dev, unsigned int enable)
{
<<<<<<< HEAD
	struct pm8xxx_rtc *rtc_dd = dev_get_drvdata(dev);
	const struct pm8xxx_rtc_regs *regs = rtc_dd->regs;
	u8 value[NUM_8_BIT_RTC_REGS] = {0};
	unsigned int val;
	int rc;

	if (enable)
		val = regs->alarm_en;
	else
		val = 0;

	rc = regmap_update_bits(rtc_dd->regmap, regs->alarm_ctrl,
				regs->alarm_en, val);
	if (rc)
		return rc;

	/* Clear alarm register */
	if (!enable) {
		rc = regmap_bulk_write(rtc_dd->regmap, regs->alarm_rw, value,
				       sizeof(value));
		if (rc)
			return rc;
	}

	return 0;
=======
	int rc;
	unsigned long irq_flags;
	struct pm8xxx_rtc *rtc_dd = dev_get_drvdata(dev);
	const struct pm8xxx_rtc_regs *regs = rtc_dd->regs;
	unsigned int ctrl_reg;
	u8 value[NUM_8_BIT_RTC_REGS] = {0};

	spin_lock_irqsave(&rtc_dd->ctrl_reg_lock, irq_flags);

	rc = regmap_read(rtc_dd->regmap, regs->alarm_ctrl, &ctrl_reg);
	if (rc)
		goto rtc_rw_fail;

	if (enable)
		ctrl_reg |= regs->alarm_en;
	else
		ctrl_reg &= ~regs->alarm_en;

	rc = regmap_write(rtc_dd->regmap, regs->alarm_ctrl, ctrl_reg);
	if (rc) {
		dev_err(dev, "Write to RTC control register failed\n");
		goto rtc_rw_fail;
	}

	/* Clear Alarm register */
	if (!enable) {
		rc = regmap_bulk_write(rtc_dd->regmap, regs->alarm_rw, value,
				       sizeof(value));
		if (rc) {
			dev_err(dev, "Clear RTC ALARM register failed\n");
			goto rtc_rw_fail;
		}
	}

rtc_rw_fail:
	spin_unlock_irqrestore(&rtc_dd->ctrl_reg_lock, irq_flags);
	return rc;
>>>>>>> b7ba80a49124 (Commit)
}

static const struct rtc_class_ops pm8xxx_rtc_ops = {
	.read_time	= pm8xxx_rtc_read_time,
	.set_time	= pm8xxx_rtc_set_time,
	.set_alarm	= pm8xxx_rtc_set_alarm,
	.read_alarm	= pm8xxx_rtc_read_alarm,
	.alarm_irq_enable = pm8xxx_rtc_alarm_irq_enable,
};

static irqreturn_t pm8xxx_alarm_trigger(int irq, void *dev_id)
{
	struct pm8xxx_rtc *rtc_dd = dev_id;
	const struct pm8xxx_rtc_regs *regs = rtc_dd->regs;
<<<<<<< HEAD
=======
	unsigned int ctrl_reg;
>>>>>>> b7ba80a49124 (Commit)
	int rc;

	rtc_update_irq(rtc_dd->rtc, 1, RTC_IRQF | RTC_AF);

<<<<<<< HEAD
	/* Disable alarm */
	rc = regmap_update_bits(rtc_dd->regmap, regs->alarm_ctrl,
				regs->alarm_en, 0);
	if (rc)
		return IRQ_NONE;

	/* Clear alarm status */
	rc = regmap_update_bits(rtc_dd->regmap, regs->alarm_ctrl2,
				PM8xxx_RTC_ALARM_CLEAR, 0);
	if (rc)
		return IRQ_NONE;

=======
	spin_lock(&rtc_dd->ctrl_reg_lock);

	/* Clear the alarm enable bit */
	rc = regmap_read(rtc_dd->regmap, regs->alarm_ctrl, &ctrl_reg);
	if (rc) {
		spin_unlock(&rtc_dd->ctrl_reg_lock);
		goto rtc_alarm_handled;
	}

	ctrl_reg &= ~regs->alarm_en;

	rc = regmap_write(rtc_dd->regmap, regs->alarm_ctrl, ctrl_reg);
	if (rc) {
		spin_unlock(&rtc_dd->ctrl_reg_lock);
		dev_err(rtc_dd->rtc_dev,
			"Write to alarm control register failed\n");
		goto rtc_alarm_handled;
	}

	spin_unlock(&rtc_dd->ctrl_reg_lock);

	/* Clear RTC alarm register */
	rc = regmap_read(rtc_dd->regmap, regs->alarm_ctrl2, &ctrl_reg);
	if (rc) {
		dev_err(rtc_dd->rtc_dev,
			"RTC Alarm control2 register read failed\n");
		goto rtc_alarm_handled;
	}

	ctrl_reg |= PM8xxx_RTC_ALARM_CLEAR;
	rc = regmap_write(rtc_dd->regmap, regs->alarm_ctrl2, ctrl_reg);
	if (rc)
		dev_err(rtc_dd->rtc_dev,
			"Write to RTC Alarm control2 register failed\n");

rtc_alarm_handled:
>>>>>>> b7ba80a49124 (Commit)
	return IRQ_HANDLED;
}

static int pm8xxx_rtc_enable(struct pm8xxx_rtc *rtc_dd)
{
	const struct pm8xxx_rtc_regs *regs = rtc_dd->regs;
<<<<<<< HEAD

	return regmap_update_bits(rtc_dd->regmap, regs->ctrl, PM8xxx_RTC_ENABLE,
				  PM8xxx_RTC_ENABLE);
=======
	unsigned int ctrl_reg;
	int rc;

	/* Check if the RTC is on, else turn it on */
	rc = regmap_read(rtc_dd->regmap, regs->ctrl, &ctrl_reg);
	if (rc)
		return rc;

	if (!(ctrl_reg & PM8xxx_RTC_ENABLE)) {
		ctrl_reg |= PM8xxx_RTC_ENABLE;
		rc = regmap_write(rtc_dd->regmap, regs->ctrl, ctrl_reg);
		if (rc)
			return rc;
	}

	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

static const struct pm8xxx_rtc_regs pm8921_regs = {
	.ctrl		= 0x11d,
	.write		= 0x11f,
	.read		= 0x123,
	.alarm_rw	= 0x127,
	.alarm_ctrl	= 0x11d,
	.alarm_ctrl2	= 0x11e,
	.alarm_en	= BIT(1),
};

static const struct pm8xxx_rtc_regs pm8058_regs = {
	.ctrl		= 0x1e8,
	.write		= 0x1ea,
	.read		= 0x1ee,
	.alarm_rw	= 0x1f2,
	.alarm_ctrl	= 0x1e8,
	.alarm_ctrl2	= 0x1e9,
	.alarm_en	= BIT(1),
};

static const struct pm8xxx_rtc_regs pm8941_regs = {
	.ctrl		= 0x6046,
	.write		= 0x6040,
	.read		= 0x6048,
	.alarm_rw	= 0x6140,
	.alarm_ctrl	= 0x6146,
	.alarm_ctrl2	= 0x6148,
	.alarm_en	= BIT(7),
};

static const struct pm8xxx_rtc_regs pmk8350_regs = {
	.ctrl		= 0x6146,
	.write		= 0x6140,
	.read		= 0x6148,
	.alarm_rw	= 0x6240,
	.alarm_ctrl	= 0x6246,
	.alarm_ctrl2	= 0x6248,
	.alarm_en	= BIT(7),
};

<<<<<<< HEAD
static const struct of_device_id pm8xxx_id_table[] = {
	{ .compatible = "qcom,pm8921-rtc", .data = &pm8921_regs },
=======
/*
 * Hardcoded RTC bases until IORESOURCE_REG mapping is figured out
 */
static const struct of_device_id pm8xxx_id_table[] = {
	{ .compatible = "qcom,pm8921-rtc", .data = &pm8921_regs },
	{ .compatible = "qcom,pm8018-rtc", .data = &pm8921_regs },
>>>>>>> b7ba80a49124 (Commit)
	{ .compatible = "qcom,pm8058-rtc", .data = &pm8058_regs },
	{ .compatible = "qcom,pm8941-rtc", .data = &pm8941_regs },
	{ .compatible = "qcom,pmk8350-rtc", .data = &pmk8350_regs },
	{ },
};
MODULE_DEVICE_TABLE(of, pm8xxx_id_table);

static int pm8xxx_rtc_probe(struct platform_device *pdev)
{
<<<<<<< HEAD
	const struct of_device_id *match;
	struct pm8xxx_rtc *rtc_dd;
	int rc;
=======
	int rc;
	struct pm8xxx_rtc *rtc_dd;
	const struct of_device_id *match;
>>>>>>> b7ba80a49124 (Commit)

	match = of_match_node(pm8xxx_id_table, pdev->dev.of_node);
	if (!match)
		return -ENXIO;

	rtc_dd = devm_kzalloc(&pdev->dev, sizeof(*rtc_dd), GFP_KERNEL);
	if (rtc_dd == NULL)
		return -ENOMEM;

<<<<<<< HEAD
	rtc_dd->regmap = dev_get_regmap(pdev->dev.parent, NULL);
	if (!rtc_dd->regmap)
		return -ENXIO;

	rtc_dd->alarm_irq = platform_get_irq(pdev, 0);
	if (rtc_dd->alarm_irq < 0)
=======
	/* Initialise spinlock to protect RTC control register */
	spin_lock_init(&rtc_dd->ctrl_reg_lock);

	rtc_dd->regmap = dev_get_regmap(pdev->dev.parent, NULL);
	if (!rtc_dd->regmap) {
		dev_err(&pdev->dev, "Parent regmap unavailable.\n");
		return -ENXIO;
	}

	rtc_dd->rtc_alarm_irq = platform_get_irq(pdev, 0);
	if (rtc_dd->rtc_alarm_irq < 0)
>>>>>>> b7ba80a49124 (Commit)
		return -ENXIO;

	rtc_dd->allow_set_time = of_property_read_bool(pdev->dev.of_node,
						      "allow-set-time");

<<<<<<< HEAD
	rtc_dd->nvmem_cell = devm_nvmem_cell_get(&pdev->dev, "offset");
	if (IS_ERR(rtc_dd->nvmem_cell)) {
		rc = PTR_ERR(rtc_dd->nvmem_cell);
		if (rc != -ENOENT)
			return rc;
		rtc_dd->nvmem_cell = NULL;
	}

	rtc_dd->regs = match->data;
	rtc_dd->dev = &pdev->dev;

	if (!rtc_dd->allow_set_time) {
		rc = pm8xxx_rtc_read_offset(rtc_dd);
		if (rc)
			return rc;
	}
=======
	rtc_dd->regs = match->data;
	rtc_dd->rtc_dev = &pdev->dev;
>>>>>>> b7ba80a49124 (Commit)

	rc = pm8xxx_rtc_enable(rtc_dd);
	if (rc)
		return rc;

	platform_set_drvdata(pdev, rtc_dd);

	device_init_wakeup(&pdev->dev, 1);

<<<<<<< HEAD
=======
	/* Register the RTC device */
>>>>>>> b7ba80a49124 (Commit)
	rtc_dd->rtc = devm_rtc_allocate_device(&pdev->dev);
	if (IS_ERR(rtc_dd->rtc))
		return PTR_ERR(rtc_dd->rtc);

	rtc_dd->rtc->ops = &pm8xxx_rtc_ops;
	rtc_dd->rtc->range_max = U32_MAX;

<<<<<<< HEAD
	rc = devm_request_any_context_irq(&pdev->dev, rtc_dd->alarm_irq,
					  pm8xxx_alarm_trigger,
					  IRQF_TRIGGER_RISING,
					  "pm8xxx_rtc_alarm", rtc_dd);
	if (rc < 0)
		return rc;
=======
	/* Request the alarm IRQ */
	rc = devm_request_any_context_irq(&pdev->dev, rtc_dd->rtc_alarm_irq,
					  pm8xxx_alarm_trigger,
					  IRQF_TRIGGER_RISING,
					  "pm8xxx_rtc_alarm", rtc_dd);
	if (rc < 0) {
		dev_err(&pdev->dev, "Request IRQ failed (%d)\n", rc);
		return rc;
	}
>>>>>>> b7ba80a49124 (Commit)

	rc = devm_rtc_register_device(rtc_dd->rtc);
	if (rc)
		return rc;

<<<<<<< HEAD
	rc = dev_pm_set_wake_irq(&pdev->dev, rtc_dd->alarm_irq);
=======
	rc = dev_pm_set_wake_irq(&pdev->dev, rtc_dd->rtc_alarm_irq);
>>>>>>> b7ba80a49124 (Commit)
	if (rc)
		return rc;

	return 0;
}

<<<<<<< HEAD
static void pm8xxx_remove(struct platform_device *pdev)
{
	dev_pm_clear_wake_irq(&pdev->dev);
=======
static int pm8xxx_remove(struct platform_device *pdev)
{
	dev_pm_clear_wake_irq(&pdev->dev);
	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

static struct platform_driver pm8xxx_rtc_driver = {
	.probe		= pm8xxx_rtc_probe,
<<<<<<< HEAD
	.remove_new	= pm8xxx_remove,
=======
	.remove		= pm8xxx_remove,
>>>>>>> b7ba80a49124 (Commit)
	.driver	= {
		.name		= "rtc-pm8xxx",
		.of_match_table	= pm8xxx_id_table,
	},
};

module_platform_driver(pm8xxx_rtc_driver);

MODULE_ALIAS("platform:rtc-pm8xxx");
MODULE_DESCRIPTION("PMIC8xxx RTC driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Anirudh Ghayal <aghayal@codeaurora.org>");
<<<<<<< HEAD
MODULE_AUTHOR("Johan Hovold <johan@kernel.org>");
=======
>>>>>>> b7ba80a49124 (Commit)
