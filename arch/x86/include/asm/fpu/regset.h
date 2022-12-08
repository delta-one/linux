/* SPDX-License-Identifier: GPL-2.0 */
/*
 * FPU regset handling methods:
 */
#ifndef _ASM_X86_FPU_REGSET_H
#define _ASM_X86_FPU_REGSET_H

#include <linux/regset.h>

<<<<<<< HEAD
extern user_regset_active_fn regset_fpregs_active, regset_xregset_fpregs_active,
				ssp_active;
extern user_regset_get2_fn fpregs_get, xfpregs_get, fpregs_soft_get,
				 xstateregs_get, ssp_get;
extern user_regset_set_fn fpregs_set, xfpregs_set, fpregs_soft_set,
				 xstateregs_set, ssp_set;
=======
extern user_regset_active_fn regset_fpregs_active, regset_xregset_fpregs_active;
extern user_regset_get2_fn fpregs_get, xfpregs_get, fpregs_soft_get,
				 xstateregs_get;
extern user_regset_set_fn fpregs_set, xfpregs_set, fpregs_soft_set,
				 xstateregs_set;
>>>>>>> b7ba80a49124 (Commit)

/*
 * xstateregs_active == regset_fpregs_active. Please refer to the comment
 * at the definition of regset_fpregs_active.
 */
#define xstateregs_active	regset_fpregs_active

#endif /* _ASM_X86_FPU_REGSET_H */
