// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/fs/nfs/read.c
 *
 * Block I/O for NFS
 *
 * Partial copy of Linus' read cache modifications to fs/nfs/file.c
 * modified for async RPC by okir@monad.swb.de
 */

#include <linux/time.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/stat.h>
#include <linux/mm.h>
#include <linux/slab.h>
<<<<<<< HEAD
#include <linux/task_io_accounting_ops.h>
=======
>>>>>>> b7ba80a49124 (Commit)
#include <linux/pagemap.h>
#include <linux/sunrpc/clnt.h>
#include <linux/nfs_fs.h>
#include <linux/nfs_page.h>
#include <linux/module.h>

#include "nfs4_fs.h"
#include "internal.h"
#include "iostat.h"
#include "fscache.h"
#include "pnfs.h"
#include "nfstrace.h"

#define NFSDBG_FACILITY		NFSDBG_PAGECACHE

static const struct nfs_pgio_completion_ops nfs_async_read_completion_ops;
static const struct nfs_rw_ops nfs_rw_read_ops;

static struct kmem_cache *nfs_rdata_cachep;

static struct nfs_pgio_header *nfs_readhdr_alloc(void)
{
	struct nfs_pgio_header *p = kmem_cache_zalloc(nfs_rdata_cachep, GFP_KERNEL);

	if (p)
		p->rw_mode = FMODE_READ;
	return p;
}

static void nfs_readhdr_free(struct nfs_pgio_header *rhdr)
{
	kmem_cache_free(nfs_rdata_cachep, rhdr);
}

<<<<<<< HEAD
static int nfs_return_empty_folio(struct folio *folio)
{
	folio_zero_segment(folio, 0, folio_size(folio));
	folio_mark_uptodate(folio);
	folio_unlock(folio);
=======
static
int nfs_return_empty_page(struct page *page)
{
	zero_user(page, 0, PAGE_SIZE);
	SetPageUptodate(page);
	unlock_page(page);
>>>>>>> b7ba80a49124 (Commit)
	return 0;
}

void nfs_pageio_init_read(struct nfs_pageio_descriptor *pgio,
			      struct inode *inode, bool force_mds,
			      const struct nfs_pgio_completion_ops *compl_ops)
{
	struct nfs_server *server = NFS_SERVER(inode);
	const struct nfs_pageio_ops *pg_ops = &nfs_pgio_rw_ops;

#ifdef CONFIG_NFS_V4_1
	if (server->pnfs_curr_ld && !force_mds)
		pg_ops = server->pnfs_curr_ld->pg_read_ops;
#endif
	nfs_pageio_init(pgio, inode, pg_ops, compl_ops, &nfs_rw_read_ops,
			server->rsize, 0);
}
EXPORT_SYMBOL_GPL(nfs_pageio_init_read);

static void nfs_pageio_complete_read(struct nfs_pageio_descriptor *pgio)
{
	struct nfs_pgio_mirror *pgm;
	unsigned long npages;

	nfs_pageio_complete(pgio);

	/* It doesn't make sense to do mirrored reads! */
	WARN_ON_ONCE(pgio->pg_mirror_count != 1);

	pgm = &pgio->pg_mirrors[0];
	NFS_I(pgio->pg_inode)->read_io += pgm->pg_bytes_written;
	npages = (pgm->pg_bytes_written + PAGE_SIZE - 1) >> PAGE_SHIFT;
	nfs_add_stats(pgio->pg_inode, NFSIOS_READPAGES, npages);
}


void nfs_pageio_reset_read_mds(struct nfs_pageio_descriptor *pgio)
{
	struct nfs_pgio_mirror *mirror;

	if (pgio->pg_ops && pgio->pg_ops->pg_cleanup)
		pgio->pg_ops->pg_cleanup(pgio);

	pgio->pg_ops = &nfs_pgio_rw_ops;

	/* read path should never have more than one mirror */
	WARN_ON_ONCE(pgio->pg_mirror_count != 1);

	mirror = &pgio->pg_mirrors[0];
	mirror->pg_bsize = NFS_SERVER(pgio->pg_inode)->rsize;
}
EXPORT_SYMBOL_GPL(nfs_pageio_reset_read_mds);

static void nfs_readpage_release(struct nfs_page *req, int error)
{
	struct inode *inode = d_inode(nfs_req_openctx(req)->dentry);
<<<<<<< HEAD
	struct folio *folio = nfs_page_to_folio(req);
=======
	struct page *page = req->wb_page;
>>>>>>> b7ba80a49124 (Commit)

	dprintk("NFS: read done (%s/%llu %d@%lld)\n", inode->i_sb->s_id,
		(unsigned long long)NFS_FILEID(inode), req->wb_bytes,
		(long long)req_offset(req));

	if (nfs_error_is_fatal_on_server(error) && error != -ETIMEDOUT)
<<<<<<< HEAD
		folio_set_error(folio);
	if (nfs_page_group_sync_on_bit(req, PG_UNLOCKPAGE)) {
		if (folio_test_uptodate(folio))
			nfs_fscache_write_page(inode, &folio->page);
		folio_unlock(folio);
=======
		SetPageError(page);
	if (nfs_page_group_sync_on_bit(req, PG_UNLOCKPAGE)) {
		if (PageUptodate(page))
			nfs_fscache_write_page(inode, page);
		unlock_page(page);
>>>>>>> b7ba80a49124 (Commit)
	}
	nfs_release_request(req);
}

struct nfs_readdesc {
	struct nfs_pageio_descriptor pgio;
	struct nfs_open_context *ctx;
};

static void nfs_page_group_set_uptodate(struct nfs_page *req)
{
	if (nfs_page_group_sync_on_bit(req, PG_UPTODATE))
<<<<<<< HEAD
		folio_mark_uptodate(nfs_page_to_folio(req));
=======
		SetPageUptodate(req->wb_page);
>>>>>>> b7ba80a49124 (Commit)
}

static void nfs_read_completion(struct nfs_pgio_header *hdr)
{
	unsigned long bytes = 0;
	int error;

	if (test_bit(NFS_IOHDR_REDO, &hdr->flags))
		goto out;
	while (!list_empty(&hdr->pages)) {
		struct nfs_page *req = nfs_list_entry(hdr->pages.next);
<<<<<<< HEAD
		struct folio *folio = nfs_page_to_folio(req);
=======
		struct page *page = req->wb_page;
>>>>>>> b7ba80a49124 (Commit)
		unsigned long start = req->wb_pgbase;
		unsigned long end = req->wb_pgbase + req->wb_bytes;

		if (test_bit(NFS_IOHDR_EOF, &hdr->flags)) {
			/* note: regions of the page not covered by a
			 * request are zeroed in readpage_async_filler */
			if (bytes > hdr->good_bytes) {
				/* nothing in this request was good, so zero
				 * the full extent of the request */
<<<<<<< HEAD
				folio_zero_segment(folio, start, end);
=======
				zero_user_segment(page, start, end);
>>>>>>> b7ba80a49124 (Commit)

			} else if (hdr->good_bytes - bytes < req->wb_bytes) {
				/* part of this request has good bytes, but
				 * not all. zero the bad bytes */
				start += hdr->good_bytes - bytes;
				WARN_ON(start < req->wb_pgbase);
<<<<<<< HEAD
				folio_zero_segment(folio, start, end);
=======
				zero_user_segment(page, start, end);
>>>>>>> b7ba80a49124 (Commit)
			}
		}
		error = 0;
		bytes += req->wb_bytes;
		if (test_bit(NFS_IOHDR_ERROR, &hdr->flags)) {
			if (bytes <= hdr->good_bytes)
				nfs_page_group_set_uptodate(req);
			else {
				error = hdr->error;
				xchg(&nfs_req_openctx(req)->error, error);
			}
		} else
			nfs_page_group_set_uptodate(req);
		nfs_list_remove_request(req);
		nfs_readpage_release(req, error);
	}
out:
	hdr->release(hdr);
}

static void nfs_initiate_read(struct nfs_pgio_header *hdr,
			      struct rpc_message *msg,
			      const struct nfs_rpc_ops *rpc_ops,
			      struct rpc_task_setup *task_setup_data, int how)
{
	rpc_ops->read_setup(hdr, msg);
	trace_nfs_initiate_read(hdr);
}

static void
nfs_async_read_error(struct list_head *head, int error)
{
	struct nfs_page	*req;

	while (!list_empty(head)) {
		req = nfs_list_entry(head->next);
		nfs_list_remove_request(req);
		nfs_readpage_release(req, error);
	}
}

static const struct nfs_pgio_completion_ops nfs_async_read_completion_ops = {
	.error_cleanup = nfs_async_read_error,
	.completion = nfs_read_completion,
};

/*
 * This is the callback from RPC telling us whether a reply was
 * received or some error occurred (timeout or socket shutdown).
 */
static int nfs_readpage_done(struct rpc_task *task,
			     struct nfs_pgio_header *hdr,
			     struct inode *inode)
{
	int status = NFS_PROTO(inode)->read_done(task, hdr);
	if (status != 0)
		return status;

	nfs_add_stats(inode, NFSIOS_SERVERREADBYTES, hdr->res.count);
	trace_nfs_readpage_done(task, hdr);

	if (task->tk_status == -ESTALE) {
		nfs_set_inode_stale(inode);
		nfs_mark_for_revalidate(inode);
	}
	return 0;
}

static void nfs_readpage_retry(struct rpc_task *task,
			       struct nfs_pgio_header *hdr)
{
	struct nfs_pgio_args *argp = &hdr->args;
	struct nfs_pgio_res  *resp = &hdr->res;

	/* This is a short read! */
	nfs_inc_stats(hdr->inode, NFSIOS_SHORTREAD);
	trace_nfs_readpage_short(task, hdr);

	/* Has the server at least made some progress? */
	if (resp->count == 0) {
		nfs_set_pgio_error(hdr, -EIO, argp->offset);
		return;
	}

	/* For non rpc-based layout drivers, retry-through-MDS */
	if (!task->tk_ops) {
		hdr->pnfs_error = -EAGAIN;
		return;
	}

	/* Yes, so retry the read at the end of the hdr */
	hdr->mds_offset += resp->count;
	argp->offset += resp->count;
	argp->pgbase += resp->count;
	argp->count -= resp->count;
	resp->count = 0;
	resp->eof = 0;
	rpc_restart_call_prepare(task);
}

static void nfs_readpage_result(struct rpc_task *task,
				struct nfs_pgio_header *hdr)
{
	if (hdr->res.eof) {
		loff_t pos = hdr->args.offset + hdr->res.count;
		unsigned int new = pos - hdr->io_start;

		if (hdr->good_bytes > new) {
			hdr->good_bytes = new;
			set_bit(NFS_IOHDR_EOF, &hdr->flags);
			clear_bit(NFS_IOHDR_ERROR, &hdr->flags);
		}
	} else if (hdr->res.count < hdr->args.count)
		nfs_readpage_retry(task, hdr);
}

<<<<<<< HEAD
static int readpage_async_filler(struct nfs_readdesc *desc, struct folio *folio)
{
	struct inode *inode = folio_file_mapping(folio)->host;
	struct nfs_server *server = NFS_SERVER(inode);
	size_t fsize = folio_size(folio);
	unsigned int rsize = server->rsize;
=======
static int
readpage_async_filler(struct nfs_readdesc *desc, struct page *page)
{
	struct inode *inode = page_file_mapping(page)->host;
	unsigned int rsize = NFS_SERVER(inode)->rsize;
>>>>>>> b7ba80a49124 (Commit)
	struct nfs_page *new;
	unsigned int len, aligned_len;
	int error;

<<<<<<< HEAD
	len = nfs_folio_length(folio);
	if (len == 0)
		return nfs_return_empty_folio(folio);

	aligned_len = min_t(unsigned int, ALIGN(len, rsize), fsize);

	if (!IS_SYNC(inode)) {
		error = nfs_fscache_read_page(inode, &folio->page);
=======
	len = nfs_page_length(page);
	if (len == 0)
		return nfs_return_empty_page(page);

	aligned_len = min_t(unsigned int, ALIGN(len, rsize), PAGE_SIZE);

	if (!IS_SYNC(page->mapping->host)) {
		error = nfs_fscache_read_page(page->mapping->host, page);
>>>>>>> b7ba80a49124 (Commit)
		if (error == 0)
			goto out_unlock;
	}

<<<<<<< HEAD
	new = nfs_page_create_from_folio(desc->ctx, folio, 0, aligned_len);
	if (IS_ERR(new))
		goto out_error;

	if (len < fsize)
		folio_zero_segment(folio, len, fsize);
=======
	new = nfs_create_request(desc->ctx, page, 0, aligned_len);
	if (IS_ERR(new))
		goto out_error;

	if (len < PAGE_SIZE)
		zero_user_segment(page, len, PAGE_SIZE);
>>>>>>> b7ba80a49124 (Commit)
	if (!nfs_pageio_add_request(&desc->pgio, new)) {
		nfs_list_remove_request(new);
		error = desc->pgio.pg_error;
		nfs_readpage_release(new, error);
		goto out;
	}
	return 0;
out_error:
	error = PTR_ERR(new);
out_unlock:
<<<<<<< HEAD
	folio_unlock(folio);
=======
	unlock_page(page);
>>>>>>> b7ba80a49124 (Commit)
out:
	return error;
}

/*
 * Read a page over NFS.
 * We read the page synchronously in the following case:
 *  -	The error flag is set for this page. This happens only when a
 *	previous async read operation failed.
 */
int nfs_read_folio(struct file *file, struct folio *folio)
{
<<<<<<< HEAD
	struct nfs_readdesc desc;
	struct inode *inode = file_inode(file);
	int ret;

	trace_nfs_aop_readpage(inode, folio);
	nfs_inc_stats(inode, NFSIOS_VFSREADPAGE);
	task_io_account_read(folio_size(folio));
=======
	struct page *page = &folio->page;
	struct nfs_readdesc desc;
	struct inode *inode = page_file_mapping(page)->host;
	int ret;

	trace_nfs_aop_readpage(inode, page);
	nfs_inc_stats(inode, NFSIOS_VFSREADPAGE);
>>>>>>> b7ba80a49124 (Commit)

	/*
	 * Try to flush any pending writes to the file..
	 *
<<<<<<< HEAD
	 * NOTE! Because we own the folio lock, there cannot
	 * be any new pending writes generated at this point
	 * for this folio (other folios can be written to).
	 */
	ret = nfs_wb_folio(inode, folio);
	if (ret)
		goto out_unlock;
	if (folio_test_uptodate(folio))
=======
	 * NOTE! Because we own the page lock, there cannot
	 * be any new pending writes generated at this point
	 * for this page (other pages can be written to).
	 */
	ret = nfs_wb_page(inode, page);
	if (ret)
		goto out_unlock;
	if (PageUptodate(page))
>>>>>>> b7ba80a49124 (Commit)
		goto out_unlock;

	ret = -ESTALE;
	if (NFS_STALE(inode))
		goto out_unlock;

<<<<<<< HEAD
	desc.ctx = get_nfs_open_context(nfs_file_open_context(file));
=======
	if (file == NULL) {
		ret = -EBADF;
		desc.ctx = nfs_find_open_context(inode, NULL, FMODE_READ);
		if (desc.ctx == NULL)
			goto out_unlock;
	} else
		desc.ctx = get_nfs_open_context(nfs_file_open_context(file));
>>>>>>> b7ba80a49124 (Commit)

	xchg(&desc.ctx->error, 0);
	nfs_pageio_init_read(&desc.pgio, inode, false,
			     &nfs_async_read_completion_ops);

<<<<<<< HEAD
	ret = readpage_async_filler(&desc, folio);
=======
	ret = readpage_async_filler(&desc, page);
>>>>>>> b7ba80a49124 (Commit)
	if (ret)
		goto out;

	nfs_pageio_complete_read(&desc.pgio);
	ret = desc.pgio.pg_error < 0 ? desc.pgio.pg_error : 0;
	if (!ret) {
<<<<<<< HEAD
		ret = folio_wait_locked_killable(folio);
		if (!folio_test_uptodate(folio) && !ret)
=======
		ret = wait_on_page_locked_killable(page);
		if (!PageUptodate(page) && !ret)
>>>>>>> b7ba80a49124 (Commit)
			ret = xchg(&desc.ctx->error, 0);
	}
out:
	put_nfs_open_context(desc.ctx);
<<<<<<< HEAD
	trace_nfs_aop_readpage_done(inode, folio, ret);
	return ret;
out_unlock:
	folio_unlock(folio);
	trace_nfs_aop_readpage_done(inode, folio, ret);
=======
	trace_nfs_aop_readpage_done(inode, page, ret);
	return ret;
out_unlock:
	unlock_page(page);
	trace_nfs_aop_readpage_done(inode, page, ret);
>>>>>>> b7ba80a49124 (Commit)
	return ret;
}

void nfs_readahead(struct readahead_control *ractl)
{
	unsigned int nr_pages = readahead_count(ractl);
	struct file *file = ractl->file;
	struct nfs_readdesc desc;
	struct inode *inode = ractl->mapping->host;
<<<<<<< HEAD
	struct folio *folio;
=======
	struct page *page;
>>>>>>> b7ba80a49124 (Commit)
	int ret;

	trace_nfs_aop_readahead(inode, readahead_pos(ractl), nr_pages);
	nfs_inc_stats(inode, NFSIOS_VFSREADPAGES);
<<<<<<< HEAD
	task_io_account_read(readahead_length(ractl));
=======
>>>>>>> b7ba80a49124 (Commit)

	ret = -ESTALE;
	if (NFS_STALE(inode))
		goto out;

	if (file == NULL) {
		ret = -EBADF;
		desc.ctx = nfs_find_open_context(inode, NULL, FMODE_READ);
		if (desc.ctx == NULL)
			goto out;
	} else
		desc.ctx = get_nfs_open_context(nfs_file_open_context(file));

	nfs_pageio_init_read(&desc.pgio, inode, false,
			     &nfs_async_read_completion_ops);

<<<<<<< HEAD
	while ((folio = readahead_folio(ractl)) != NULL) {
		ret = readpage_async_filler(&desc, folio);
=======
	while ((page = readahead_page(ractl)) != NULL) {
		ret = readpage_async_filler(&desc, page);
		put_page(page);
>>>>>>> b7ba80a49124 (Commit)
		if (ret)
			break;
	}

	nfs_pageio_complete_read(&desc.pgio);

	put_nfs_open_context(desc.ctx);
out:
	trace_nfs_aop_readahead_done(inode, nr_pages, ret);
}

int __init nfs_init_readpagecache(void)
{
	nfs_rdata_cachep = kmem_cache_create("nfs_read_data",
					     sizeof(struct nfs_pgio_header),
					     0, SLAB_HWCACHE_ALIGN,
					     NULL);
	if (nfs_rdata_cachep == NULL)
		return -ENOMEM;

	return 0;
}

void nfs_destroy_readpagecache(void)
{
	kmem_cache_destroy(nfs_rdata_cachep);
}

static const struct nfs_rw_ops nfs_rw_read_ops = {
	.rw_alloc_header	= nfs_readhdr_alloc,
	.rw_free_header		= nfs_readhdr_free,
	.rw_done		= nfs_readpage_done,
	.rw_result		= nfs_readpage_result,
	.rw_initiate		= nfs_initiate_read,
};
