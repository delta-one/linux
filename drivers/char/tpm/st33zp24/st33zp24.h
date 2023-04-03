/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * STMicroelectronics TPM Linux driver for TPM ST33ZP24
 * Copyright (C) 2009 - 2016  STMicroelectronics
 */

#ifndef __LOCAL_ST33ZP24_H__
#define __LOCAL_ST33ZP24_H__

<<<<<<< HEAD
#define TPM_ST33_I2C		"st33zp24-i2c"
#define TPM_ST33_SPI		"st33zp24-spi"

=======
>>>>>>> b7ba80a49124 (Commit)
#define TPM_WRITE_DIRECTION	0x80
#define ST33ZP24_BUFSIZE	2048

struct st33zp24_dev {
	struct tpm_chip *chip;
	void *phy_id;
	const struct st33zp24_phy_ops *ops;
	int locality;
	int irq;
	u32 intrs;
<<<<<<< HEAD
	struct gpio_desc *io_lpcpd;
=======
	int io_lpcpd;
>>>>>>> b7ba80a49124 (Commit)
	wait_queue_head_t read_queue;
};


struct st33zp24_phy_ops {
	int (*send)(void *phy_id, u8 tpm_register, u8 *tpm_data, int tpm_size);
	int (*recv)(void *phy_id, u8 tpm_register, u8 *tpm_data, int tpm_size);
};

#ifdef CONFIG_PM_SLEEP
int st33zp24_pm_suspend(struct device *dev);
int st33zp24_pm_resume(struct device *dev);
#endif

int st33zp24_probe(void *phy_id, const struct st33zp24_phy_ops *ops,
<<<<<<< HEAD
		   struct device *dev, int irq);
=======
		   struct device *dev, int irq, int io_lpcpd);
>>>>>>> b7ba80a49124 (Commit)
void st33zp24_remove(struct tpm_chip *chip);
#endif /* __LOCAL_ST33ZP24_H__ */
