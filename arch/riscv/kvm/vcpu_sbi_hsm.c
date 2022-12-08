// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *     Atish Patra <atish.patra@wdc.com>
 */

#include <linux/errno.h>
#include <linux/err.h>
#include <linux/kvm_host.h>
<<<<<<< HEAD
=======
#include <asm/csr.h>
>>>>>>> b7ba80a49124 (Commit)
#include <asm/sbi.h>
#include <asm/kvm_vcpu_sbi.h>

static int kvm_sbi_hsm_vcpu_start(struct kvm_vcpu *vcpu)
{
	struct kvm_cpu_context *reset_cntx;
	struct kvm_cpu_context *cp = &vcpu->arch.guest_context;
	struct kvm_vcpu *target_vcpu;
	unsigned long target_vcpuid = cp->a0;

	target_vcpu = kvm_get_vcpu_by_id(vcpu->kvm, target_vcpuid);
	if (!target_vcpu)
<<<<<<< HEAD
		return SBI_ERR_INVALID_PARAM;
	if (!target_vcpu->arch.power_off)
		return SBI_ERR_ALREADY_AVAILABLE;
=======
		return -EINVAL;
	if (!target_vcpu->arch.power_off)
		return -EALREADY;
>>>>>>> b7ba80a49124 (Commit)

	reset_cntx = &target_vcpu->arch.guest_reset_context;
	/* start address */
	reset_cntx->sepc = cp->a1;
	/* target vcpu id to start */
	reset_cntx->a0 = target_vcpuid;
	/* private data passed from kernel */
	reset_cntx->a1 = cp->a2;
	kvm_make_request(KVM_REQ_VCPU_RESET, target_vcpu);

	kvm_riscv_vcpu_power_on(target_vcpu);

	return 0;
}

static int kvm_sbi_hsm_vcpu_stop(struct kvm_vcpu *vcpu)
{
	if (vcpu->arch.power_off)
<<<<<<< HEAD
		return SBI_ERR_FAILURE;
=======
		return -EINVAL;
>>>>>>> b7ba80a49124 (Commit)

	kvm_riscv_vcpu_power_off(vcpu);

	return 0;
}

static int kvm_sbi_hsm_vcpu_get_status(struct kvm_vcpu *vcpu)
{
	struct kvm_cpu_context *cp = &vcpu->arch.guest_context;
	unsigned long target_vcpuid = cp->a0;
	struct kvm_vcpu *target_vcpu;

	target_vcpu = kvm_get_vcpu_by_id(vcpu->kvm, target_vcpuid);
	if (!target_vcpu)
<<<<<<< HEAD
		return SBI_ERR_INVALID_PARAM;
=======
		return -EINVAL;
>>>>>>> b7ba80a49124 (Commit)
	if (!target_vcpu->arch.power_off)
		return SBI_HSM_STATE_STARTED;
	else if (vcpu->stat.generic.blocking)
		return SBI_HSM_STATE_SUSPENDED;
	else
		return SBI_HSM_STATE_STOPPED;
}

static int kvm_sbi_ext_hsm_handler(struct kvm_vcpu *vcpu, struct kvm_run *run,
<<<<<<< HEAD
				   struct kvm_vcpu_sbi_return *retdata)
=======
				   unsigned long *out_val,
				   struct kvm_cpu_trap *utrap,
				   bool *exit)
>>>>>>> b7ba80a49124 (Commit)
{
	int ret = 0;
	struct kvm_cpu_context *cp = &vcpu->arch.guest_context;
	struct kvm *kvm = vcpu->kvm;
	unsigned long funcid = cp->a6;

	switch (funcid) {
	case SBI_EXT_HSM_HART_START:
		mutex_lock(&kvm->lock);
		ret = kvm_sbi_hsm_vcpu_start(vcpu);
		mutex_unlock(&kvm->lock);
		break;
	case SBI_EXT_HSM_HART_STOP:
		ret = kvm_sbi_hsm_vcpu_stop(vcpu);
		break;
	case SBI_EXT_HSM_HART_STATUS:
		ret = kvm_sbi_hsm_vcpu_get_status(vcpu);
		if (ret >= 0) {
<<<<<<< HEAD
			retdata->out_val = ret;
			retdata->err_val = 0;
		}
		return 0;
=======
			*out_val = ret;
			ret = 0;
		}
		break;
>>>>>>> b7ba80a49124 (Commit)
	case SBI_EXT_HSM_HART_SUSPEND:
		switch (cp->a0) {
		case SBI_HSM_SUSPEND_RET_DEFAULT:
			kvm_riscv_vcpu_wfi(vcpu);
			break;
		case SBI_HSM_SUSPEND_NON_RET_DEFAULT:
<<<<<<< HEAD
			ret = SBI_ERR_NOT_SUPPORTED;
			break;
		default:
			ret = SBI_ERR_INVALID_PARAM;
		}
		break;
	default:
		ret = SBI_ERR_NOT_SUPPORTED;
	}

	retdata->err_val = ret;

	return 0;
=======
			ret = -EOPNOTSUPP;
			break;
		default:
			ret = -EINVAL;
		}
		break;
	default:
		ret = -EOPNOTSUPP;
	}

	return ret;
>>>>>>> b7ba80a49124 (Commit)
}

const struct kvm_vcpu_sbi_extension vcpu_sbi_ext_hsm = {
	.extid_start = SBI_EXT_HSM,
	.extid_end = SBI_EXT_HSM,
	.handler = kvm_sbi_ext_hsm_handler,
};
