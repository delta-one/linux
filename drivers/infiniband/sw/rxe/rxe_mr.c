// SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB
/*
 * Copyright (c) 2016 Mellanox Technologies Ltd. All rights reserved.
 * Copyright (c) 2015 System Fabric Works, Inc. All rights reserved.
 */

<<<<<<< HEAD
#include <linux/libnvdimm.h>

=======
>>>>>>> b7ba80a49124 (Commit)
#include "rxe.h"
#include "rxe_loc.h"

/* Return a random 8 bit key value that is
 * different than the last_key. Set last_key to -1
 * if this is the first key for an MR or MW
 */
u8 rxe_get_next_key(u32 last_key)
{
	u8 key;

	do {
		get_random_bytes(&key, 1);
	} while (key == last_key);

	return key;
}

int mr_check_range(struct rxe_mr *mr, u64 iova, size_t length)
{
<<<<<<< HEAD
	switch (mr->ibmr.type) {
=======


	switch (mr->type) {
>>>>>>> b7ba80a49124 (Commit)
	case IB_MR_TYPE_DMA:
		return 0;

	case IB_MR_TYPE_USER:
	case IB_MR_TYPE_MEM_REG:
<<<<<<< HEAD
		if (iova < mr->ibmr.iova ||
		    iova + length > mr->ibmr.iova + mr->ibmr.length) {
			rxe_dbg_mr(mr, "iova/length out of range");
			return -EINVAL;
		}
		return 0;

	default:
		rxe_dbg_mr(mr, "mr type not supported\n");
		return -EINVAL;
=======
		if (iova < mr->iova || length > mr->length ||
		    iova > mr->iova + mr->length - length)
			return -EFAULT;
		return 0;

	default:
		pr_warn("%s: mr type (%d) not supported\n",
			__func__, mr->type);
		return -EFAULT;
>>>>>>> b7ba80a49124 (Commit)
	}
}

#define IB_ACCESS_REMOTE	(IB_ACCESS_REMOTE_READ		\
				| IB_ACCESS_REMOTE_WRITE	\
				| IB_ACCESS_REMOTE_ATOMIC)

static void rxe_mr_init(int access, struct rxe_mr *mr)
{
	u32 lkey = mr->elem.index << 8 | rxe_get_next_key(-1);
	u32 rkey = (access & IB_ACCESS_REMOTE) ? lkey : 0;

	/* set ibmr->l/rkey and also copy into private l/rkey
	 * for user MRs these will always be the same
	 * for cases where caller 'owns' the key portion
	 * they may be different until REG_MR WQE is executed.
	 */
	mr->lkey = mr->ibmr.lkey = lkey;
	mr->rkey = mr->ibmr.rkey = rkey;

<<<<<<< HEAD
	mr->access = access;
	mr->ibmr.page_size = PAGE_SIZE;
	mr->page_mask = PAGE_MASK;
	mr->page_shift = PAGE_SHIFT;
	mr->state = RXE_MR_STATE_INVALID;
}

void rxe_mr_init_dma(int access, struct rxe_mr *mr)
{
	rxe_mr_init(access, mr);

	mr->state = RXE_MR_STATE_VALID;
	mr->ibmr.type = IB_MR_TYPE_DMA;
}

static unsigned long rxe_mr_iova_to_index(struct rxe_mr *mr, u64 iova)
{
	return (iova >> mr->page_shift) - (mr->ibmr.iova >> mr->page_shift);
}

static unsigned long rxe_mr_iova_to_page_offset(struct rxe_mr *mr, u64 iova)
{
	return iova & (mr_page_size(mr) - 1);
}

static bool is_pmem_page(struct page *pg)
{
	unsigned long paddr = page_to_phys(pg);

	return REGION_INTERSECTS ==
	       region_intersects(paddr, PAGE_SIZE, IORESOURCE_MEM,
				 IORES_DESC_PERSISTENT_MEMORY);
}

static int rxe_mr_fill_pages_from_sgt(struct rxe_mr *mr, struct sg_table *sgt)
{
	XA_STATE(xas, &mr->page_list, 0);
	struct sg_page_iter sg_iter;
	struct page *page;
	bool persistent = !!(mr->access & IB_ACCESS_FLUSH_PERSISTENT);

	__sg_page_iter_start(&sg_iter, sgt->sgl, sgt->orig_nents, 0);
	if (!__sg_page_iter_next(&sg_iter))
		return 0;

	do {
		xas_lock(&xas);
		while (true) {
			page = sg_page_iter_page(&sg_iter);

			if (persistent && !is_pmem_page(page)) {
				rxe_dbg_mr(mr, "Page can't be persistent\n");
				xas_set_err(&xas, -EINVAL);
				break;
			}

			xas_store(&xas, page);
			if (xas_error(&xas))
				break;
			xas_next(&xas);
			if (!__sg_page_iter_next(&sg_iter))
				break;
		}
		xas_unlock(&xas);
	} while (xas_nomem(&xas, GFP_KERNEL));

	return xas_error(&xas);
}

int rxe_mr_init_user(struct rxe_dev *rxe, u64 start, u64 length, u64 iova,
		     int access, struct rxe_mr *mr)
{
	struct ib_umem *umem;
	int err;

	rxe_mr_init(access, mr);

	xa_init(&mr->page_list);

	umem = ib_umem_get(&rxe->ib_dev, start, length, access);
	if (IS_ERR(umem)) {
		rxe_dbg_mr(mr, "Unable to pin memory region err = %d\n",
			(int)PTR_ERR(umem));
		return PTR_ERR(umem);
	}

	err = rxe_mr_fill_pages_from_sgt(mr, &umem->sgt_append.sgt);
	if (err) {
		ib_umem_release(umem);
		return err;
	}

	mr->umem = umem;
	mr->ibmr.type = IB_MR_TYPE_USER;
	mr->state = RXE_MR_STATE_VALID;

	return 0;
=======
	mr->state = RXE_MR_STATE_INVALID;
	mr->map_shift = ilog2(RXE_BUF_PER_MAP);
>>>>>>> b7ba80a49124 (Commit)
}

static int rxe_mr_alloc(struct rxe_mr *mr, int num_buf)
{
<<<<<<< HEAD
	XA_STATE(xas, &mr->page_list, 0);
	int i = 0;
	int err;

	xa_init(&mr->page_list);

	do {
		xas_lock(&xas);
		while (i != num_buf) {
			xas_store(&xas, XA_ZERO_ENTRY);
			if (xas_error(&xas))
				break;
			xas_next(&xas);
			i++;
		}
		xas_unlock(&xas);
	} while (xas_nomem(&xas, GFP_KERNEL));

	err = xas_error(&xas);
	if (err)
		return err;

	mr->num_buf = num_buf;

	return 0;
}

int rxe_mr_init_fast(int max_pages, struct rxe_mr *mr)
=======
	int i;
	int num_map;
	struct rxe_map **map = mr->map;

	num_map = (num_buf + RXE_BUF_PER_MAP - 1) / RXE_BUF_PER_MAP;

	mr->map = kmalloc_array(num_map, sizeof(*map), GFP_KERNEL);
	if (!mr->map)
		goto err1;

	for (i = 0; i < num_map; i++) {
		mr->map[i] = kmalloc(sizeof(**map), GFP_KERNEL);
		if (!mr->map[i])
			goto err2;
	}

	BUILD_BUG_ON(!is_power_of_2(RXE_BUF_PER_MAP));

	mr->map_shift = ilog2(RXE_BUF_PER_MAP);
	mr->map_mask = RXE_BUF_PER_MAP - 1;

	mr->num_buf = num_buf;
	mr->num_map = num_map;
	mr->max_buf = num_map * RXE_BUF_PER_MAP;

	return 0;

err2:
	for (i--; i >= 0; i--)
		kfree(mr->map[i]);

	kfree(mr->map);
err1:
	return -ENOMEM;
}

void rxe_mr_init_dma(struct rxe_pd *pd, int access, struct rxe_mr *mr)
{
	rxe_mr_init(access, mr);

	mr->ibmr.pd = &pd->ibpd;
	mr->access = access;
	mr->state = RXE_MR_STATE_VALID;
	mr->type = IB_MR_TYPE_DMA;
}

int rxe_mr_init_user(struct rxe_pd *pd, u64 start, u64 length, u64 iova,
		     int access, struct rxe_mr *mr)
{
	struct rxe_map		**map;
	struct rxe_phys_buf	*buf = NULL;
	struct ib_umem		*umem;
	struct sg_page_iter	sg_iter;
	int			num_buf;
	void			*vaddr;
	int err;
	int i;

	umem = ib_umem_get(pd->ibpd.device, start, length, access);
	if (IS_ERR(umem)) {
		pr_warn("%s: Unable to pin memory region err = %d\n",
			__func__, (int)PTR_ERR(umem));
		err = PTR_ERR(umem);
		goto err_out;
	}

	num_buf = ib_umem_num_pages(umem);

	rxe_mr_init(access, mr);

	err = rxe_mr_alloc(mr, num_buf);
	if (err) {
		pr_warn("%s: Unable to allocate memory for map\n",
				__func__);
		goto err_release_umem;
	}

	mr->page_shift = PAGE_SHIFT;
	mr->page_mask = PAGE_SIZE - 1;

	num_buf			= 0;
	map = mr->map;
	if (length > 0) {
		buf = map[0]->buf;

		for_each_sgtable_page (&umem->sgt_append.sgt, &sg_iter, 0) {
			if (num_buf >= RXE_BUF_PER_MAP) {
				map++;
				buf = map[0]->buf;
				num_buf = 0;
			}

			vaddr = page_address(sg_page_iter_page(&sg_iter));
			if (!vaddr) {
				pr_warn("%s: Unable to get virtual address\n",
						__func__);
				err = -ENOMEM;
				goto err_cleanup_map;
			}

			buf->addr = (uintptr_t)vaddr;
			buf->size = PAGE_SIZE;
			num_buf++;
			buf++;

		}
	}

	mr->ibmr.pd = &pd->ibpd;
	mr->umem = umem;
	mr->access = access;
	mr->length = length;
	mr->iova = iova;
	mr->offset = ib_umem_offset(umem);
	mr->state = RXE_MR_STATE_VALID;
	mr->type = IB_MR_TYPE_USER;

	return 0;

err_cleanup_map:
	for (i = 0; i < mr->num_map; i++)
		kfree(mr->map[i]);
	kfree(mr->map);
err_release_umem:
	ib_umem_release(umem);
err_out:
	return err;
}

int rxe_mr_init_fast(struct rxe_pd *pd, int max_pages, struct rxe_mr *mr)
>>>>>>> b7ba80a49124 (Commit)
{
	int err;

	/* always allow remote access for FMRs */
	rxe_mr_init(IB_ACCESS_REMOTE, mr);

	err = rxe_mr_alloc(mr, max_pages);
	if (err)
		goto err1;

<<<<<<< HEAD
	mr->state = RXE_MR_STATE_FREE;
	mr->ibmr.type = IB_MR_TYPE_MEM_REG;
=======
	mr->ibmr.pd = &pd->ibpd;
	mr->max_buf = max_pages;
	mr->state = RXE_MR_STATE_FREE;
	mr->type = IB_MR_TYPE_MEM_REG;
>>>>>>> b7ba80a49124 (Commit)

	return 0;

err1:
	return err;
}

<<<<<<< HEAD
static int rxe_set_page(struct ib_mr *ibmr, u64 iova)
{
	struct rxe_mr *mr = to_rmr(ibmr);
	struct page *page = virt_to_page(iova & mr->page_mask);
	bool persistent = !!(mr->access & IB_ACCESS_FLUSH_PERSISTENT);
	int err;

	if (persistent && !is_pmem_page(page)) {
		rxe_dbg_mr(mr, "Page cannot be persistent\n");
		return -EINVAL;
	}

	if (unlikely(mr->nbuf == mr->num_buf))
		return -ENOMEM;

	err = xa_err(xa_store(&mr->page_list, mr->nbuf, page, GFP_KERNEL));
	if (err)
		return err;

	mr->nbuf++;
	return 0;
}

int rxe_map_mr_sg(struct ib_mr *ibmr, struct scatterlist *sgl,
		  int sg_nents, unsigned int *sg_offset)
{
	struct rxe_mr *mr = to_rmr(ibmr);
	unsigned int page_size = mr_page_size(mr);

	mr->nbuf = 0;
	mr->page_shift = ilog2(page_size);
	mr->page_mask = ~((u64)page_size - 1);
	mr->page_offset = mr->ibmr.iova & (page_size - 1);

	return ib_sg_to_pages(ibmr, sgl, sg_nents, sg_offset, rxe_set_page);
}

static int rxe_mr_copy_xarray(struct rxe_mr *mr, u64 iova, void *addr,
			      unsigned int length, enum rxe_mr_copy_dir dir)
{
	unsigned int page_offset = rxe_mr_iova_to_page_offset(mr, iova);
	unsigned long index = rxe_mr_iova_to_index(mr, iova);
	unsigned int bytes;
	struct page *page;
	void *va;

	while (length) {
		page = xa_load(&mr->page_list, index);
		if (!page)
			return -EFAULT;

		bytes = min_t(unsigned int, length,
				mr_page_size(mr) - page_offset);
		va = kmap_local_page(page);
		if (dir == RXE_FROM_MR_OBJ)
			memcpy(addr, va + page_offset, bytes);
		else
			memcpy(va + page_offset, addr, bytes);
		kunmap_local(va);

		page_offset = 0;
		addr += bytes;
		length -= bytes;
		index++;
	}

	return 0;
}

static void rxe_mr_copy_dma(struct rxe_mr *mr, u64 iova, void *addr,
			    unsigned int length, enum rxe_mr_copy_dir dir)
{
	unsigned int page_offset = iova & (PAGE_SIZE - 1);
	unsigned int bytes;
	struct page *page;
	u8 *va;

	while (length) {
		page = virt_to_page(iova & mr->page_mask);
		bytes = min_t(unsigned int, length,
				PAGE_SIZE - page_offset);
		va = kmap_local_page(page);

		if (dir == RXE_TO_MR_OBJ)
			memcpy(va + page_offset, addr, bytes);
		else
			memcpy(addr, va + page_offset, bytes);

		kunmap_local(va);
		page_offset = 0;
		iova += bytes;
		addr += bytes;
		length -= bytes;
	}
}

int rxe_mr_copy(struct rxe_mr *mr, u64 iova, void *addr,
		unsigned int length, enum rxe_mr_copy_dir dir)
{
	int err;
=======
static void lookup_iova(struct rxe_mr *mr, u64 iova, int *m_out, int *n_out,
			size_t *offset_out)
{
	size_t offset = iova - mr->iova + mr->offset;
	int			map_index;
	int			buf_index;
	u64			length;

	if (likely(mr->page_shift)) {
		*offset_out = offset & mr->page_mask;
		offset >>= mr->page_shift;
		*n_out = offset & mr->map_mask;
		*m_out = offset >> mr->map_shift;
	} else {
		map_index = 0;
		buf_index = 0;

		length = mr->map[map_index]->buf[buf_index].size;

		while (offset >= length) {
			offset -= length;
			buf_index++;

			if (buf_index == RXE_BUF_PER_MAP) {
				map_index++;
				buf_index = 0;
			}
			length = mr->map[map_index]->buf[buf_index].size;
		}

		*m_out = map_index;
		*n_out = buf_index;
		*offset_out = offset;
	}
}

void *iova_to_vaddr(struct rxe_mr *mr, u64 iova, int length)
{
	size_t offset;
	int m, n;
	void *addr;

	if (mr->state != RXE_MR_STATE_VALID) {
		pr_warn("mr not in valid state\n");
		addr = NULL;
		goto out;
	}

	if (!mr->map) {
		addr = (void *)(uintptr_t)iova;
		goto out;
	}

	if (mr_check_range(mr, iova, length)) {
		pr_warn("range violation\n");
		addr = NULL;
		goto out;
	}

	lookup_iova(mr, iova, &m, &n, &offset);

	if (offset + length > mr->map[m]->buf[n].size) {
		pr_warn("crosses page boundary\n");
		addr = NULL;
		goto out;
	}

	addr = (void *)(uintptr_t)mr->map[m]->buf[n].addr + offset;

out:
	return addr;
}

/* copy data from a range (vaddr, vaddr+length-1) to or from
 * a mr object starting at iova.
 */
int rxe_mr_copy(struct rxe_mr *mr, u64 iova, void *addr, int length,
		enum rxe_mr_copy_dir dir)
{
	int			err;
	int			bytes;
	u8			*va;
	struct rxe_map		**map;
	struct rxe_phys_buf	*buf;
	int			m;
	int			i;
	size_t			offset;
>>>>>>> b7ba80a49124 (Commit)

	if (length == 0)
		return 0;

<<<<<<< HEAD
	if (WARN_ON(!mr))
		return -EINVAL;

	if (mr->ibmr.type == IB_MR_TYPE_DMA) {
		rxe_mr_copy_dma(mr, iova, addr, length, dir);
		return 0;
	}

	err = mr_check_range(mr, iova, length);
	if (unlikely(err)) {
		rxe_dbg_mr(mr, "iova out of range");
		return err;
	}

	return rxe_mr_copy_xarray(mr, iova, addr, length, dir);
=======
	if (mr->type == IB_MR_TYPE_DMA) {
		u8 *src, *dest;

		src = (dir == RXE_TO_MR_OBJ) ? addr : ((void *)(uintptr_t)iova);

		dest = (dir == RXE_TO_MR_OBJ) ? ((void *)(uintptr_t)iova) : addr;

		memcpy(dest, src, length);

		return 0;
	}

	WARN_ON_ONCE(!mr->map);

	err = mr_check_range(mr, iova, length);
	if (err) {
		err = -EFAULT;
		goto err1;
	}

	lookup_iova(mr, iova, &m, &i, &offset);

	map = mr->map + m;
	buf	= map[0]->buf + i;

	while (length > 0) {
		u8 *src, *dest;

		va	= (u8 *)(uintptr_t)buf->addr + offset;
		src = (dir == RXE_TO_MR_OBJ) ? addr : va;
		dest = (dir == RXE_TO_MR_OBJ) ? va : addr;

		bytes	= buf->size - offset;

		if (bytes > length)
			bytes = length;

		memcpy(dest, src, bytes);

		length	-= bytes;
		addr	+= bytes;

		offset	= 0;
		buf++;
		i++;

		if (i == RXE_BUF_PER_MAP) {
			i = 0;
			map++;
			buf = map[0]->buf;
		}
	}

	return 0;

err1:
	return err;
>>>>>>> b7ba80a49124 (Commit)
}

/* copy data in or out of a wqe, i.e. sg list
 * under the control of a dma descriptor
 */
int copy_data(
	struct rxe_pd		*pd,
	int			access,
	struct rxe_dma_info	*dma,
	void			*addr,
	int			length,
	enum rxe_mr_copy_dir	dir)
{
	int			bytes;
	struct rxe_sge		*sge	= &dma->sge[dma->cur_sge];
	int			offset	= dma->sge_offset;
	int			resid	= dma->resid;
	struct rxe_mr		*mr	= NULL;
	u64			iova;
	int			err;

	if (length == 0)
		return 0;

	if (length > resid) {
		err = -EINVAL;
		goto err2;
	}

	if (sge->length && (offset < sge->length)) {
		mr = lookup_mr(pd, access, sge->lkey, RXE_LOOKUP_LOCAL);
		if (!mr) {
			err = -EINVAL;
			goto err1;
		}
	}

	while (length > 0) {
		bytes = length;

		if (offset >= sge->length) {
			if (mr) {
				rxe_put(mr);
				mr = NULL;
			}
			sge++;
			dma->cur_sge++;
			offset = 0;

			if (dma->cur_sge >= dma->num_sge) {
				err = -ENOSPC;
				goto err2;
			}

			if (sge->length) {
				mr = lookup_mr(pd, access, sge->lkey,
					       RXE_LOOKUP_LOCAL);
				if (!mr) {
					err = -EINVAL;
					goto err1;
				}
			} else {
				continue;
			}
		}

		if (bytes > sge->length - offset)
			bytes = sge->length - offset;

		if (bytes > 0) {
			iova = sge->addr + offset;
<<<<<<< HEAD
=======

>>>>>>> b7ba80a49124 (Commit)
			err = rxe_mr_copy(mr, iova, addr, bytes, dir);
			if (err)
				goto err2;

			offset	+= bytes;
			resid	-= bytes;
			length	-= bytes;
			addr	+= bytes;
		}
	}

	dma->sge_offset = offset;
	dma->resid	= resid;

	if (mr)
		rxe_put(mr);

	return 0;

err2:
	if (mr)
		rxe_put(mr);
err1:
	return err;
}

<<<<<<< HEAD
int rxe_flush_pmem_iova(struct rxe_mr *mr, u64 iova, unsigned int length)
{
	unsigned int page_offset;
	unsigned long index;
	struct page *page;
	unsigned int bytes;
	int err;
	u8 *va;

	/* mr must be valid even if length is zero */
	if (WARN_ON(!mr))
		return -EINVAL;

	if (length == 0)
		return 0;

	if (mr->ibmr.type == IB_MR_TYPE_DMA)
		return -EFAULT;

	err = mr_check_range(mr, iova, length);
	if (err)
		return err;

	while (length > 0) {
		index = rxe_mr_iova_to_index(mr, iova);
		page = xa_load(&mr->page_list, index);
		page_offset = rxe_mr_iova_to_page_offset(mr, iova);
		if (!page)
			return -EFAULT;
		bytes = min_t(unsigned int, length,
				mr_page_size(mr) - page_offset);

		va = kmap_local_page(page);
		arch_wb_cache_pmem(va + page_offset, bytes);
		kunmap_local(va);

		length -= bytes;
		iova += bytes;
		page_offset = 0;
	}

	return 0;
}

/* Guarantee atomicity of atomic operations at the machine level. */
static DEFINE_SPINLOCK(atomic_ops_lock);

int rxe_mr_do_atomic_op(struct rxe_mr *mr, u64 iova, int opcode,
			u64 compare, u64 swap_add, u64 *orig_val)
{
	unsigned int page_offset;
	struct page *page;
	u64 value;
	u64 *va;

	if (unlikely(mr->state != RXE_MR_STATE_VALID)) {
		rxe_dbg_mr(mr, "mr not in valid state");
		return RESPST_ERR_RKEY_VIOLATION;
	}

	if (mr->ibmr.type == IB_MR_TYPE_DMA) {
		page_offset = iova & (PAGE_SIZE - 1);
		page = virt_to_page(iova & PAGE_MASK);
	} else {
		unsigned long index;
		int err;

		err = mr_check_range(mr, iova, sizeof(value));
		if (err) {
			rxe_dbg_mr(mr, "iova out of range");
			return RESPST_ERR_RKEY_VIOLATION;
		}
		page_offset = rxe_mr_iova_to_page_offset(mr, iova);
		index = rxe_mr_iova_to_index(mr, iova);
		page = xa_load(&mr->page_list, index);
		if (!page)
			return RESPST_ERR_RKEY_VIOLATION;
	}

	if (unlikely(page_offset & 0x7)) {
		rxe_dbg_mr(mr, "iova not aligned");
		return RESPST_ERR_MISALIGNED_ATOMIC;
	}

	va = kmap_local_page(page);

	spin_lock_bh(&atomic_ops_lock);
	value = *orig_val = va[page_offset >> 3];

	if (opcode == IB_OPCODE_RC_COMPARE_SWAP) {
		if (value == compare)
			va[page_offset >> 3] = swap_add;
	} else {
		value += swap_add;
		va[page_offset >> 3] = value;
	}
	spin_unlock_bh(&atomic_ops_lock);

	kunmap_local(va);

	return 0;
}

#if defined CONFIG_64BIT
/* only implemented or called for 64 bit architectures */
int rxe_mr_do_atomic_write(struct rxe_mr *mr, u64 iova, u64 value)
{
	unsigned int page_offset;
	struct page *page;
	u64 *va;

	/* See IBA oA19-28 */
	if (unlikely(mr->state != RXE_MR_STATE_VALID)) {
		rxe_dbg_mr(mr, "mr not in valid state");
		return RESPST_ERR_RKEY_VIOLATION;
	}

	if (mr->ibmr.type == IB_MR_TYPE_DMA) {
		page_offset = iova & (PAGE_SIZE - 1);
		page = virt_to_page(iova & PAGE_MASK);
	} else {
		unsigned long index;
		int err;

		/* See IBA oA19-28 */
		err = mr_check_range(mr, iova, sizeof(value));
		if (unlikely(err)) {
			rxe_dbg_mr(mr, "iova out of range");
			return RESPST_ERR_RKEY_VIOLATION;
		}
		page_offset = rxe_mr_iova_to_page_offset(mr, iova);
		index = rxe_mr_iova_to_index(mr, iova);
		page = xa_load(&mr->page_list, index);
		if (!page)
			return RESPST_ERR_RKEY_VIOLATION;
	}

	/* See IBA A19.4.2 */
	if (unlikely(page_offset & 0x7)) {
		rxe_dbg_mr(mr, "misaligned address");
		return RESPST_ERR_MISALIGNED_ATOMIC;
	}

	va = kmap_local_page(page);

	/* Do atomic write after all prior operations have completed */
	smp_store_release(&va[page_offset >> 3], value);

	kunmap_local(va);

	return 0;
}
#else
int rxe_mr_do_atomic_write(struct rxe_mr *mr, u64 iova, u64 value)
{
	return RESPST_ERR_UNSUPPORTED_OPCODE;
}
#endif

=======
>>>>>>> b7ba80a49124 (Commit)
int advance_dma_data(struct rxe_dma_info *dma, unsigned int length)
{
	struct rxe_sge		*sge	= &dma->sge[dma->cur_sge];
	int			offset	= dma->sge_offset;
	int			resid	= dma->resid;

	while (length) {
		unsigned int bytes;

		if (offset >= sge->length) {
			sge++;
			dma->cur_sge++;
			offset = 0;
			if (dma->cur_sge >= dma->num_sge)
				return -ENOSPC;
		}

		bytes = length;

		if (bytes > sge->length - offset)
			bytes = sge->length - offset;

		offset	+= bytes;
		resid	-= bytes;
		length	-= bytes;
	}

	dma->sge_offset = offset;
	dma->resid	= resid;

	return 0;
}

<<<<<<< HEAD
=======
/* (1) find the mr corresponding to lkey/rkey
 *     depending on lookup_type
 * (2) verify that the (qp) pd matches the mr pd
 * (3) verify that the mr can support the requested access
 * (4) verify that mr state is valid
 */
>>>>>>> b7ba80a49124 (Commit)
struct rxe_mr *lookup_mr(struct rxe_pd *pd, int access, u32 key,
			 enum rxe_mr_lookup_type type)
{
	struct rxe_mr *mr;
	struct rxe_dev *rxe = to_rdev(pd->ibpd.device);
	int index = key >> 8;

	mr = rxe_pool_get_index(&rxe->mr_pool, index);
	if (!mr)
		return NULL;

	if (unlikely((type == RXE_LOOKUP_LOCAL && mr->lkey != key) ||
		     (type == RXE_LOOKUP_REMOTE && mr->rkey != key) ||
<<<<<<< HEAD
		     mr_pd(mr) != pd || ((access & mr->access) != access) ||
=======
		     mr_pd(mr) != pd || (access && !(access & mr->access)) ||
>>>>>>> b7ba80a49124 (Commit)
		     mr->state != RXE_MR_STATE_VALID)) {
		rxe_put(mr);
		mr = NULL;
	}

	return mr;
}

int rxe_invalidate_mr(struct rxe_qp *qp, u32 key)
{
	struct rxe_dev *rxe = to_rdev(qp->ibqp.device);
	struct rxe_mr *mr;
	int ret;

	mr = rxe_pool_get_index(&rxe->mr_pool, key >> 8);
	if (!mr) {
<<<<<<< HEAD
		rxe_dbg_qp(qp, "No MR for key %#x\n", key);
=======
		pr_err("%s: No MR for key %#x\n", __func__, key);
>>>>>>> b7ba80a49124 (Commit)
		ret = -EINVAL;
		goto err;
	}

	if (mr->rkey ? (key != mr->rkey) : (key != mr->lkey)) {
<<<<<<< HEAD
		rxe_dbg_mr(mr, "wr key (%#x) doesn't match mr key (%#x)\n",
			key, (mr->rkey ? mr->rkey : mr->lkey));
=======
		pr_err("%s: wr key (%#x) doesn't match mr key (%#x)\n",
			__func__, key, (mr->rkey ? mr->rkey : mr->lkey));
>>>>>>> b7ba80a49124 (Commit)
		ret = -EINVAL;
		goto err_drop_ref;
	}

	if (atomic_read(&mr->num_mw) > 0) {
<<<<<<< HEAD
		rxe_dbg_mr(mr, "Attempt to invalidate an MR while bound to MWs\n");
=======
		pr_warn("%s: Attempt to invalidate an MR while bound to MWs\n",
			__func__);
>>>>>>> b7ba80a49124 (Commit)
		ret = -EINVAL;
		goto err_drop_ref;
	}

<<<<<<< HEAD
	if (unlikely(mr->ibmr.type != IB_MR_TYPE_MEM_REG)) {
		rxe_dbg_mr(mr, "Type (%d) is wrong\n", mr->ibmr.type);
=======
	if (unlikely(mr->type != IB_MR_TYPE_MEM_REG)) {
		pr_warn("%s: mr->type (%d) is wrong type\n", __func__, mr->type);
>>>>>>> b7ba80a49124 (Commit)
		ret = -EINVAL;
		goto err_drop_ref;
	}

	mr->state = RXE_MR_STATE_FREE;
	ret = 0;

err_drop_ref:
	rxe_put(mr);
err:
	return ret;
}

/* user can (re)register fast MR by executing a REG_MR WQE.
 * user is expected to hold a reference on the ib mr until the
 * WQE completes.
 * Once a fast MR is created this is the only way to change the
 * private keys. It is the responsibility of the user to maintain
 * the ib mr keys in sync with rxe mr keys.
 */
int rxe_reg_fast_mr(struct rxe_qp *qp, struct rxe_send_wqe *wqe)
{
	struct rxe_mr *mr = to_rmr(wqe->wr.wr.reg.mr);
	u32 key = wqe->wr.wr.reg.key;
	u32 access = wqe->wr.wr.reg.access;

	/* user can only register MR in free state */
	if (unlikely(mr->state != RXE_MR_STATE_FREE)) {
<<<<<<< HEAD
		rxe_dbg_mr(mr, "mr->lkey = 0x%x not free\n", mr->lkey);
=======
		pr_warn("%s: mr->lkey = 0x%x not free\n",
			__func__, mr->lkey);
>>>>>>> b7ba80a49124 (Commit)
		return -EINVAL;
	}

	/* user can only register mr with qp in same protection domain */
	if (unlikely(qp->ibqp.pd != mr->ibmr.pd)) {
<<<<<<< HEAD
		rxe_dbg_mr(mr, "qp->pd and mr->pd don't match\n");
=======
		pr_warn("%s: qp->pd and mr->pd don't match\n",
			__func__);
>>>>>>> b7ba80a49124 (Commit)
		return -EINVAL;
	}

	/* user is only allowed to change key portion of l/rkey */
	if (unlikely((mr->lkey & ~0xff) != (key & ~0xff))) {
<<<<<<< HEAD
		rxe_dbg_mr(mr, "key = 0x%x has wrong index mr->lkey = 0x%x\n",
			key, mr->lkey);
=======
		pr_warn("%s: key = 0x%x has wrong index mr->lkey = 0x%x\n",
			__func__, key, mr->lkey);
>>>>>>> b7ba80a49124 (Commit)
		return -EINVAL;
	}

	mr->access = access;
	mr->lkey = key;
	mr->rkey = (access & IB_ACCESS_REMOTE) ? key : 0;
<<<<<<< HEAD
	mr->ibmr.iova = wqe->wr.wr.reg.mr->iova;
=======
	mr->iova = wqe->wr.wr.reg.mr->iova;
>>>>>>> b7ba80a49124 (Commit)
	mr->state = RXE_MR_STATE_VALID;

	return 0;
}

int rxe_dereg_mr(struct ib_mr *ibmr, struct ib_udata *udata)
{
	struct rxe_mr *mr = to_rmr(ibmr);

	/* See IBA 10.6.7.2.6 */
	if (atomic_read(&mr->num_mw) > 0)
		return -EINVAL;

	rxe_cleanup(mr);
<<<<<<< HEAD
	kfree(mr);
=======

>>>>>>> b7ba80a49124 (Commit)
	return 0;
}

void rxe_mr_cleanup(struct rxe_pool_elem *elem)
{
	struct rxe_mr *mr = container_of(elem, typeof(*mr), elem);
<<<<<<< HEAD
=======
	int i;
>>>>>>> b7ba80a49124 (Commit)

	rxe_put(mr_pd(mr));
	ib_umem_release(mr->umem);

<<<<<<< HEAD
	if (mr->ibmr.type != IB_MR_TYPE_DMA)
		xa_destroy(&mr->page_list);
=======
	if (mr->map) {
		for (i = 0; i < mr->num_map; i++)
			kfree(mr->map[i]);

		kfree(mr->map);
	}
>>>>>>> b7ba80a49124 (Commit)
}
