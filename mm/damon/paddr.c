// SPDX-License-Identifier: GPL-2.0
/*
 * DAMON Primitives for The Physical Address Space
 *
 * Author: SeongJae Park <sj@kernel.org>
 */

#define pr_fmt(fmt) "damon-pa: " fmt

#include <linux/mmu_notifier.h>
#include <linux/page_idle.h>
#include <linux/pagemap.h>
#include <linux/rmap.h>
#include <linux/swap.h>

#include "../internal.h"
#include "ops-common.h"

static bool __damon_pa_mkold(struct folio *folio, struct vm_area_struct *vma,
		unsigned long addr, void *arg)
{
	DEFINE_FOLIO_VMA_WALK(pvmw, folio, vma, addr, 0);

	while (page_vma_mapped_walk(&pvmw)) {
		addr = pvmw.address;
		if (pvmw.pte)
			damon_ptep_mkold(pvmw.pte, vma->vm_mm, addr);
		else
			damon_pmdp_mkold(pvmw.pmd, vma->vm_mm, addr);
	}
	return true;
}

static void damon_pa_mkold(unsigned long paddr)
{
<<<<<<< HEAD
	struct folio *folio = damon_get_folio(PHYS_PFN(paddr));
=======
	struct folio *folio;
	struct page *page = damon_get_page(PHYS_PFN(paddr));
>>>>>>> b7ba80a49124 (Commit)
	struct rmap_walk_control rwc = {
		.rmap_one = __damon_pa_mkold,
		.anon_lock = folio_lock_anon_vma_read,
	};
	bool need_lock;

<<<<<<< HEAD
	if (!folio)
		return;
=======
	if (!page)
		return;
	folio = page_folio(page);
>>>>>>> b7ba80a49124 (Commit)

	if (!folio_mapped(folio) || !folio_raw_mapping(folio)) {
		folio_set_idle(folio);
		goto out;
	}

	need_lock = !folio_test_anon(folio) || folio_test_ksm(folio);
	if (need_lock && !folio_trylock(folio))
		goto out;

	rmap_walk(folio, &rwc);

	if (need_lock)
		folio_unlock(folio);

out:
	folio_put(folio);
}

static void __damon_pa_prepare_access_check(struct damon_region *r)
{
	r->sampling_addr = damon_rand(r->ar.start, r->ar.end);

	damon_pa_mkold(r->sampling_addr);
}

static void damon_pa_prepare_access_checks(struct damon_ctx *ctx)
{
	struct damon_target *t;
	struct damon_region *r;

	damon_for_each_target(t, ctx) {
		damon_for_each_region(r, t)
			__damon_pa_prepare_access_check(r);
	}
}

<<<<<<< HEAD
static bool __damon_pa_young(struct folio *folio, struct vm_area_struct *vma,
		unsigned long addr, void *arg)
{
	bool *accessed = arg;
	DEFINE_FOLIO_VMA_WALK(pvmw, folio, vma, addr, 0);

	*accessed = false;
	while (page_vma_mapped_walk(&pvmw)) {
		addr = pvmw.address;
		if (pvmw.pte) {
			*accessed = pte_young(*pvmw.pte) ||
=======
struct damon_pa_access_chk_result {
	unsigned long page_sz;
	bool accessed;
};

static bool __damon_pa_young(struct folio *folio, struct vm_area_struct *vma,
		unsigned long addr, void *arg)
{
	struct damon_pa_access_chk_result *result = arg;
	DEFINE_FOLIO_VMA_WALK(pvmw, folio, vma, addr, 0);

	result->accessed = false;
	result->page_sz = PAGE_SIZE;
	while (page_vma_mapped_walk(&pvmw)) {
		addr = pvmw.address;
		if (pvmw.pte) {
			result->accessed = pte_young(*pvmw.pte) ||
>>>>>>> b7ba80a49124 (Commit)
				!folio_test_idle(folio) ||
				mmu_notifier_test_young(vma->vm_mm, addr);
		} else {
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
<<<<<<< HEAD
			*accessed = pmd_young(*pvmw.pmd) ||
				!folio_test_idle(folio) ||
				mmu_notifier_test_young(vma->vm_mm, addr);
=======
			result->accessed = pmd_young(*pvmw.pmd) ||
				!folio_test_idle(folio) ||
				mmu_notifier_test_young(vma->vm_mm, addr);
			result->page_sz = HPAGE_PMD_SIZE;
>>>>>>> b7ba80a49124 (Commit)
#else
			WARN_ON_ONCE(1);
#endif	/* CONFIG_TRANSPARENT_HUGEPAGE */
		}
<<<<<<< HEAD
		if (*accessed) {
=======
		if (result->accessed) {
>>>>>>> b7ba80a49124 (Commit)
			page_vma_mapped_walk_done(&pvmw);
			break;
		}
	}

	/* If accessed, stop walking */
<<<<<<< HEAD
	return *accessed == false;
}

static bool damon_pa_young(unsigned long paddr, unsigned long *folio_sz)
{
	struct folio *folio = damon_get_folio(PHYS_PFN(paddr));
	bool accessed = false;
	struct rmap_walk_control rwc = {
		.arg = &accessed,
=======
	return !result->accessed;
}

static bool damon_pa_young(unsigned long paddr, unsigned long *page_sz)
{
	struct folio *folio;
	struct page *page = damon_get_page(PHYS_PFN(paddr));
	struct damon_pa_access_chk_result result = {
		.page_sz = PAGE_SIZE,
		.accessed = false,
	};
	struct rmap_walk_control rwc = {
		.arg = &result,
>>>>>>> b7ba80a49124 (Commit)
		.rmap_one = __damon_pa_young,
		.anon_lock = folio_lock_anon_vma_read,
	};
	bool need_lock;

<<<<<<< HEAD
	if (!folio)
		return false;

	if (!folio_mapped(folio) || !folio_raw_mapping(folio)) {
		if (folio_test_idle(folio))
			accessed = false;
		else
			accessed = true;
=======
	if (!page)
		return false;
	folio = page_folio(page);

	if (!folio_mapped(folio) || !folio_raw_mapping(folio)) {
		if (folio_test_idle(folio))
			result.accessed = false;
		else
			result.accessed = true;
		folio_put(folio);
>>>>>>> b7ba80a49124 (Commit)
		goto out;
	}

	need_lock = !folio_test_anon(folio) || folio_test_ksm(folio);
<<<<<<< HEAD
	if (need_lock && !folio_trylock(folio))
		goto out;
=======
	if (need_lock && !folio_trylock(folio)) {
		folio_put(folio);
		return false;
	}
>>>>>>> b7ba80a49124 (Commit)

	rmap_walk(folio, &rwc);

	if (need_lock)
		folio_unlock(folio);
<<<<<<< HEAD

out:
	*folio_sz = folio_size(folio);
	folio_put(folio);
	return accessed;
=======
	folio_put(folio);

out:
	*page_sz = result.page_sz;
	return result.accessed;
>>>>>>> b7ba80a49124 (Commit)
}

static void __damon_pa_check_access(struct damon_region *r)
{
	static unsigned long last_addr;
<<<<<<< HEAD
	static unsigned long last_folio_sz = PAGE_SIZE;
	static bool last_accessed;

	/* If the region is in the last checked page, reuse the result */
	if (ALIGN_DOWN(last_addr, last_folio_sz) ==
				ALIGN_DOWN(r->sampling_addr, last_folio_sz)) {
=======
	static unsigned long last_page_sz = PAGE_SIZE;
	static bool last_accessed;

	/* If the region is in the last checked page, reuse the result */
	if (ALIGN_DOWN(last_addr, last_page_sz) ==
				ALIGN_DOWN(r->sampling_addr, last_page_sz)) {
>>>>>>> b7ba80a49124 (Commit)
		if (last_accessed)
			r->nr_accesses++;
		return;
	}

<<<<<<< HEAD
	last_accessed = damon_pa_young(r->sampling_addr, &last_folio_sz);
=======
	last_accessed = damon_pa_young(r->sampling_addr, &last_page_sz);
>>>>>>> b7ba80a49124 (Commit)
	if (last_accessed)
		r->nr_accesses++;

	last_addr = r->sampling_addr;
}

static unsigned int damon_pa_check_accesses(struct damon_ctx *ctx)
{
	struct damon_target *t;
	struct damon_region *r;
	unsigned int max_nr_accesses = 0;

	damon_for_each_target(t, ctx) {
		damon_for_each_region(r, t) {
			__damon_pa_check_access(r);
			max_nr_accesses = max(r->nr_accesses, max_nr_accesses);
		}
	}

	return max_nr_accesses;
}

<<<<<<< HEAD
static bool __damos_pa_filter_out(struct damos_filter *filter,
		struct folio *folio)
{
	bool matched = false;
	struct mem_cgroup *memcg;

	switch (filter->type) {
	case DAMOS_FILTER_TYPE_ANON:
		matched = folio_test_anon(folio);
		break;
	case DAMOS_FILTER_TYPE_MEMCG:
		rcu_read_lock();
		memcg = folio_memcg_check(folio);
		if (!memcg)
			matched = false;
		else
			matched = filter->memcg_id == mem_cgroup_id(memcg);
		rcu_read_unlock();
		break;
	default:
		break;
	}

	return matched == filter->matching;
}

/*
 * damos_pa_filter_out - Return true if the page should be filtered out.
 */
static bool damos_pa_filter_out(struct damos *scheme, struct folio *folio)
{
	struct damos_filter *filter;

	damos_for_each_filter(filter, scheme) {
		if (__damos_pa_filter_out(filter, folio))
			return true;
	}
	return false;
}

static unsigned long damon_pa_pageout(struct damon_region *r, struct damos *s)
{
	unsigned long addr, applied;
	LIST_HEAD(folio_list);

	for (addr = r->ar.start; addr < r->ar.end; addr += PAGE_SIZE) {
		struct folio *folio = damon_get_folio(PHYS_PFN(addr));

		if (!folio)
			continue;

		if (damos_pa_filter_out(s, folio))
			goto put_folio;

		folio_clear_referenced(folio);
		folio_test_clear_young(folio);
		if (!folio_isolate_lru(folio))
			goto put_folio;
		if (folio_test_unevictable(folio))
			folio_putback_lru(folio);
		else
			list_add(&folio->lru, &folio_list);
put_folio:
		folio_put(folio);
	}
	applied = reclaim_pages(&folio_list);
=======
static unsigned long damon_pa_pageout(struct damon_region *r)
{
	unsigned long addr, applied;
	LIST_HEAD(page_list);

	for (addr = r->ar.start; addr < r->ar.end; addr += PAGE_SIZE) {
		struct page *page = damon_get_page(PHYS_PFN(addr));

		if (!page)
			continue;

		ClearPageReferenced(page);
		test_and_clear_page_young(page);
		if (isolate_lru_page(page)) {
			put_page(page);
			continue;
		}
		if (PageUnevictable(page)) {
			putback_lru_page(page);
		} else {
			list_add(&page->lru, &page_list);
			put_page(page);
		}
	}
	applied = reclaim_pages(&page_list);
>>>>>>> b7ba80a49124 (Commit)
	cond_resched();
	return applied * PAGE_SIZE;
}

static inline unsigned long damon_pa_mark_accessed_or_deactivate(
<<<<<<< HEAD
		struct damon_region *r, struct damos *s, bool mark_accessed)
=======
		struct damon_region *r, bool mark_accessed)
>>>>>>> b7ba80a49124 (Commit)
{
	unsigned long addr, applied = 0;

	for (addr = r->ar.start; addr < r->ar.end; addr += PAGE_SIZE) {
<<<<<<< HEAD
		struct folio *folio = damon_get_folio(PHYS_PFN(addr));

		if (!folio)
			continue;

		if (damos_pa_filter_out(s, folio))
			goto put_folio;

		if (mark_accessed)
			folio_mark_accessed(folio);
		else
			folio_deactivate(folio);
		applied += folio_nr_pages(folio);
put_folio:
		folio_put(folio);
=======
		struct page *page = damon_get_page(PHYS_PFN(addr));

		if (!page)
			continue;
		if (mark_accessed)
			mark_page_accessed(page);
		else
			deactivate_page(page);
		put_page(page);
		applied++;
>>>>>>> b7ba80a49124 (Commit)
	}
	return applied * PAGE_SIZE;
}

<<<<<<< HEAD
static unsigned long damon_pa_mark_accessed(struct damon_region *r,
	struct damos *s)
{
	return damon_pa_mark_accessed_or_deactivate(r, s, true);
}

static unsigned long damon_pa_deactivate_pages(struct damon_region *r,
	struct damos *s)
{
	return damon_pa_mark_accessed_or_deactivate(r, s, false);
=======
static unsigned long damon_pa_mark_accessed(struct damon_region *r)
{
	return damon_pa_mark_accessed_or_deactivate(r, true);
}

static unsigned long damon_pa_deactivate_pages(struct damon_region *r)
{
	return damon_pa_mark_accessed_or_deactivate(r, false);
>>>>>>> b7ba80a49124 (Commit)
}

static unsigned long damon_pa_apply_scheme(struct damon_ctx *ctx,
		struct damon_target *t, struct damon_region *r,
		struct damos *scheme)
{
	switch (scheme->action) {
	case DAMOS_PAGEOUT:
<<<<<<< HEAD
		return damon_pa_pageout(r, scheme);
	case DAMOS_LRU_PRIO:
		return damon_pa_mark_accessed(r, scheme);
	case DAMOS_LRU_DEPRIO:
		return damon_pa_deactivate_pages(r, scheme);
=======
		return damon_pa_pageout(r);
	case DAMOS_LRU_PRIO:
		return damon_pa_mark_accessed(r);
	case DAMOS_LRU_DEPRIO:
		return damon_pa_deactivate_pages(r);
>>>>>>> b7ba80a49124 (Commit)
	case DAMOS_STAT:
		break;
	default:
		/* DAMOS actions that not yet supported by 'paddr'. */
		break;
	}
	return 0;
}

static int damon_pa_scheme_score(struct damon_ctx *context,
		struct damon_target *t, struct damon_region *r,
		struct damos *scheme)
{
	switch (scheme->action) {
	case DAMOS_PAGEOUT:
		return damon_cold_score(context, r, scheme);
	case DAMOS_LRU_PRIO:
		return damon_hot_score(context, r, scheme);
	case DAMOS_LRU_DEPRIO:
		return damon_cold_score(context, r, scheme);
	default:
		break;
	}

	return DAMOS_MAX_SCORE;
}

static int __init damon_pa_initcall(void)
{
	struct damon_operations ops = {
		.id = DAMON_OPS_PADDR,
		.init = NULL,
		.update = NULL,
		.prepare_access_checks = damon_pa_prepare_access_checks,
		.check_accesses = damon_pa_check_accesses,
		.reset_aggregated = NULL,
		.target_valid = NULL,
		.cleanup = NULL,
		.apply_scheme = damon_pa_apply_scheme,
		.get_scheme_score = damon_pa_scheme_score,
	};

	return damon_register_ops(&ops);
};

subsys_initcall(damon_pa_initcall);
