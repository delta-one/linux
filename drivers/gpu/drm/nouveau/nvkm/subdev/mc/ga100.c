/*
 * Copyright 2021 Red Hat Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
#include "priv.h"

static void
<<<<<<< HEAD
ga100_mc_device_disable(struct nvkm_mc *mc, u32 mask)
{
	struct nvkm_device *device = mc->subdev.device;

	nvkm_mask(device, 0x000600, mask, 0x00000000);
	nvkm_rd32(device, 0x000600);
	nvkm_rd32(device, 0x000600);
}

static void
ga100_mc_device_enable(struct nvkm_mc *mc, u32 mask)
{
	struct nvkm_device *device = mc->subdev.device;

	nvkm_mask(device, 0x000600, mask, mask);
	nvkm_rd32(device, 0x000600);
	nvkm_rd32(device, 0x000600);
}

static bool
ga100_mc_device_enabled(struct nvkm_mc *mc, u32 mask)
{
	return (nvkm_rd32(mc->subdev.device, 0x000600) & mask) == mask;
}

const struct nvkm_mc_device_func
ga100_mc_device = {
	.enabled = ga100_mc_device_enabled,
	.enable = ga100_mc_device_enable,
	.disable = ga100_mc_device_disable,
};
=======
ga100_mc_intr_unarm(struct nvkm_mc *mc)
{
	nvkm_wr32(mc->subdev.device, 0xb81610, 0x00000004);
}

static void
ga100_mc_intr_rearm(struct nvkm_mc *mc)
{
	nvkm_wr32(mc->subdev.device, 0xb81608, 0x00000004);
}

static void
ga100_mc_intr_mask(struct nvkm_mc *mc, u32 mask, u32 intr)
{
	nvkm_wr32(mc->subdev.device, 0xb81210,          mask & intr );
	nvkm_wr32(mc->subdev.device, 0xb81410, mask & ~(mask & intr));
}

static u32
ga100_mc_intr_stat(struct nvkm_mc *mc)
{
	u32 intr_top = nvkm_rd32(mc->subdev.device, 0xb81600), intr = 0x00000000;
	if (intr_top & 0x00000004)
		intr = nvkm_mask(mc->subdev.device, 0xb81010, 0x00000000, 0x00000000);
	return intr;
}
>>>>>>> b7ba80a49124 (Commit)

static void
ga100_mc_init(struct nvkm_mc *mc)
{
<<<<<<< HEAD
	struct nvkm_device *device = mc->subdev.device;

	nvkm_wr32(device, 0x000200, 0xffffffff);
	nvkm_wr32(device, 0x000600, 0xffffffff);
=======
	nv50_mc_init(mc);
	nvkm_wr32(mc->subdev.device, 0xb81210, 0xffffffff);
>>>>>>> b7ba80a49124 (Commit)
}

static const struct nvkm_mc_func
ga100_mc = {
	.init = ga100_mc_init,
<<<<<<< HEAD
	.device = &ga100_mc_device,
=======
	.intr = gp100_mc_intr,
	.intr_unarm = ga100_mc_intr_unarm,
	.intr_rearm = ga100_mc_intr_rearm,
	.intr_mask = ga100_mc_intr_mask,
	.intr_stat = ga100_mc_intr_stat,
	.reset = gk104_mc_reset,
>>>>>>> b7ba80a49124 (Commit)
};

int
ga100_mc_new(struct nvkm_device *device, enum nvkm_subdev_type type, int inst, struct nvkm_mc **pmc)
{
	return nvkm_mc_new_(&ga100_mc, device, type, inst, pmc);
}
