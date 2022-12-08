# SPDX-License-Identifier: GPL-2.0
<<<<<<< HEAD
subdir-y += allwinner
subdir-y += sifive
subdir-y += starfive
subdir-y += canaan
subdir-y += microchip
subdir-y += renesas
=======
subdir-y += sifive
subdir-y += starfive
subdir-$(CONFIG_SOC_CANAAN_K210_DTB_BUILTIN) += canaan
subdir-y += microchip
>>>>>>> b7ba80a49124 (Commit)

obj-$(CONFIG_BUILTIN_DTB) := $(addsuffix /, $(subdir-y))
