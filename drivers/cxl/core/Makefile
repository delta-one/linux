# SPDX-License-Identifier: GPL-2.0
obj-$(CONFIG_CXL_BUS) += cxl_core.o
obj-$(CONFIG_CXL_SUSPEND) += suspend.o

ccflags-y += -I$(srctree)/drivers/cxl
<<<<<<< HEAD
CFLAGS_trace.o = -DTRACE_INCLUDE_PATH=. -I$(src)

=======
>>>>>>> b7ba80a49124 (Commit)
cxl_core-y := port.o
cxl_core-y += pmem.o
cxl_core-y += regs.o
cxl_core-y += memdev.o
cxl_core-y += mbox.o
cxl_core-y += pci.o
cxl_core-y += hdm.o
<<<<<<< HEAD
cxl_core-$(CONFIG_TRACING) += trace.o
=======
>>>>>>> b7ba80a49124 (Commit)
cxl_core-$(CONFIG_CXL_REGION) += region.o
