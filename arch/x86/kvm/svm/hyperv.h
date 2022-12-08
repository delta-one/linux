/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Common Hyper-V on KVM and KVM on Hyper-V definitions (SVM).
 */

#ifndef __ARCH_X86_KVM_SVM_HYPERV_H__
#define __ARCH_X86_KVM_SVM_HYPERV_H__

#include <asm/mshyperv.h>

#include "../hyperv.h"
<<<<<<< HEAD
#include "svm.h"

static inline void nested_svm_hv_update_vm_vp_ids(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	struct hv_vmcb_enlightenments *hve = &svm->nested.ctl.hv_enlightenments;
	struct kvm_vcpu_hv *hv_vcpu = to_hv_vcpu(vcpu);

	if (!hv_vcpu)
		return;

	hv_vcpu->nested.pa_page_gpa = hve->partition_assist_page;
	hv_vcpu->nested.vm_id = hve->hv_vm_id;
	hv_vcpu->nested.vp_id = hve->hv_vp_id;
}

static inline bool nested_svm_l2_tlb_flush_enabled(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	struct hv_vmcb_enlightenments *hve = &svm->nested.ctl.hv_enlightenments;
	struct kvm_vcpu_hv *hv_vcpu = to_hv_vcpu(vcpu);

	if (!hv_vcpu)
		return false;

	if (!hve->hv_enlightenments_control.nested_flush_hypercall)
		return false;

	return hv_vcpu->vp_assist_page.nested_control.features.directhypercall;
}

void svm_hv_inject_synthetic_vmexit_post_tlb_flush(struct kvm_vcpu *vcpu);
=======

/*
 * Hyper-V uses the software reserved 32 bytes in VMCB
 * control area to expose SVM enlightenments to guests.
 */
struct hv_enlightenments {
	struct __packed hv_enlightenments_control {
		u32 nested_flush_hypercall:1;
		u32 msr_bitmap:1;
		u32 enlightened_npt_tlb: 1;
		u32 reserved:29;
	} __packed hv_enlightenments_control;
	u32 hv_vp_id;
	u64 hv_vm_id;
	u64 partition_assist_page;
	u64 reserved;
} __packed;

/*
 * Hyper-V uses the software reserved clean bit in VMCB
 */
#define VMCB_HV_NESTED_ENLIGHTENMENTS VMCB_SW
>>>>>>> b7ba80a49124 (Commit)

#endif /* __ARCH_X86_KVM_SVM_HYPERV_H__ */
