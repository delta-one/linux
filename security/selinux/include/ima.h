/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2021 Microsoft Corporation
 *
 * Author: Lakshmi Ramasubramanian (nramas@linux.microsoft.com)
 *
 * Measure critical data structures maintainted by SELinux
 * using IMA subsystem.
 */

#ifndef _SELINUX_IMA_H_
#define _SELINUX_IMA_H_

#include "security.h"

#ifdef CONFIG_IMA
<<<<<<< HEAD
extern void selinux_ima_measure_state(void);
extern void selinux_ima_measure_state_locked(void);
#else
static inline void selinux_ima_measure_state(void)
{
}
static inline void selinux_ima_measure_state_locked(void)
=======
extern void selinux_ima_measure_state(struct selinux_state *selinux_state);
extern void selinux_ima_measure_state_locked(
			struct selinux_state *selinux_state);
#else
static inline void selinux_ima_measure_state(struct selinux_state *selinux_state)
{
}
static inline void selinux_ima_measure_state_locked(
			struct selinux_state *selinux_state)
>>>>>>> b7ba80a49124 (Commit)
{
}
#endif

#endif	/* _SELINUX_IMA_H_ */
