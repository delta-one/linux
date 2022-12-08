/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Common Primitives for Data Access Monitoring
 *
 * Author: SeongJae Park <sj@kernel.org>
 */

#include <linux/damon.h>

<<<<<<< HEAD
struct folio *damon_get_folio(unsigned long pfn);
=======
struct page *damon_get_page(unsigned long pfn);
>>>>>>> b7ba80a49124 (Commit)

void damon_ptep_mkold(pte_t *pte, struct mm_struct *mm, unsigned long addr);
void damon_pmdp_mkold(pmd_t *pmd, struct mm_struct *mm, unsigned long addr);

int damon_cold_score(struct damon_ctx *c, struct damon_region *r,
			struct damos *s);
int damon_hot_score(struct damon_ctx *c, struct damon_region *r,
			struct damos *s);
