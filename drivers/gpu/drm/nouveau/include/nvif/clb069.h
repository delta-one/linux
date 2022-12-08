#ifndef __NVIF_CLB069_H__
#define __NVIF_CLB069_H__
struct nvif_clb069_v0 {
	__u8  version;
	__u8  pad01[3];
	__u32 entries;
	__u32 get;
	__u32 put;
};

<<<<<<< HEAD
union nvif_clb069_event_args {
	struct nvif_clb069_event_vn {
	} vn;
};
=======
#define NVB069_V0_NTFY_FAULT                                                0x00
>>>>>>> b7ba80a49124 (Commit)
#endif
