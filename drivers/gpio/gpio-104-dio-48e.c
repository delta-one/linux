// SPDX-License-Identifier: GPL-2.0-only
/*
 * GPIO driver for the ACCES 104-DIO-48E series
 * Copyright (C) 2016 William Breathitt Gray
 *
 * This driver supports the following ACCES devices: 104-DIO-48E and
 * 104-DIO-24E.
 */
#include <linux/bits.h>
#include <linux/device.h>
<<<<<<< HEAD
#include <linux/err.h>
#include <linux/ioport.h>
#include <linux/irq.h>
=======
#include <linux/errno.h>
#include <linux/gpio/driver.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/irqdesc.h>
>>>>>>> b7ba80a49124 (Commit)
#include <linux/isa.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
<<<<<<< HEAD
#include <linux/regmap.h>
=======
#include <linux/spinlock.h>
>>>>>>> b7ba80a49124 (Commit)
#include <linux/types.h>

#include "gpio-i8255.h"

MODULE_IMPORT_NS(I8255);

#define DIO48E_EXTENT 16
#define MAX_NUM_DIO48E max_num_isa_dev(DIO48E_EXTENT)

static unsigned int base[MAX_NUM_DIO48E];
static unsigned int num_dio48e;
module_param_hw_array(base, uint, ioport, &num_dio48e, 0);
MODULE_PARM_DESC(base, "ACCES 104-DIO-48E base addresses");

static unsigned int irq[MAX_NUM_DIO48E];
static unsigned int num_irq;
module_param_hw_array(irq, uint, irq, &num_irq, 0);
MODULE_PARM_DESC(irq, "ACCES 104-DIO-48E interrupt line numbers");

<<<<<<< HEAD
#define DIO48E_ENABLE_INTERRUPT 0xB
#define DIO48E_DISABLE_INTERRUPT DIO48E_ENABLE_INTERRUPT
#define DIO48E_CLEAR_INTERRUPT 0xF

#define DIO48E_NUM_PPI 2

static const struct regmap_range dio48e_wr_ranges[] = {
	regmap_reg_range(0x0, 0x9), regmap_reg_range(0xB, 0xB),
	regmap_reg_range(0xD, 0xD), regmap_reg_range(0xF, 0xF),
};
static const struct regmap_range dio48e_rd_ranges[] = {
	regmap_reg_range(0x0, 0x2), regmap_reg_range(0x4, 0x6),
	regmap_reg_range(0xB, 0xB), regmap_reg_range(0xD, 0xD),
	regmap_reg_range(0xF, 0xF),
};
static const struct regmap_range dio48e_volatile_ranges[] = {
	i8255_volatile_regmap_range(0x0), i8255_volatile_regmap_range(0x4),
	regmap_reg_range(0xB, 0xB), regmap_reg_range(0xD, 0xD),
	regmap_reg_range(0xF, 0xF),
};
static const struct regmap_range dio48e_precious_ranges[] = {
	regmap_reg_range(0xB, 0xB), regmap_reg_range(0xD, 0xD),
	regmap_reg_range(0xF, 0xF),
};
static const struct regmap_access_table dio48e_wr_table = {
	.yes_ranges = dio48e_wr_ranges,
	.n_yes_ranges = ARRAY_SIZE(dio48e_wr_ranges),
};
static const struct regmap_access_table dio48e_rd_table = {
	.yes_ranges = dio48e_rd_ranges,
	.n_yes_ranges = ARRAY_SIZE(dio48e_rd_ranges),
};
static const struct regmap_access_table dio48e_volatile_table = {
	.yes_ranges = dio48e_volatile_ranges,
	.n_yes_ranges = ARRAY_SIZE(dio48e_volatile_ranges),
};
static const struct regmap_access_table dio48e_precious_table = {
	.yes_ranges = dio48e_precious_ranges,
	.n_yes_ranges = ARRAY_SIZE(dio48e_precious_ranges),
};
static const struct regmap_config dio48e_regmap_config = {
	.reg_bits = 8,
	.reg_stride = 1,
	.val_bits = 8,
	.io_port = true,
	.max_register = 0xF,
	.wr_table = &dio48e_wr_table,
	.rd_table = &dio48e_rd_table,
	.volatile_table = &dio48e_volatile_table,
	.precious_table = &dio48e_precious_table,
	.cache_type = REGCACHE_FLAT,
};

/* only bit 3 on each respective Port C supports interrupts */
#define DIO48E_REGMAP_IRQ(_ppi)						\
	[19 + (_ppi) * 24] = {						\
		.mask = BIT(_ppi),					\
		.type = { .types_supported = IRQ_TYPE_EDGE_RISING },	\
	}

static const struct regmap_irq dio48e_regmap_irqs[] = {
	DIO48E_REGMAP_IRQ(0), DIO48E_REGMAP_IRQ(1),
};

static int dio48e_handle_mask_sync(struct regmap *const map, const int index,
				   const unsigned int mask_buf_def,
				   const unsigned int mask_buf,
				   void *const irq_drv_data)
{
	unsigned int *const irq_mask = irq_drv_data;
	const unsigned int prev_mask = *irq_mask;
	int err;
	unsigned int val;

	/* exit early if no change since the previous mask */
	if (mask_buf == prev_mask)
		return 0;

	/* remember the current mask for the next mask sync */
	*irq_mask = mask_buf;

	/* if all previously masked, enable interrupts when unmasking */
	if (prev_mask == mask_buf_def) {
		err = regmap_write(map, DIO48E_CLEAR_INTERRUPT, 0x00);
		if (err)
			return err;
		return regmap_write(map, DIO48E_ENABLE_INTERRUPT, 0x00);
	}

	/* if all are currently masked, disable interrupts */
	if (mask_buf == mask_buf_def)
		return regmap_read(map, DIO48E_DISABLE_INTERRUPT, &val);
=======
#define DIO48E_NUM_PPI 2

/**
 * struct dio48e_reg - device register structure
 * @ppi:		Programmable Peripheral Interface groups
 * @enable_buffer:	Enable/Disable Buffer groups
 * @unused1:		Unused
 * @enable_interrupt:	Write: Enable Interrupt
 *			Read: Disable Interrupt
 * @unused2:		Unused
 * @enable_counter:	Write: Enable Counter/Timer Addressing
 *			Read: Disable Counter/Timer Addressing
 * @unused3:		Unused
 * @clear_interrupt:	Clear Interrupt
 */
struct dio48e_reg {
	struct i8255 ppi[DIO48E_NUM_PPI];
	u8 enable_buffer[DIO48E_NUM_PPI];
	u8 unused1;
	u8 enable_interrupt;
	u8 unused2;
	u8 enable_counter;
	u8 unused3;
	u8 clear_interrupt;
};

/**
 * struct dio48e_gpio - GPIO device private data structure
 * @chip:		instance of the gpio_chip
 * @ppi_state:		PPI device states
 * @lock:		synchronization lock to prevent I/O race conditions
 * @reg:		I/O address offset for the device registers
 * @irq_mask:		I/O bits affected by interrupts
 */
struct dio48e_gpio {
	struct gpio_chip chip;
	struct i8255_state ppi_state[DIO48E_NUM_PPI];
	raw_spinlock_t lock;
	struct dio48e_reg __iomem *reg;
	unsigned char irq_mask;
};

static int dio48e_gpio_get_direction(struct gpio_chip *chip, unsigned int offset)
{
	struct dio48e_gpio *const dio48egpio = gpiochip_get_data(chip);

	if (i8255_get_direction(dio48egpio->ppi_state, offset))
		return GPIO_LINE_DIRECTION_IN;

	return GPIO_LINE_DIRECTION_OUT;
}

static int dio48e_gpio_direction_input(struct gpio_chip *chip, unsigned int offset)
{
	struct dio48e_gpio *const dio48egpio = gpiochip_get_data(chip);

	i8255_direction_input(dio48egpio->reg->ppi, dio48egpio->ppi_state,
			      offset);
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

<<<<<<< HEAD
=======
static int dio48e_gpio_direction_output(struct gpio_chip *chip, unsigned int offset,
					int value)
{
	struct dio48e_gpio *const dio48egpio = gpiochip_get_data(chip);

	i8255_direction_output(dio48egpio->reg->ppi, dio48egpio->ppi_state,
			       offset, value);

	return 0;
}

static int dio48e_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	struct dio48e_gpio *const dio48egpio = gpiochip_get_data(chip);

	return i8255_get(dio48egpio->reg->ppi, offset);
}

static int dio48e_gpio_get_multiple(struct gpio_chip *chip, unsigned long *mask,
	unsigned long *bits)
{
	struct dio48e_gpio *const dio48egpio = gpiochip_get_data(chip);

	i8255_get_multiple(dio48egpio->reg->ppi, mask, bits, chip->ngpio);

	return 0;
}

static void dio48e_gpio_set(struct gpio_chip *chip, unsigned int offset, int value)
{
	struct dio48e_gpio *const dio48egpio = gpiochip_get_data(chip);

	i8255_set(dio48egpio->reg->ppi, dio48egpio->ppi_state, offset, value);
}

static void dio48e_gpio_set_multiple(struct gpio_chip *chip,
	unsigned long *mask, unsigned long *bits)
{
	struct dio48e_gpio *const dio48egpio = gpiochip_get_data(chip);

	i8255_set_multiple(dio48egpio->reg->ppi, dio48egpio->ppi_state, mask,
			   bits, chip->ngpio);
}

static void dio48e_irq_ack(struct irq_data *data)
{
}

static void dio48e_irq_mask(struct irq_data *data)
{
	struct gpio_chip *chip = irq_data_get_irq_chip_data(data);
	struct dio48e_gpio *const dio48egpio = gpiochip_get_data(chip);
	const unsigned long offset = irqd_to_hwirq(data);
	unsigned long flags;

	/* only bit 3 on each respective Port C supports interrupts */
	if (offset != 19 && offset != 43)
		return;

	raw_spin_lock_irqsave(&dio48egpio->lock, flags);

	if (offset == 19)
		dio48egpio->irq_mask &= ~BIT(0);
	else
		dio48egpio->irq_mask &= ~BIT(1);
	gpiochip_disable_irq(chip, offset);

	if (!dio48egpio->irq_mask)
		/* disable interrupts */
		ioread8(&dio48egpio->reg->enable_interrupt);

	raw_spin_unlock_irqrestore(&dio48egpio->lock, flags);
}

static void dio48e_irq_unmask(struct irq_data *data)
{
	struct gpio_chip *chip = irq_data_get_irq_chip_data(data);
	struct dio48e_gpio *const dio48egpio = gpiochip_get_data(chip);
	const unsigned long offset = irqd_to_hwirq(data);
	unsigned long flags;

	/* only bit 3 on each respective Port C supports interrupts */
	if (offset != 19 && offset != 43)
		return;

	raw_spin_lock_irqsave(&dio48egpio->lock, flags);

	if (!dio48egpio->irq_mask) {
		/* enable interrupts */
		iowrite8(0x00, &dio48egpio->reg->clear_interrupt);
		iowrite8(0x00, &dio48egpio->reg->enable_interrupt);
	}

	gpiochip_enable_irq(chip, offset);
	if (offset == 19)
		dio48egpio->irq_mask |= BIT(0);
	else
		dio48egpio->irq_mask |= BIT(1);

	raw_spin_unlock_irqrestore(&dio48egpio->lock, flags);
}

static int dio48e_irq_set_type(struct irq_data *data, unsigned int flow_type)
{
	const unsigned long offset = irqd_to_hwirq(data);

	/* only bit 3 on each respective Port C supports interrupts */
	if (offset != 19 && offset != 43)
		return -EINVAL;

	if (flow_type != IRQ_TYPE_NONE && flow_type != IRQ_TYPE_EDGE_RISING)
		return -EINVAL;

	return 0;
}

static const struct irq_chip dio48e_irqchip = {
	.name = "104-dio-48e",
	.irq_ack = dio48e_irq_ack,
	.irq_mask = dio48e_irq_mask,
	.irq_unmask = dio48e_irq_unmask,
	.irq_set_type = dio48e_irq_set_type,
	.flags = IRQCHIP_IMMUTABLE,
	GPIOCHIP_IRQ_RESOURCE_HELPERS,
};

static irqreturn_t dio48e_irq_handler(int irq, void *dev_id)
{
	struct dio48e_gpio *const dio48egpio = dev_id;
	struct gpio_chip *const chip = &dio48egpio->chip;
	const unsigned long irq_mask = dio48egpio->irq_mask;
	unsigned long gpio;

	for_each_set_bit(gpio, &irq_mask, 2)
		generic_handle_domain_irq(chip->irq.domain,
					  19 + gpio*24);

	raw_spin_lock(&dio48egpio->lock);

	iowrite8(0x00, &dio48egpio->reg->clear_interrupt);

	raw_spin_unlock(&dio48egpio->lock);

	return IRQ_HANDLED;
}

>>>>>>> b7ba80a49124 (Commit)
#define DIO48E_NGPIO 48
static const char *dio48e_names[DIO48E_NGPIO] = {
	"PPI Group 0 Port A 0", "PPI Group 0 Port A 1", "PPI Group 0 Port A 2",
	"PPI Group 0 Port A 3", "PPI Group 0 Port A 4", "PPI Group 0 Port A 5",
	"PPI Group 0 Port A 6", "PPI Group 0 Port A 7",	"PPI Group 0 Port B 0",
	"PPI Group 0 Port B 1", "PPI Group 0 Port B 2", "PPI Group 0 Port B 3",
	"PPI Group 0 Port B 4", "PPI Group 0 Port B 5", "PPI Group 0 Port B 6",
	"PPI Group 0 Port B 7", "PPI Group 0 Port C 0", "PPI Group 0 Port C 1",
	"PPI Group 0 Port C 2", "PPI Group 0 Port C 3", "PPI Group 0 Port C 4",
	"PPI Group 0 Port C 5", "PPI Group 0 Port C 6", "PPI Group 0 Port C 7",
	"PPI Group 1 Port A 0", "PPI Group 1 Port A 1", "PPI Group 1 Port A 2",
	"PPI Group 1 Port A 3", "PPI Group 1 Port A 4", "PPI Group 1 Port A 5",
	"PPI Group 1 Port A 6", "PPI Group 1 Port A 7",	"PPI Group 1 Port B 0",
	"PPI Group 1 Port B 1", "PPI Group 1 Port B 2", "PPI Group 1 Port B 3",
	"PPI Group 1 Port B 4", "PPI Group 1 Port B 5", "PPI Group 1 Port B 6",
	"PPI Group 1 Port B 7", "PPI Group 1 Port C 0", "PPI Group 1 Port C 1",
	"PPI Group 1 Port C 2", "PPI Group 1 Port C 3", "PPI Group 1 Port C 4",
	"PPI Group 1 Port C 5", "PPI Group 1 Port C 6", "PPI Group 1 Port C 7"
};

<<<<<<< HEAD
static int dio48e_irq_init_hw(struct regmap *const map)
{
	unsigned int val;

	/* Disable IRQ by default */
	return regmap_read(map, DIO48E_DISABLE_INTERRUPT, &val);
=======
static int dio48e_irq_init_hw(struct gpio_chip *gc)
{
	struct dio48e_gpio *const dio48egpio = gpiochip_get_data(gc);

	/* Disable IRQ by default */
	ioread8(&dio48egpio->reg->enable_interrupt);

	return 0;
}

static void dio48e_init_ppi(struct i8255 __iomem *const ppi,
			    struct i8255_state *const ppi_state)
{
	const unsigned long ngpio = 24;
	const unsigned long mask = GENMASK(ngpio - 1, 0);
	const unsigned long bits = 0;
	unsigned long i;

	/* Initialize all GPIO to output 0 */
	for (i = 0; i < DIO48E_NUM_PPI; i++) {
		i8255_mode0_output(&ppi[i]);
		i8255_set_multiple(&ppi[i], &ppi_state[i], &mask, &bits, ngpio);
	}
>>>>>>> b7ba80a49124 (Commit)
}

static int dio48e_probe(struct device *dev, unsigned int id)
{
<<<<<<< HEAD
	const char *const name = dev_name(dev);
	struct i8255_regmap_config config = {};
	void __iomem *regs;
	struct regmap *map;
	int err;
	struct regmap_irq_chip *chip;
	unsigned int irq_mask;
	struct regmap_irq_chip_data *chip_data;
=======
	struct dio48e_gpio *dio48egpio;
	const char *const name = dev_name(dev);
	struct gpio_irq_chip *girq;
	int err;

	dio48egpio = devm_kzalloc(dev, sizeof(*dio48egpio), GFP_KERNEL);
	if (!dio48egpio)
		return -ENOMEM;
>>>>>>> b7ba80a49124 (Commit)

	if (!devm_request_region(dev, base[id], DIO48E_EXTENT, name)) {
		dev_err(dev, "Unable to lock port addresses (0x%X-0x%X)\n",
			base[id], base[id] + DIO48E_EXTENT);
		return -EBUSY;
	}

<<<<<<< HEAD
	regs = devm_ioport_map(dev, base[id], DIO48E_EXTENT);
	if (!regs)
		return -ENOMEM;

	map = devm_regmap_init_mmio(dev, regs, &dio48e_regmap_config);
	if (IS_ERR(map))
		return dev_err_probe(dev, PTR_ERR(map),
				     "Unable to initialize register map\n");

	chip = devm_kzalloc(dev, sizeof(*chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	chip->irq_drv_data = devm_kzalloc(dev, sizeof(irq_mask), GFP_KERNEL);
	if (!chip->irq_drv_data)
		return -ENOMEM;

	chip->name = name;
	chip->mask_base = DIO48E_ENABLE_INTERRUPT;
	chip->ack_base = DIO48E_CLEAR_INTERRUPT;
	chip->no_status = true;
	chip->num_regs = 1;
	chip->irqs = dio48e_regmap_irqs;
	chip->num_irqs = ARRAY_SIZE(dio48e_regmap_irqs);
	chip->handle_mask_sync = dio48e_handle_mask_sync;

	/* Initialize to prevent spurious interrupts before we're ready */
	err = dio48e_irq_init_hw(map);
	if (err)
		return err;

	err = devm_regmap_add_irq_chip(dev, map, irq[id], 0, 0, chip, &chip_data);
	if (err)
		return dev_err_probe(dev, err, "IRQ registration failed\n");

	config.parent = dev;
	config.map = map;
	config.num_ppi = DIO48E_NUM_PPI;
	config.names = dio48e_names;
	config.domain = regmap_irq_get_domain(chip_data);

	return devm_i8255_regmap_register(dev, &config);
=======
	dio48egpio->reg = devm_ioport_map(dev, base[id], DIO48E_EXTENT);
	if (!dio48egpio->reg)
		return -ENOMEM;

	dio48egpio->chip.label = name;
	dio48egpio->chip.parent = dev;
	dio48egpio->chip.owner = THIS_MODULE;
	dio48egpio->chip.base = -1;
	dio48egpio->chip.ngpio = DIO48E_NGPIO;
	dio48egpio->chip.names = dio48e_names;
	dio48egpio->chip.get_direction = dio48e_gpio_get_direction;
	dio48egpio->chip.direction_input = dio48e_gpio_direction_input;
	dio48egpio->chip.direction_output = dio48e_gpio_direction_output;
	dio48egpio->chip.get = dio48e_gpio_get;
	dio48egpio->chip.get_multiple = dio48e_gpio_get_multiple;
	dio48egpio->chip.set = dio48e_gpio_set;
	dio48egpio->chip.set_multiple = dio48e_gpio_set_multiple;

	girq = &dio48egpio->chip.irq;
	gpio_irq_chip_set_chip(girq, &dio48e_irqchip);
	/* This will let us handle the parent IRQ in the driver */
	girq->parent_handler = NULL;
	girq->num_parents = 0;
	girq->parents = NULL;
	girq->default_type = IRQ_TYPE_NONE;
	girq->handler = handle_edge_irq;
	girq->init_hw = dio48e_irq_init_hw;

	raw_spin_lock_init(&dio48egpio->lock);

	i8255_state_init(dio48egpio->ppi_state, DIO48E_NUM_PPI);
	dio48e_init_ppi(dio48egpio->reg->ppi, dio48egpio->ppi_state);

	err = devm_gpiochip_add_data(dev, &dio48egpio->chip, dio48egpio);
	if (err) {
		dev_err(dev, "GPIO registering failed (%d)\n", err);
		return err;
	}

	err = devm_request_irq(dev, irq[id], dio48e_irq_handler, 0, name,
		dio48egpio);
	if (err) {
		dev_err(dev, "IRQ handler registering failed (%d)\n", err);
		return err;
	}

	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

static struct isa_driver dio48e_driver = {
	.probe = dio48e_probe,
	.driver = {
		.name = "104-dio-48e"
	},
};
module_isa_driver_with_irq(dio48e_driver, num_dio48e, num_irq);

MODULE_AUTHOR("William Breathitt Gray <vilhelm.gray@gmail.com>");
MODULE_DESCRIPTION("ACCES 104-DIO-48E GPIO driver");
MODULE_LICENSE("GPL v2");
