/*
 * Copyright 2013 Red Hat Inc.
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
 *
 * Authors: Ben Skeggs
 */
<<<<<<< HEAD
#include "priv.h"
#include "runq.h"
=======
#include "gk104.h"
#include "changk104.h"
>>>>>>> b7ba80a49124 (Commit)

#include <nvif/class.h>

void
<<<<<<< HEAD
gk208_runq_init(struct nvkm_runq *runq)
{
	gk104_runq_init(runq);

	nvkm_wr32(runq->fifo->engine.subdev.device, 0x04012c + (runq->id * 0x2000), 0x000f4240);
}

const struct nvkm_runq_func
gk208_runq = {
	.init = gk208_runq_init,
	.intr = gk104_runq_intr,
	.intr_0_names = gk104_runq_intr_0_names,
	.idle = gk104_runq_idle,
};

static int
gk208_fifo_chid_nr(struct nvkm_fifo *fifo)
{
	return 1024;
}

static const struct nvkm_fifo_func
gk208_fifo = {
	.chid_nr = gk208_fifo_chid_nr,
	.chid_ctor = gk110_fifo_chid_ctor,
	.runq_nr = gf100_fifo_runq_nr,
	.runl_ctor = gk104_fifo_runl_ctor,
	.init = gk104_fifo_init,
	.init_pbdmas = gk104_fifo_init_pbdmas,
	.intr = gk104_fifo_intr,
	.intr_mmu_fault_unit = gf100_fifo_intr_mmu_fault_unit,
	.intr_ctxsw_timeout = gf100_fifo_intr_ctxsw_timeout,
	.mmu_fault = &gk104_fifo_mmu_fault,
	.nonstall = &gf100_fifo_nonstall,
	.runl = &gk110_runl,
	.runq = &gk208_runq,
	.engn = &gk104_engn,
	.engn_ce = &gk104_engn_ce,
	.cgrp = {{ 0, 0, KEPLER_CHANNEL_GROUP_A  }, &gk110_cgrp },
	.chan = {{ 0, 0, KEPLER_CHANNEL_GPFIFO_A }, &gk110_chan },
=======
gk208_fifo_pbdma_init_timeout(struct gk104_fifo *fifo)
{
	struct nvkm_device *device = fifo->base.engine.subdev.device;
	int i;

	for (i = 0; i < fifo->pbdma_nr; i++)
		nvkm_wr32(device, 0x04012c + (i * 0x2000), 0x0000ffff);
}

const struct gk104_fifo_pbdma_func
gk208_fifo_pbdma = {
	.nr = gk104_fifo_pbdma_nr,
	.init = gk104_fifo_pbdma_init,
	.init_timeout = gk208_fifo_pbdma_init_timeout,
};

static const struct gk104_fifo_func
gk208_fifo = {
	.intr.fault = gf100_fifo_intr_fault,
	.pbdma = &gk208_fifo_pbdma,
	.fault.access = gk104_fifo_fault_access,
	.fault.engine = gk104_fifo_fault_engine,
	.fault.reason = gk104_fifo_fault_reason,
	.fault.hubclient = gk104_fifo_fault_hubclient,
	.fault.gpcclient = gk104_fifo_fault_gpcclient,
	.runlist = &gk110_fifo_runlist,
	.chan = {{0,0,KEPLER_CHANNEL_GPFIFO_A}, gk104_fifo_gpfifo_new },
>>>>>>> b7ba80a49124 (Commit)
};

int
gk208_fifo_new(struct nvkm_device *device, enum nvkm_subdev_type type, int inst,
	       struct nvkm_fifo **pfifo)
{
<<<<<<< HEAD
	return nvkm_fifo_new_(&gk208_fifo, device, type, inst, pfifo);
=======
	return gk104_fifo_new_(&gk208_fifo, device, type, inst, 1024, pfifo);
>>>>>>> b7ba80a49124 (Commit)
}
