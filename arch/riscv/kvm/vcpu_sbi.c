// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *     Atish Patra <atish.patra@wdc.com>
 */

#include <linux/errno.h>
#include <linux/err.h>
#include <linux/kvm_host.h>
#include <asm/sbi.h>
#include <asm/kvm_vcpu_sbi.h>

<<<<<<< HEAD
=======
static int kvm_linux_err_map_sbi(int err)
{
	switch (err) {
	case 0:
		return SBI_SUCCESS;
	case -EPERM:
		return SBI_ERR_DENIED;
	case -EINVAL:
		return SBI_ERR_INVALID_PARAM;
	case -EFAULT:
		return SBI_ERR_INVALID_ADDRESS;
	case -EOPNOTSUPP:
		return SBI_ERR_NOT_SUPPORTED;
	case -EALREADY:
		return SBI_ERR_ALREADY_AVAILABLE;
	default:
		return SBI_ERR_FAILURE;
	};
}

>>>>>>> b7ba80a49124 (Commit)
#ifndef CONFIG_RISCV_SBI_V01
static const struct kvm_vcpu_sbi_extension vcpu_sbi_ext_v01 = {
	.extid_start = -1UL,
	.extid_end = -1UL,
	.handler = NULL,
};
#endif

<<<<<<< HEAD
#ifdef CONFIG_RISCV_PMU_SBI
extern const struct kvm_vcpu_sbi_extension vcpu_sbi_ext_pmu;
#else
static const struct kvm_vcpu_sbi_extension vcpu_sbi_ext_pmu = {
	.extid_start = -1UL,
	.extid_end = -1UL,
	.handler = NULL,
};
#endif

=======
>>>>>>> b7ba80a49124 (Commit)
static const struct kvm_vcpu_sbi_extension *sbi_ext[] = {
	&vcpu_sbi_ext_v01,
	&vcpu_sbi_ext_base,
	&vcpu_sbi_ext_time,
	&vcpu_sbi_ext_ipi,
	&vcpu_sbi_ext_rfence,
	&vcpu_sbi_ext_srst,
	&vcpu_sbi_ext_hsm,
<<<<<<< HEAD
	&vcpu_sbi_ext_pmu,
=======
>>>>>>> b7ba80a49124 (Commit)
	&vcpu_sbi_ext_experimental,
	&vcpu_sbi_ext_vendor,
};

void kvm_riscv_vcpu_sbi_forward(struct kvm_vcpu *vcpu, struct kvm_run *run)
{
	struct kvm_cpu_context *cp = &vcpu->arch.guest_context;

	vcpu->arch.sbi_context.return_handled = 0;
	vcpu->stat.ecall_exit_stat++;
	run->exit_reason = KVM_EXIT_RISCV_SBI;
	run->riscv_sbi.extension_id = cp->a7;
	run->riscv_sbi.function_id = cp->a6;
	run->riscv_sbi.args[0] = cp->a0;
	run->riscv_sbi.args[1] = cp->a1;
	run->riscv_sbi.args[2] = cp->a2;
	run->riscv_sbi.args[3] = cp->a3;
	run->riscv_sbi.args[4] = cp->a4;
	run->riscv_sbi.args[5] = cp->a5;
	run->riscv_sbi.ret[0] = cp->a0;
	run->riscv_sbi.ret[1] = cp->a1;
}

void kvm_riscv_vcpu_sbi_system_reset(struct kvm_vcpu *vcpu,
				     struct kvm_run *run,
				     u32 type, u64 reason)
{
	unsigned long i;
	struct kvm_vcpu *tmp;

	kvm_for_each_vcpu(i, tmp, vcpu->kvm)
		tmp->arch.power_off = true;
	kvm_make_all_cpus_request(vcpu->kvm, KVM_REQ_SLEEP);

	memset(&run->system_event, 0, sizeof(run->system_event));
	run->system_event.type = type;
	run->system_event.ndata = 1;
	run->system_event.data[0] = reason;
	run->exit_reason = KVM_EXIT_SYSTEM_EVENT;
}

int kvm_riscv_vcpu_sbi_return(struct kvm_vcpu *vcpu, struct kvm_run *run)
{
	struct kvm_cpu_context *cp = &vcpu->arch.guest_context;

	/* Handle SBI return only once */
	if (vcpu->arch.sbi_context.return_handled)
		return 0;
	vcpu->arch.sbi_context.return_handled = 1;

	/* Update return values */
	cp->a0 = run->riscv_sbi.ret[0];
	cp->a1 = run->riscv_sbi.ret[1];

	/* Move to next instruction */
	vcpu->arch.guest_context.sepc += 4;

	return 0;
}

const struct kvm_vcpu_sbi_extension *kvm_vcpu_sbi_find_ext(unsigned long extid)
{
	int i = 0;

	for (i = 0; i < ARRAY_SIZE(sbi_ext); i++) {
		if (sbi_ext[i]->extid_start <= extid &&
		    sbi_ext[i]->extid_end >= extid)
			return sbi_ext[i];
	}

	return NULL;
}

int kvm_riscv_vcpu_sbi_ecall(struct kvm_vcpu *vcpu, struct kvm_run *run)
{
	int ret = 1;
	bool next_sepc = true;
<<<<<<< HEAD
	struct kvm_cpu_context *cp = &vcpu->arch.guest_context;
	const struct kvm_vcpu_sbi_extension *sbi_ext;
	struct kvm_cpu_trap utrap = {0};
	struct kvm_vcpu_sbi_return sbi_ret = {
		.out_val = 0,
		.err_val = 0,
		.utrap = &utrap,
	};
=======
	bool userspace_exit = false;
	struct kvm_cpu_context *cp = &vcpu->arch.guest_context;
	const struct kvm_vcpu_sbi_extension *sbi_ext;
	struct kvm_cpu_trap utrap = { 0 };
	unsigned long out_val = 0;
>>>>>>> b7ba80a49124 (Commit)
	bool ext_is_v01 = false;

	sbi_ext = kvm_vcpu_sbi_find_ext(cp->a7);
	if (sbi_ext && sbi_ext->handler) {
#ifdef CONFIG_RISCV_SBI_V01
		if (cp->a7 >= SBI_EXT_0_1_SET_TIMER &&
		    cp->a7 <= SBI_EXT_0_1_SHUTDOWN)
			ext_is_v01 = true;
#endif
<<<<<<< HEAD
		ret = sbi_ext->handler(vcpu, run, &sbi_ret);
=======
		ret = sbi_ext->handler(vcpu, run, &out_val, &utrap, &userspace_exit);
>>>>>>> b7ba80a49124 (Commit)
	} else {
		/* Return error for unsupported SBI calls */
		cp->a0 = SBI_ERR_NOT_SUPPORTED;
		goto ecall_done;
	}

<<<<<<< HEAD
	/*
	 * When the SBI extension returns a Linux error code, it exits the ioctl
	 * loop and forwards the error to userspace.
	 */
	if (ret < 0) {
		next_sepc = false;
		goto ecall_done;
	}

	/* Handle special error cases i.e trap, exit or userspace forward */
	if (sbi_ret.utrap->scause) {
		/* No need to increment sepc or exit ioctl loop */
		ret = 1;
		sbi_ret.utrap->sepc = cp->sepc;
		kvm_riscv_vcpu_trap_redirect(vcpu, sbi_ret.utrap);
=======
	/* Handle special error cases i.e trap, exit or userspace forward */
	if (utrap.scause) {
		/* No need to increment sepc or exit ioctl loop */
		ret = 1;
		utrap.sepc = cp->sepc;
		kvm_riscv_vcpu_trap_redirect(vcpu, &utrap);
>>>>>>> b7ba80a49124 (Commit)
		next_sepc = false;
		goto ecall_done;
	}

	/* Exit ioctl loop or Propagate the error code the guest */
<<<<<<< HEAD
	if (sbi_ret.uexit) {
		next_sepc = false;
		ret = 0;
	} else {
		cp->a0 = sbi_ret.err_val;
=======
	if (userspace_exit) {
		next_sepc = false;
		ret = 0;
	} else {
		/**
		 * SBI extension handler always returns an Linux error code. Convert
		 * it to the SBI specific error code that can be propagated the SBI
		 * caller.
		 */
		ret = kvm_linux_err_map_sbi(ret);
		cp->a0 = ret;
>>>>>>> b7ba80a49124 (Commit)
		ret = 1;
	}
ecall_done:
	if (next_sepc)
		cp->sepc += 4;
<<<<<<< HEAD
	/* a1 should only be updated when we continue the ioctl loop */
	if (!ext_is_v01 && ret == 1)
		cp->a1 = sbi_ret.out_val;
=======
	if (!ext_is_v01)
		cp->a1 = out_val;
>>>>>>> b7ba80a49124 (Commit)

	return ret;
}
