// SPDX-License-Identifier: GPL-2.0
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <errno.h>
#include <linux/capability.h>

<<<<<<< HEAD
typedef struct { unsigned long long val; } kernel_cap_t;

struct cred {
	kernel_cap_t cap_effective;
=======
struct kernel_cap_struct {
	__u32 cap[_LINUX_CAPABILITY_U32S_3];
} __attribute__((preserve_access_index));

struct cred {
	struct kernel_cap_struct cap_effective;
>>>>>>> b7ba80a49124 (Commit)
} __attribute__((preserve_access_index));

char _license[] SEC("license") = "GPL";

SEC("lsm.s/userns_create")
int BPF_PROG(test_userns_create, const struct cred *cred, int ret)
{
<<<<<<< HEAD
	kernel_cap_t caps = cred->cap_effective;
	__u64 cap_mask = 1ULL << CAP_SYS_ADMIN;
=======
	struct kernel_cap_struct caps = cred->cap_effective;
	int cap_index = CAP_TO_INDEX(CAP_SYS_ADMIN);
	__u32 cap_mask = CAP_TO_MASK(CAP_SYS_ADMIN);
>>>>>>> b7ba80a49124 (Commit)

	if (ret)
		return 0;

	ret = -EPERM;
<<<<<<< HEAD
	if (caps.val & cap_mask)
=======
	if (caps.cap[cap_index] & cap_mask)
>>>>>>> b7ba80a49124 (Commit)
		return 0;

	return -EPERM;
}
