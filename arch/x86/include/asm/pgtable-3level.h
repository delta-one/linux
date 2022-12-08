/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_PGTABLE_3LEVEL_H
#define _ASM_X86_PGTABLE_3LEVEL_H

<<<<<<< HEAD
=======
#include <asm/atomic64_32.h>

>>>>>>> b7ba80a49124 (Commit)
/*
 * Intel Physical Address Extension (PAE) Mode - three-level page
 * tables on PPro+ CPUs.
 *
 * Copyright (C) 1999 Ingo Molnar <mingo@redhat.com>
 */

#define pte_ERROR(e)							\
	pr_err("%s:%d: bad pte %p(%08lx%08lx)\n",			\
	       __FILE__, __LINE__, &(e), (e).pte_high, (e).pte_low)
#define pmd_ERROR(e)							\
	pr_err("%s:%d: bad pmd %p(%016Lx)\n",				\
	       __FILE__, __LINE__, &(e), pmd_val(e))
#define pgd_ERROR(e)							\
	pr_err("%s:%d: bad pgd %p(%016Lx)\n",				\
	       __FILE__, __LINE__, &(e), pgd_val(e))

<<<<<<< HEAD
#define pxx_xchg64(_pxx, _ptr, _val) ({					\
	_pxx##val_t *_p = (_pxx##val_t *)_ptr;				\
	_pxx##val_t _o = *_p;						\
	do { } while (!try_cmpxchg64(_p, &_o, (_val)));			\
	native_make_##_pxx(_o);						\
})

/*
 * Rules for using set_pte: the pte being assigned *must* be
=======
/* Rules for using set_pte: the pte being assigned *must* be
>>>>>>> b7ba80a49124 (Commit)
 * either not present or in a state where the hardware will
 * not attempt to update the pte.  In places where this is
 * not possible, use pte_get_and_clear to obtain the old pte
 * value and then use set_pte to update it.  -ben
 */
static inline void native_set_pte(pte_t *ptep, pte_t pte)
{
<<<<<<< HEAD
	WRITE_ONCE(ptep->pte_high, pte.pte_high);
	smp_wmb();
	WRITE_ONCE(ptep->pte_low, pte.pte_low);
=======
	ptep->pte_high = pte.pte_high;
	smp_wmb();
	ptep->pte_low = pte.pte_low;
}

#define pmd_read_atomic pmd_read_atomic
/*
 * pte_offset_map_lock() on 32-bit PAE kernels was reading the pmd_t with
 * a "*pmdp" dereference done by GCC. Problem is, in certain places
 * where pte_offset_map_lock() is called, concurrent page faults are
 * allowed, if the mmap_lock is hold for reading. An example is mincore
 * vs page faults vs MADV_DONTNEED. On the page fault side
 * pmd_populate() rightfully does a set_64bit(), but if we're reading the
 * pmd_t with a "*pmdp" on the mincore side, a SMP race can happen
 * because GCC will not read the 64-bit value of the pmd atomically.
 *
 * To fix this all places running pte_offset_map_lock() while holding the
 * mmap_lock in read mode, shall read the pmdp pointer using this
 * function to know if the pmd is null or not, and in turn to know if
 * they can run pte_offset_map_lock() or pmd_trans_huge() or other pmd
 * operations.
 *
 * Without THP if the mmap_lock is held for reading, the pmd can only
 * transition from null to not null while pmd_read_atomic() runs. So
 * we can always return atomic pmd values with this function.
 *
 * With THP if the mmap_lock is held for reading, the pmd can become
 * trans_huge or none or point to a pte (and in turn become "stable")
 * at any time under pmd_read_atomic(). We could read it truly
 * atomically here with an atomic64_read() for the THP enabled case (and
 * it would be a whole lot simpler), but to avoid using cmpxchg8b we
 * only return an atomic pmdval if the low part of the pmdval is later
 * found to be stable (i.e. pointing to a pte). We are also returning a
 * 'none' (zero) pmdval if the low part of the pmd is zero.
 *
 * In some cases the high and low part of the pmdval returned may not be
 * consistent if THP is enabled (the low part may point to previously
 * mapped hugepage, while the high part may point to a more recently
 * mapped hugepage), but pmd_none_or_trans_huge_or_clear_bad() only
 * needs the low part of the pmd to be read atomically to decide if the
 * pmd is unstable or not, with the only exception when the low part
 * of the pmd is zero, in which case we return a 'none' pmd.
 */
static inline pmd_t pmd_read_atomic(pmd_t *pmdp)
{
	pmdval_t ret;
	u32 *tmp = (u32 *)pmdp;

	ret = (pmdval_t) (*tmp);
	if (ret) {
		/*
		 * If the low part is null, we must not read the high part
		 * or we can end up with a partial pmd.
		 */
		smp_rmb();
		ret |= ((pmdval_t)*(tmp + 1)) << 32;
	}

	return (pmd_t) { ret };
>>>>>>> b7ba80a49124 (Commit)
}

static inline void native_set_pte_atomic(pte_t *ptep, pte_t pte)
{
<<<<<<< HEAD
	pxx_xchg64(pte, ptep, native_pte_val(pte));
=======
	set_64bit((unsigned long long *)(ptep), native_pte_val(pte));
>>>>>>> b7ba80a49124 (Commit)
}

static inline void native_set_pmd(pmd_t *pmdp, pmd_t pmd)
{
<<<<<<< HEAD
	pxx_xchg64(pmd, pmdp, native_pmd_val(pmd));
=======
	set_64bit((unsigned long long *)(pmdp), native_pmd_val(pmd));
>>>>>>> b7ba80a49124 (Commit)
}

static inline void native_set_pud(pud_t *pudp, pud_t pud)
{
#ifdef CONFIG_PAGE_TABLE_ISOLATION
	pud.p4d.pgd = pti_set_user_pgtbl(&pudp->p4d.pgd, pud.p4d.pgd);
#endif
<<<<<<< HEAD
	pxx_xchg64(pud, pudp, native_pud_val(pud));
=======
	set_64bit((unsigned long long *)(pudp), native_pud_val(pud));
>>>>>>> b7ba80a49124 (Commit)
}

/*
 * For PTEs and PDEs, we must clear the P-bit first when clearing a page table
 * entry, so clear the bottom half first and enforce ordering with a compiler
 * barrier.
 */
static inline void native_pte_clear(struct mm_struct *mm, unsigned long addr,
				    pte_t *ptep)
{
<<<<<<< HEAD
	WRITE_ONCE(ptep->pte_low, 0);
	smp_wmb();
	WRITE_ONCE(ptep->pte_high, 0);
}

static inline void native_pmd_clear(pmd_t *pmdp)
{
	WRITE_ONCE(pmdp->pmd_low, 0);
	smp_wmb();
	WRITE_ONCE(pmdp->pmd_high, 0);
=======
	ptep->pte_low = 0;
	smp_wmb();
	ptep->pte_high = 0;
}

static inline void native_pmd_clear(pmd_t *pmd)
{
	u32 *tmp = (u32 *)pmd;
	*tmp = 0;
	smp_wmb();
	*(tmp + 1) = 0;
>>>>>>> b7ba80a49124 (Commit)
}

static inline void native_pud_clear(pud_t *pudp)
{
}

static inline void pud_clear(pud_t *pudp)
{
	set_pud(pudp, __pud(0));

	/*
	 * According to Intel App note "TLBs, Paging-Structure Caches,
	 * and Their Invalidation", April 2007, document 317080-001,
	 * section 8.1: in PAE mode we explicitly have to flush the
	 * TLB via cr3 if the top-level pgd is changed...
	 *
	 * Currently all places where pud_clear() is called either have
	 * flush_tlb_mm() followed or don't need TLB flush (x86_64 code or
	 * pud_clear_bad()), so we don't need TLB flush here.
	 */
}

<<<<<<< HEAD

#ifdef CONFIG_SMP
static inline pte_t native_ptep_get_and_clear(pte_t *ptep)
{
	return pxx_xchg64(pte, ptep, 0ULL);
}

static inline pmd_t native_pmdp_get_and_clear(pmd_t *pmdp)
{
	return pxx_xchg64(pmd, pmdp, 0ULL);
}

static inline pud_t native_pudp_get_and_clear(pud_t *pudp)
{
	return pxx_xchg64(pud, pudp, 0ULL);
}
#else
#define native_ptep_get_and_clear(xp) native_local_ptep_get_and_clear(xp)
#define native_pmdp_get_and_clear(xp) native_local_pmdp_get_and_clear(xp)
#define native_pudp_get_and_clear(xp) native_local_pudp_get_and_clear(xp)
=======
#ifdef CONFIG_SMP
static inline pte_t native_ptep_get_and_clear(pte_t *ptep)
{
	pte_t res;

	res.pte = (pteval_t)arch_atomic64_xchg((atomic64_t *)ptep, 0);

	return res;
}
#else
#define native_ptep_get_and_clear(xp) native_local_ptep_get_and_clear(xp)
#endif

union split_pmd {
	struct {
		u32 pmd_low;
		u32 pmd_high;
	};
	pmd_t pmd;
};

#ifdef CONFIG_SMP
static inline pmd_t native_pmdp_get_and_clear(pmd_t *pmdp)
{
	union split_pmd res, *orig = (union split_pmd *)pmdp;

	/* xchg acts as a barrier before setting of the high bits */
	res.pmd_low = xchg(&orig->pmd_low, 0);
	res.pmd_high = orig->pmd_high;
	orig->pmd_high = 0;

	return res.pmd;
}
#else
#define native_pmdp_get_and_clear(xp) native_local_pmdp_get_and_clear(xp)
>>>>>>> b7ba80a49124 (Commit)
#endif

#ifndef pmdp_establish
#define pmdp_establish pmdp_establish
static inline pmd_t pmdp_establish(struct vm_area_struct *vma,
		unsigned long address, pmd_t *pmdp, pmd_t pmd)
{
	pmd_t old;

	/*
	 * If pmd has present bit cleared we can get away without expensive
	 * cmpxchg64: we can update pmdp half-by-half without racing with
	 * anybody.
	 */
	if (!(pmd_val(pmd) & _PAGE_PRESENT)) {
<<<<<<< HEAD
		/* xchg acts as a barrier before setting of the high bits */
		old.pmd_low = xchg(&pmdp->pmd_low, pmd.pmd_low);
		old.pmd_high = READ_ONCE(pmdp->pmd_high);
		WRITE_ONCE(pmdp->pmd_high, pmd.pmd_high);

		return old;
	}

	return pxx_xchg64(pmd, pmdp, pmd.pmd);
}
#endif

/*
 * Encode/decode swap entries and swap PTEs. Swap PTEs are all PTEs that
 * are !pte_none() && !pte_present().
 *
 * Format of swap PTEs:
 *
 *   6 6 6 6 5 5 5 5 5 5 5 5 5 5 4 4 4 4 4 4 4 4 4 4 3 3 3 3 3 3 3 3
 *   3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2
 *   < type -> <---------------------- offset ----------------------
 *
 *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
 *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *   --------------------------------------------> 0 E 0 0 0 0 0 0 0
 *
 *   E is the exclusive marker that is not stored in swap entries.
 */
#define SWP_TYPE_BITS		5
#define _SWP_TYPE_MASK ((1U << SWP_TYPE_BITS) - 1)
=======
		union split_pmd old, new, *ptr;

		ptr = (union split_pmd *)pmdp;

		new.pmd = pmd;

		/* xchg acts as a barrier before setting of the high bits */
		old.pmd_low = xchg(&ptr->pmd_low, new.pmd_low);
		old.pmd_high = ptr->pmd_high;
		ptr->pmd_high = new.pmd_high;
		return old.pmd;
	}

	do {
		old = *pmdp;
	} while (cmpxchg64(&pmdp->pmd, old.pmd, pmd.pmd) != old.pmd);

	return old;
}
#endif

#ifdef CONFIG_SMP
union split_pud {
	struct {
		u32 pud_low;
		u32 pud_high;
	};
	pud_t pud;
};

static inline pud_t native_pudp_get_and_clear(pud_t *pudp)
{
	union split_pud res, *orig = (union split_pud *)pudp;

#ifdef CONFIG_PAGE_TABLE_ISOLATION
	pti_set_user_pgtbl(&pudp->p4d.pgd, __pgd(0));
#endif

	/* xchg acts as a barrier before setting of the high bits */
	res.pud_low = xchg(&orig->pud_low, 0);
	res.pud_high = orig->pud_high;
	orig->pud_high = 0;

	return res.pud;
}
#else
#define native_pudp_get_and_clear(xp) native_local_pudp_get_and_clear(xp)
#endif

/* Encode and de-code a swap entry */
#define SWP_TYPE_BITS		5
>>>>>>> b7ba80a49124 (Commit)

#define SWP_OFFSET_FIRST_BIT	(_PAGE_BIT_PROTNONE + 1)

/* We always extract/encode the offset by shifting it all the way up, and then down again */
#define SWP_OFFSET_SHIFT	(SWP_OFFSET_FIRST_BIT + SWP_TYPE_BITS)

#define MAX_SWAPFILES_CHECK() BUILD_BUG_ON(MAX_SWAPFILES_SHIFT > SWP_TYPE_BITS)
<<<<<<< HEAD
#define __swp_type(x)			(((x).val) & _SWP_TYPE_MASK)
#define __swp_offset(x)			((x).val >> SWP_TYPE_BITS)
#define __swp_entry(type, offset)	((swp_entry_t){((type) & _SWP_TYPE_MASK) \
					| (offset) << SWP_TYPE_BITS})
=======
#define __swp_type(x)			(((x).val) & ((1UL << SWP_TYPE_BITS) - 1))
#define __swp_offset(x)			((x).val >> SWP_TYPE_BITS)
#define __swp_entry(type, offset)	((swp_entry_t){(type) | (offset) << SWP_TYPE_BITS})
>>>>>>> b7ba80a49124 (Commit)

/*
 * Normally, __swp_entry() converts from arch-independent swp_entry_t to
 * arch-dependent swp_entry_t, and __swp_entry_to_pte() just stores the result
 * to pte. But here we have 32bit swp_entry_t and 64bit pte, and need to use the
 * whole 64 bits. Thus, we shift the "real" arch-dependent conversion to
 * __swp_entry_to_pte() through the following helper macro based on 64bit
 * __swp_entry().
 */
#define __swp_pteval_entry(type, offset) ((pteval_t) { \
	(~(pteval_t)(offset) << SWP_OFFSET_SHIFT >> SWP_TYPE_BITS) \
	| ((pteval_t)(type) << (64 - SWP_TYPE_BITS)) })

#define __swp_entry_to_pte(x)	((pte_t){ .pte = \
		__swp_pteval_entry(__swp_type(x), __swp_offset(x)) })
/*
 * Analogically, __pte_to_swp_entry() doesn't just extract the arch-dependent
 * swp_entry_t, but also has to convert it from 64bit to the 32bit
 * intermediate representation, using the following macros based on 64bit
 * __swp_type() and __swp_offset().
 */
#define __pteval_swp_type(x) ((unsigned long)((x).pte >> (64 - SWP_TYPE_BITS)))
#define __pteval_swp_offset(x) ((unsigned long)(~((x).pte) << SWP_TYPE_BITS >> SWP_OFFSET_SHIFT))

#define __pte_to_swp_entry(pte)	(__swp_entry(__pteval_swp_type(pte), \
					     __pteval_swp_offset(pte)))

<<<<<<< HEAD
/* We borrow bit 7 to store the exclusive marker in swap PTEs. */
#define _PAGE_SWP_EXCLUSIVE	_PAGE_PSE

=======
>>>>>>> b7ba80a49124 (Commit)
#include <asm/pgtable-invert.h>

#endif /* _ASM_X86_PGTABLE_3LEVEL_H */
