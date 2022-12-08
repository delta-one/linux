#ifndef __NVKM_GSP_H__
#define __NVKM_GSP_H__
#define nvkm_gsp(p) container_of((p), struct nvkm_gsp, subdev)
#include <core/subdev.h>
#include <core/falcon.h>

struct nvkm_gsp {
<<<<<<< HEAD
	const struct nvkm_gsp_func *func;
	struct nvkm_subdev subdev;

=======
	struct nvkm_subdev subdev;
>>>>>>> b7ba80a49124 (Commit)
	struct nvkm_falcon falcon;
};

int gv100_gsp_new(struct nvkm_device *, enum nvkm_subdev_type, int, struct nvkm_gsp **);
<<<<<<< HEAD
int ga102_gsp_new(struct nvkm_device *, enum nvkm_subdev_type, int, struct nvkm_gsp **);
=======
>>>>>>> b7ba80a49124 (Commit)
#endif
