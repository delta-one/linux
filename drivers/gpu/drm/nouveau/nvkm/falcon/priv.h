/* SPDX-License-Identifier: MIT */
#ifndef __NVKM_FALCON_PRIV_H__
#define __NVKM_FALCON_PRIV_H__
#include <core/falcon.h>
<<<<<<< HEAD

static inline int
nvkm_falcon_enable(struct nvkm_falcon *falcon)
{
	if (falcon->func->enable)
		return falcon->func->enable(falcon);
	return 0;
}
=======
>>>>>>> b7ba80a49124 (Commit)
#endif
