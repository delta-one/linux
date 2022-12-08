/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * KVM L1 hypervisor optimizations on Hyper-V for SVM.
 */

#ifndef __ARCH_X86_KVM_SVM_ONHYPERV_H__
#define __ARCH_X86_KVM_SVM_ONHYPERV_H__

#if IS_ENABLED(CONFIG_HYPERV)

#include "kvm_onhyperv.h"
#include "svm/hyperv.h"

static struct kvm_x86_ops svm_x86_ops;

<<<<<<< HEAD
int svm_hv_enable_l2_tlb_flush(struct kvm_vcpu *vcpu);

static inline void svm_hv_init_vmcb(struct vmcb *vmcb)
{
	struct hv_vmcb_enlightenments *hve = &vmcb->control.hv_enlightenments;

	BUILD_BUG_ON(sizeof(vmcb->control.hv_enlightenments) !=
		     sizeof(vmcb->control.reserved_sw));
=======
int svm_hv_enable_direct_tlbflush(struct kvm_vcpu *vcpu);

static inline void svm_hv_init_vmcb(struct vmcb *vmcb)
{
	struct hv_enlightenments *hve =
		(struct hv_enlightenments *)vmcb->control.reserved_sw;
>>>>>>> b7ba80a49124 (Commit)

	if (npt_enabled &&
	    ms_hyperv.nested_features & HV_X64_NESTED_ENLIGHTENED_TLB)
		hve->hv_enlightenments_control.enlightened_npt_tlb = 1;

	if (ms_hyperv.nested_features & HV_X64_NESTED_MSR_BITMAP)
		hve->hv_enlightenments_control.msr_bitmap = 1;
}

<<<<<<< HEAD
static inline __init void svm_hv_hardware_setup(void)
{
	if (npt_enabled &&
	    ms_hyperv.nested_features & HV_X64_NESTED_ENLIGHTENED_TLB) {
		pr_info(KBUILD_MODNAME ": Hyper-V enlightened NPT TLB flush enabled\n");
=======
static inline void svm_hv_hardware_setup(void)
{
	if (npt_enabled &&
	    ms_hyperv.nested_features & HV_X64_NESTED_ENLIGHTENED_TLB) {
		pr_info("kvm: Hyper-V enlightened NPT TLB flush enabled\n");
>>>>>>> b7ba80a49124 (Commit)
		svm_x86_ops.tlb_remote_flush = hv_remote_flush_tlb;
		svm_x86_ops.tlb_remote_flush_with_range =
				hv_remote_flush_tlb_with_range;
	}

	if (ms_hyperv.nested_features & HV_X64_NESTED_DIRECT_FLUSH) {
		int cpu;

<<<<<<< HEAD
		pr_info(KBUILD_MODNAME ": Hyper-V Direct TLB Flush enabled\n");
=======
		pr_info("kvm: Hyper-V Direct TLB Flush enabled\n");
>>>>>>> b7ba80a49124 (Commit)
		for_each_online_cpu(cpu) {
			struct hv_vp_assist_page *vp_ap =
				hv_get_vp_assist_page(cpu);

			if (!vp_ap)
				continue;

			vp_ap->nested_control.features.directhypercall = 1;
		}
<<<<<<< HEAD
		svm_x86_ops.enable_l2_tlb_flush =
				svm_hv_enable_l2_tlb_flush;
=======
		svm_x86_ops.enable_direct_tlbflush =
				svm_hv_enable_direct_tlbflush;
>>>>>>> b7ba80a49124 (Commit)
	}
}

static inline void svm_hv_vmcb_dirty_nested_enlightenments(
		struct kvm_vcpu *vcpu)
{
	struct vmcb *vmcb = to_svm(vcpu)->vmcb;
<<<<<<< HEAD
	struct hv_vmcb_enlightenments *hve = &vmcb->control.hv_enlightenments;

	if (hve->hv_enlightenments_control.msr_bitmap)
		vmcb_mark_dirty(vmcb, HV_VMCB_NESTED_ENLIGHTENMENTS);
}

static inline void svm_hv_update_vp_id(struct vmcb *vmcb, struct kvm_vcpu *vcpu)
{
	struct hv_vmcb_enlightenments *hve = &vmcb->control.hv_enlightenments;
=======
	struct hv_enlightenments *hve =
		(struct hv_enlightenments *)vmcb->control.reserved_sw;

	if (hve->hv_enlightenments_control.msr_bitmap)
		vmcb_mark_dirty(vmcb, VMCB_HV_NESTED_ENLIGHTENMENTS);
}

static inline void svm_hv_update_vp_id(struct vmcb *vmcb,
		struct kvm_vcpu *vcpu)
{
	struct hv_enlightenments *hve =
		(struct hv_enlightenments *)vmcb->control.reserved_sw;
>>>>>>> b7ba80a49124 (Commit)
	u32 vp_index = kvm_hv_get_vpindex(vcpu);

	if (hve->hv_vp_id != vp_index) {
		hve->hv_vp_id = vp_index;
<<<<<<< HEAD
		vmcb_mark_dirty(vmcb, HV_VMCB_NESTED_ENLIGHTENMENTS);
=======
		vmcb_mark_dirty(vmcb, VMCB_HV_NESTED_ENLIGHTENMENTS);
>>>>>>> b7ba80a49124 (Commit)
	}
}
#else

static inline void svm_hv_init_vmcb(struct vmcb *vmcb)
{
}

<<<<<<< HEAD
static inline __init void svm_hv_hardware_setup(void)
=======
static inline void svm_hv_hardware_setup(void)
>>>>>>> b7ba80a49124 (Commit)
{
}

static inline void svm_hv_vmcb_dirty_nested_enlightenments(
		struct kvm_vcpu *vcpu)
{
}

static inline void svm_hv_update_vp_id(struct vmcb *vmcb,
		struct kvm_vcpu *vcpu)
{
}
#endif /* CONFIG_HYPERV */

#endif /* __ARCH_X86_KVM_SVM_ONHYPERV_H__ */
