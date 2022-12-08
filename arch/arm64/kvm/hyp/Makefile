# SPDX-License-Identifier: GPL-2.0
#
# Makefile for Kernel-based Virtual Machine module, HYP part
#

incdir := $(srctree)/$(src)/include
subdir-asflags-y := -I$(incdir)
<<<<<<< HEAD
subdir-ccflags-y := -I$(incdir)
=======
subdir-ccflags-y := -I$(incdir)				\
		    -fno-stack-protector		\
		    -DDISABLE_BRANCH_PROFILING		\
		    $(DISABLE_STACKLEAK_PLUGIN)
>>>>>>> b7ba80a49124 (Commit)

obj-$(CONFIG_KVM) += vhe/ nvhe/ pgtable.o
