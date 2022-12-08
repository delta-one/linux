# SPDX-License-Identifier: GPL-2.0-only

snd-soc-avs-objs := dsp.o ipc.o messages.o utils.o core.o loader.o \
<<<<<<< HEAD
		    topology.o path.o pcm.o board_selection.o control.o
=======
		    topology.o path.o pcm.o board_selection.o
>>>>>>> b7ba80a49124 (Commit)
snd-soc-avs-objs += cldma.o
snd-soc-avs-objs += skl.o apl.o

snd-soc-avs-objs += trace.o
# tell define_trace.h where to find the trace header
CFLAGS_trace.o := -I$(src)

<<<<<<< HEAD
ifneq ($(CONFIG_DEBUG_FS),)
snd-soc-avs-objs += probes.o debugfs.o
endif

=======
>>>>>>> b7ba80a49124 (Commit)
obj-$(CONFIG_SND_SOC_INTEL_AVS) += snd-soc-avs.o

# Machine support
obj-$(CONFIG_SND_SOC) += boards/
