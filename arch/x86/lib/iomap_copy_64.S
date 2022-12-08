/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2006 PathScale, Inc.  All Rights Reserved.
 */

#include <linux/linkage.h>

/*
 * override generic version in lib/iomap_copy.c
 */
SYM_FUNC_START(__iowrite32_copy)
	movl %edx,%ecx
<<<<<<< HEAD
	rep movsl
=======
	rep movsd
>>>>>>> b7ba80a49124 (Commit)
	RET
SYM_FUNC_END(__iowrite32_copy)
