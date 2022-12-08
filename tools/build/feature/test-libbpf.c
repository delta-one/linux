// SPDX-License-Identifier: GPL-2.0
#include <bpf/libbpf.h>

<<<<<<< HEAD
#if !defined(LIBBPF_MAJOR_VERSION) || (LIBBPF_MAJOR_VERSION < 1)
#error At least libbpf 1.0 is required for Linux tools.
#endif

=======
>>>>>>> b7ba80a49124 (Commit)
int main(void)
{
	return bpf_object__open("test") ? 0 : -1;
}
