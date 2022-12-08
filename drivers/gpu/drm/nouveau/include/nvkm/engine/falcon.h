/* SPDX-License-Identifier: MIT */
#ifndef __NVKM_FLCNEN_H__
#define __NVKM_FLCNEN_H__
#define nvkm_falcon(p) container_of((p), struct nvkm_falcon, engine)
#include <core/engine.h>
struct nvkm_fifo_chan;

enum nvkm_falcon_dmaidx {
	FALCON_DMAIDX_UCODE		= 0,
	FALCON_DMAIDX_VIRT		= 1,
	FALCON_DMAIDX_PHYS_VID		= 2,
	FALCON_DMAIDX_PHYS_SYS_COH	= 3,
	FALCON_DMAIDX_PHYS_SYS_NCOH	= 4,
	FALCON_SEC2_DMAIDX_UCODE	= 6,
};

struct nvkm_falcon {
	const struct nvkm_falcon_func *func;
<<<<<<< HEAD
	struct nvkm_subdev *owner;
	const char *name;
	u32 addr;
	u32 addr2;
=======
	const struct nvkm_subdev *owner;
	const char *name;
	u32 addr;
>>>>>>> b7ba80a49124 (Commit)

	struct mutex mutex;
	struct mutex dmem_mutex;
	bool oneinit;

<<<<<<< HEAD
	struct nvkm_subdev *user;
=======
	const struct nvkm_subdev *user;
>>>>>>> b7ba80a49124 (Commit)

	u8 version;
	u8 secret;
	bool debug;

	struct nvkm_memory *core;
	bool external;

	struct {
		u32 limit;
		u32 *data;
		u32  size;
		u8 ports;
	} code;

	struct {
		u32 limit;
		u32 *data;
		u32  size;
		u8 ports;
	} data;

	struct nvkm_engine engine;
};

<<<<<<< HEAD
int nvkm_falcon_get(struct nvkm_falcon *, struct nvkm_subdev *);
void nvkm_falcon_put(struct nvkm_falcon *, struct nvkm_subdev *);
=======
int nvkm_falcon_get(struct nvkm_falcon *, const struct nvkm_subdev *);
void nvkm_falcon_put(struct nvkm_falcon *, const struct nvkm_subdev *);
>>>>>>> b7ba80a49124 (Commit)

int nvkm_falcon_new_(const struct nvkm_falcon_func *, struct nvkm_device *,
		     enum nvkm_subdev_type, int inst, bool enable, u32 addr, struct nvkm_engine **);

struct nvkm_falcon_func {
<<<<<<< HEAD
	int (*disable)(struct nvkm_falcon *);
	int (*enable)(struct nvkm_falcon *);
	int (*select)(struct nvkm_falcon *);
	u32 addr2;
	bool reset_pmc;
	int (*reset_eng)(struct nvkm_falcon *);
	int (*reset_prep)(struct nvkm_falcon *);
	int (*reset_wait_mem_scrubbing)(struct nvkm_falcon *);

	u32 debug;
	void (*bind_inst)(struct nvkm_falcon *, int target, u64 addr);
	int (*bind_stat)(struct nvkm_falcon *, bool intr);
	bool bind_intr;

	const struct nvkm_falcon_func_pio *imem_pio;
	const struct nvkm_falcon_func_dma *imem_dma;

	const struct nvkm_falcon_func_pio *dmem_pio;
	const struct nvkm_falcon_func_dma *dmem_dma;

	u32 emem_addr;
	const struct nvkm_falcon_func_pio *emem_pio;

	struct {
		u32 head;
		u32 tail;
		u32 stride;
	} cmdq, msgq;

=======
>>>>>>> b7ba80a49124 (Commit)
	struct {
		u32 *data;
		u32  size;
	} code;
	struct {
		u32 *data;
		u32  size;
	} data;
	void (*init)(struct nvkm_falcon *);
<<<<<<< HEAD
	void (*intr)(struct nvkm_falcon *, struct nvkm_chan *);

	void (*load_imem)(struct nvkm_falcon *, void *, u32, u32, u16, u8, bool);
	void (*load_dmem)(struct nvkm_falcon *, void *, u32, u32, u8);
	void (*start)(struct nvkm_falcon *);
=======
	void (*intr)(struct nvkm_falcon *, struct nvkm_fifo_chan *);

	u32 debug;
	u32 fbif;

	void (*load_imem)(struct nvkm_falcon *, void *, u32, u32, u16, u8, bool);
	void (*load_dmem)(struct nvkm_falcon *, void *, u32, u32, u8);
	void (*read_dmem)(struct nvkm_falcon *, u32, u32, u8, void *);
	u32 emem_addr;
	void (*bind_context)(struct nvkm_falcon *, struct nvkm_memory *);
	int (*wait_for_halt)(struct nvkm_falcon *, u32);
	int (*clear_interrupt)(struct nvkm_falcon *, u32);
	void (*set_start_addr)(struct nvkm_falcon *, u32 start_addr);
	void (*start)(struct nvkm_falcon *);
	int (*enable)(struct nvkm_falcon *falcon);
	void (*disable)(struct nvkm_falcon *falcon);
	int (*reset)(struct nvkm_falcon *);

	struct {
		u32 head;
		u32 tail;
		u32 stride;
	} cmdq, msgq;
>>>>>>> b7ba80a49124 (Commit)

	struct nvkm_sclass sclass[];
};

static inline u32
nvkm_falcon_rd32(struct nvkm_falcon *falcon, u32 addr)
{
	return nvkm_rd32(falcon->owner->device, falcon->addr + addr);
}

static inline void
nvkm_falcon_wr32(struct nvkm_falcon *falcon, u32 addr, u32 data)
{
	nvkm_wr32(falcon->owner->device, falcon->addr + addr, data);
}

static inline u32
nvkm_falcon_mask(struct nvkm_falcon *falcon, u32 addr, u32 mask, u32 val)
{
	struct nvkm_device *device = falcon->owner->device;

	return nvkm_mask(device, falcon->addr + addr, mask, val);
}

void nvkm_falcon_load_imem(struct nvkm_falcon *, void *, u32, u32, u16, u8,
			   bool);
void nvkm_falcon_load_dmem(struct nvkm_falcon *, void *, u32, u32, u8);
<<<<<<< HEAD
void nvkm_falcon_start(struct nvkm_falcon *);
=======
void nvkm_falcon_read_dmem(struct nvkm_falcon *, u32, u32, u8, void *);
void nvkm_falcon_bind_context(struct nvkm_falcon *, struct nvkm_memory *);
void nvkm_falcon_set_start_addr(struct nvkm_falcon *, u32);
void nvkm_falcon_start(struct nvkm_falcon *);
int nvkm_falcon_wait_for_halt(struct nvkm_falcon *, u32);
int nvkm_falcon_clear_interrupt(struct nvkm_falcon *, u32);
int nvkm_falcon_enable(struct nvkm_falcon *);
void nvkm_falcon_disable(struct nvkm_falcon *);
int nvkm_falcon_reset(struct nvkm_falcon *);
>>>>>>> b7ba80a49124 (Commit)
#endif
