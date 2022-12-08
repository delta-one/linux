# SPDX-License-Identifier: GPL-2.0-only
#
# Makefile for the linux kernel.
#
<<<<<<< HEAD
ccflags-y := -I$(srctree)/$(src)/include
=======
ccflags-$(CONFIG_ARCH_MULTIPLATFORM) := -I$(srctree)/$(src)/include
>>>>>>> b7ba80a49124 (Commit)

orion-gpio-$(CONFIG_GPIOLIB)      += gpio.o
obj-$(CONFIG_PLAT_ORION_LEGACY)   += irq.o pcie.o time.o common.o mpp.o
obj-$(CONFIG_PLAT_ORION_LEGACY)   += $(orion-gpio-y)
