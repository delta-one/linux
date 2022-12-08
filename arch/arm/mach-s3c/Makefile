# SPDX-License-Identifier: GPL-2.0
#
# Copyright 2009 Simtec Electronics

<<<<<<< HEAD
include $(srctree)/$(src)/Makefile.s3c64xx
=======
ifdef CONFIG_ARCH_S3C24XX
include $(src)/Makefile.s3c24xx
endif

ifdef CONFIG_ARCH_S3C64XX
include $(src)/Makefile.s3c64xx
endif
>>>>>>> b7ba80a49124 (Commit)

# Objects we always build independent of SoC choice

obj-y				+= init.o cpu.o

<<<<<<< HEAD
# devices

obj-$(CONFIG_SAMSUNG_ATAGS)	+= platformdata.o
=======
# ADC

obj-$(CONFIG_S3C_ADC)	+= adc.o

# devices

obj-$(CONFIG_SAMSUNG_ATAGS)	+= platformdata.o

>>>>>>> b7ba80a49124 (Commit)
obj-$(CONFIG_SAMSUNG_ATAGS)	+= devs.o
obj-$(CONFIG_SAMSUNG_ATAGS)	+= dev-uart.o

obj-$(CONFIG_GPIO_SAMSUNG)     += gpio-samsung.o

# PM support

obj-$(CONFIG_SAMSUNG_PM)	+= pm.o pm-common.o
obj-$(CONFIG_SAMSUNG_PM_GPIO)	+= pm-gpio.o
<<<<<<< HEAD
=======

>>>>>>> b7ba80a49124 (Commit)
obj-$(CONFIG_SAMSUNG_WAKEMASK)	+= wakeup-mask.o
