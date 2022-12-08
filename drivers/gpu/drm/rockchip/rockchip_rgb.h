/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) Fuzhou Rockchip Electronics Co.Ltd
 * Author:
 *      Sandy Huang <hjc@rock-chips.com>
 */

#ifdef CONFIG_ROCKCHIP_RGB
struct rockchip_rgb *rockchip_rgb_init(struct device *dev,
				       struct drm_crtc *crtc,
<<<<<<< HEAD
				       struct drm_device *drm_dev,
				       int video_port);
=======
				       struct drm_device *drm_dev);
>>>>>>> b7ba80a49124 (Commit)
void rockchip_rgb_fini(struct rockchip_rgb *rgb);
#else
static inline struct rockchip_rgb *rockchip_rgb_init(struct device *dev,
						     struct drm_crtc *crtc,
<<<<<<< HEAD
						     struct drm_device *drm_dev,
						     int video_port)
=======
						     struct drm_device *drm_dev)
>>>>>>> b7ba80a49124 (Commit)
{
	return NULL;
}

static inline void rockchip_rgb_fini(struct rockchip_rgb *rgb)
{
}
#endif
