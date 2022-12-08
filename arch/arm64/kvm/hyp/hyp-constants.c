// SPDX-License-Identifier: GPL-2.0-only

#include <linux/kbuild.h>
#include <nvhe/memory.h>
<<<<<<< HEAD
#include <nvhe/pkvm.h>
=======
>>>>>>> b7ba80a49124 (Commit)

int main(void)
{
	DEFINE(STRUCT_HYP_PAGE_SIZE,	sizeof(struct hyp_page));
<<<<<<< HEAD
	DEFINE(PKVM_HYP_VM_SIZE,	sizeof(struct pkvm_hyp_vm));
	DEFINE(PKVM_HYP_VCPU_SIZE,	sizeof(struct pkvm_hyp_vcpu));
=======
>>>>>>> b7ba80a49124 (Commit)
	return 0;
}
