/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_FPU_XCR_H
#define _ASM_X86_FPU_XCR_H

#define XCR_XFEATURE_ENABLED_MASK	0x00000000
#define XCR_XFEATURE_IN_USE_MASK	0x00000001

<<<<<<< HEAD
static __always_inline u64 xgetbv(u32 index)
=======
static inline u64 xgetbv(u32 index)
>>>>>>> b7ba80a49124 (Commit)
{
	u32 eax, edx;

	asm volatile("xgetbv" : "=a" (eax), "=d" (edx) : "c" (index));
	return eax + ((u64)edx << 32);
}

static inline void xsetbv(u32 index, u64 value)
{
	u32 eax = value;
	u32 edx = value >> 32;

	asm volatile("xsetbv" :: "a" (eax), "d" (edx), "c" (index));
}

/*
 * Return a mask of xfeatures which are currently being tracked
 * by the processor as being in the initial configuration.
 *
 * Callers should check X86_FEATURE_XGETBV1.
 */
<<<<<<< HEAD
static __always_inline u64 xfeatures_in_use(void)
=======
static inline u64 xfeatures_in_use(void)
>>>>>>> b7ba80a49124 (Commit)
{
	return xgetbv(XCR_XFEATURE_IN_USE_MASK);
}

#endif /* _ASM_X86_FPU_XCR_H */
