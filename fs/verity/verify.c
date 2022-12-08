// SPDX-License-Identifier: GPL-2.0
/*
 * Data verification functions, i.e. hooks for ->readahead()
 *
 * Copyright 2019 Google LLC
 */

#include "fsverity_private.h"

#include <crypto/hash.h>
#include <linux/bio.h>
<<<<<<< HEAD

static struct workqueue_struct *fsverity_read_workqueue;

static inline int cmp_hashes(const struct fsverity_info *vi,
			     const u8 *want_hash, const u8 *real_hash,
			     u64 data_pos, int level)
=======
#include <linux/ratelimit.h>

static struct workqueue_struct *fsverity_read_workqueue;

/**
 * hash_at_level() - compute the location of the block's hash at the given level
 *
 * @params:	(in) the Merkle tree parameters
 * @dindex:	(in) the index of the data block being verified
 * @level:	(in) the level of hash we want (0 is leaf level)
 * @hindex:	(out) the index of the hash block containing the wanted hash
 * @hoffset:	(out) the byte offset to the wanted hash within the hash block
 */
static void hash_at_level(const struct merkle_tree_params *params,
			  pgoff_t dindex, unsigned int level, pgoff_t *hindex,
			  unsigned int *hoffset)
{
	pgoff_t position;

	/* Offset of the hash within the level's region, in hashes */
	position = dindex >> (level * params->log_arity);

	/* Index of the hash block in the tree overall */
	*hindex = params->level_start[level] + (position >> params->log_arity);

	/* Offset of the wanted hash (in bytes) within the hash block */
	*hoffset = (position & ((1 << params->log_arity) - 1)) <<
		   (params->log_blocksize - params->log_arity);
}

static inline int cmp_hashes(const struct fsverity_info *vi,
			     const u8 *want_hash, const u8 *real_hash,
			     pgoff_t index, int level)
>>>>>>> b7ba80a49124 (Commit)
{
	const unsigned int hsize = vi->tree_params.digest_size;

	if (memcmp(want_hash, real_hash, hsize) == 0)
		return 0;

	fsverity_err(vi->inode,
<<<<<<< HEAD
		     "FILE CORRUPTED! pos=%llu, level=%d, want_hash=%s:%*phN, real_hash=%s:%*phN",
		     data_pos, level,
=======
		     "FILE CORRUPTED! index=%lu, level=%d, want_hash=%s:%*phN, real_hash=%s:%*phN",
		     index, level,
>>>>>>> b7ba80a49124 (Commit)
		     vi->tree_params.hash_alg->name, hsize, want_hash,
		     vi->tree_params.hash_alg->name, hsize, real_hash);
	return -EBADMSG;
}

<<<<<<< HEAD
static bool data_is_zeroed(struct inode *inode, struct page *page,
			   unsigned int len, unsigned int offset)
{
	void *virt = kmap_local_page(page);

	if (memchr_inv(virt + offset, 0, len)) {
		kunmap_local(virt);
		fsverity_err(inode,
			     "FILE CORRUPTED!  Data past EOF is not zeroed");
		return false;
	}
	kunmap_local(virt);
	return true;
}

/*
 * Returns true if the hash block with index @hblock_idx in the tree, located in
 * @hpage, has already been verified.
 */
static bool is_hash_block_verified(struct fsverity_info *vi, struct page *hpage,
				   unsigned long hblock_idx)
{
	bool verified;
	unsigned int blocks_per_page;
	unsigned int i;

	/*
	 * When the Merkle tree block size and page size are the same, then the
	 * ->hash_block_verified bitmap isn't allocated, and we use PG_checked
	 * to directly indicate whether the page's block has been verified.
	 *
	 * Using PG_checked also guarantees that we re-verify hash pages that
	 * get evicted and re-instantiated from the backing storage, as new
	 * pages always start out with PG_checked cleared.
	 */
	if (!vi->hash_block_verified)
		return PageChecked(hpage);

	/*
	 * When the Merkle tree block size and page size differ, we use a bitmap
	 * to indicate whether each hash block has been verified.
	 *
	 * However, we still need to ensure that hash pages that get evicted and
	 * re-instantiated from the backing storage are re-verified.  To do
	 * this, we use PG_checked again, but now it doesn't really mean
	 * "checked".  Instead, now it just serves as an indicator for whether
	 * the hash page is newly instantiated or not.
	 *
	 * The first thread that sees PG_checked=0 must clear the corresponding
	 * bitmap bits, then set PG_checked=1.  This requires a spinlock.  To
	 * avoid having to take this spinlock in the common case of
	 * PG_checked=1, we start with an opportunistic lockless read.
	 */
	if (PageChecked(hpage)) {
		/*
		 * A read memory barrier is needed here to give ACQUIRE
		 * semantics to the above PageChecked() test.
		 */
		smp_rmb();
		return test_bit(hblock_idx, vi->hash_block_verified);
	}
	spin_lock(&vi->hash_page_init_lock);
	if (PageChecked(hpage)) {
		verified = test_bit(hblock_idx, vi->hash_block_verified);
	} else {
		blocks_per_page = vi->tree_params.blocks_per_page;
		hblock_idx = round_down(hblock_idx, blocks_per_page);
		for (i = 0; i < blocks_per_page; i++)
			clear_bit(hblock_idx + i, vi->hash_block_verified);
		/*
		 * A write memory barrier is needed here to give RELEASE
		 * semantics to the below SetPageChecked() operation.
		 */
		smp_wmb();
		SetPageChecked(hpage);
		verified = false;
	}
	spin_unlock(&vi->hash_page_init_lock);
	return verified;
}

/*
 * Verify a single data block against the file's Merkle tree.
 *
 * In principle, we need to verify the entire path to the root node.  However,
 * for efficiency the filesystem may cache the hash blocks.  Therefore we need
 * only ascend the tree until an already-verified hash block is seen, and then
 * verify the path to that block.
 *
 * Return: %true if the data block is valid, else %false.
 */
static bool
verify_data_block(struct inode *inode, struct fsverity_info *vi,
		  struct ahash_request *req, struct page *data_page,
		  u64 data_pos, unsigned int dblock_offset_in_page,
		  unsigned long max_ra_pages)
{
	const struct merkle_tree_params *params = &vi->tree_params;
	const unsigned int hsize = params->digest_size;
=======
/*
 * Verify a single data page against the file's Merkle tree.
 *
 * In principle, we need to verify the entire path to the root node.  However,
 * for efficiency the filesystem may cache the hash pages.  Therefore we need
 * only ascend the tree until an already-verified page is seen, as indicated by
 * the PageChecked bit being set; then verify the path to that page.
 *
 * This code currently only supports the case where the verity block size is
 * equal to PAGE_SIZE.  Doing otherwise would be possible but tricky, since we
 * wouldn't be able to use the PageChecked bit.
 *
 * Note that multiple processes may race to verify a hash page and mark it
 * Checked, but it doesn't matter; the result will be the same either way.
 *
 * Return: true if the page is valid, else false.
 */
static bool verify_page(struct inode *inode, const struct fsverity_info *vi,
			struct ahash_request *req, struct page *data_page,
			unsigned long level0_ra_pages)
{
	const struct merkle_tree_params *params = &vi->tree_params;
	const unsigned int hsize = params->digest_size;
	const pgoff_t index = data_page->index;
>>>>>>> b7ba80a49124 (Commit)
	int level;
	u8 _want_hash[FS_VERITY_MAX_DIGEST_SIZE];
	const u8 *want_hash;
	u8 real_hash[FS_VERITY_MAX_DIGEST_SIZE];
<<<<<<< HEAD
	/* The hash blocks that are traversed, indexed by level */
	struct {
		/* Page containing the hash block */
		struct page *page;
		/* Index of the hash block in the tree overall */
		unsigned long index;
		/* Byte offset of the hash block within @page */
		unsigned int offset_in_page;
		/* Byte offset of the wanted hash within @page */
		unsigned int hoffset;
	} hblocks[FS_VERITY_MAX_LEVELS];
	/*
	 * The index of the previous level's block within that level; also the
	 * index of that block's hash within the current level.
	 */
	u64 hidx = data_pos >> params->log_blocksize;
	int err;

	if (unlikely(data_pos >= inode->i_size)) {
		/*
		 * This can happen in the data page spanning EOF when the Merkle
		 * tree block size is less than the page size.  The Merkle tree
		 * doesn't cover data blocks fully past EOF.  But the entire
		 * page spanning EOF can be visible to userspace via a mmap, and
		 * any part past EOF should be all zeroes.  Therefore, we need
		 * to verify that any data blocks fully past EOF are all zeroes.
		 */
		return data_is_zeroed(inode, data_page, params->block_size,
				      dblock_offset_in_page);
	}

	/*
	 * Starting at the leaf level, ascend the tree saving hash blocks along
	 * the way until we find a hash block that has already been verified, or
	 * until we reach the root.
	 */
	for (level = 0; level < params->num_levels; level++) {
		unsigned long next_hidx;
		unsigned long hblock_idx;
		pgoff_t hpage_idx;
		unsigned int hblock_offset_in_page;
		unsigned int hoffset;
		struct page *hpage;

		/*
		 * The index of the block in the current level; also the index
		 * of that block's hash within the next level.
		 */
		next_hidx = hidx >> params->log_arity;

		/* Index of the hash block in the tree overall */
		hblock_idx = params->level_start[level] + next_hidx;

		/* Index of the hash page in the tree overall */
		hpage_idx = hblock_idx >> params->log_blocks_per_page;

		/* Byte offset of the hash block within the page */
		hblock_offset_in_page =
			(hblock_idx << params->log_blocksize) & ~PAGE_MASK;

		/* Byte offset of the hash within the page */
		hoffset = hblock_offset_in_page +
			  ((hidx << params->log_digestsize) &
			   (params->block_size - 1));

		hpage = inode->i_sb->s_vop->read_merkle_tree_page(inode,
				hpage_idx, level == 0 ? min(max_ra_pages,
					params->tree_pages - hpage_idx) : 0);
=======
	struct page *hpages[FS_VERITY_MAX_LEVELS];
	unsigned int hoffsets[FS_VERITY_MAX_LEVELS];
	int err;

	if (WARN_ON_ONCE(!PageLocked(data_page) || PageUptodate(data_page)))
		return false;

	pr_debug_ratelimited("Verifying data page %lu...\n", index);

	/*
	 * Starting at the leaf level, ascend the tree saving hash pages along
	 * the way until we find a verified hash page, indicated by PageChecked;
	 * or until we reach the root.
	 */
	for (level = 0; level < params->num_levels; level++) {
		pgoff_t hindex;
		unsigned int hoffset;
		struct page *hpage;

		hash_at_level(params, index, level, &hindex, &hoffset);

		pr_debug_ratelimited("Level %d: hindex=%lu, hoffset=%u\n",
				     level, hindex, hoffset);

		hpage = inode->i_sb->s_vop->read_merkle_tree_page(inode, hindex,
				level == 0 ? level0_ra_pages : 0);
>>>>>>> b7ba80a49124 (Commit)
		if (IS_ERR(hpage)) {
			err = PTR_ERR(hpage);
			fsverity_err(inode,
				     "Error %d reading Merkle tree page %lu",
<<<<<<< HEAD
				     err, hpage_idx);
			goto out;
		}
		if (is_hash_block_verified(vi, hpage, hblock_idx)) {
			memcpy_from_page(_want_hash, hpage, hoffset, hsize);
			want_hash = _want_hash;
			put_page(hpage);
			goto descend;
		}
		hblocks[level].page = hpage;
		hblocks[level].index = hblock_idx;
		hblocks[level].offset_in_page = hblock_offset_in_page;
		hblocks[level].hoffset = hoffset;
		hidx = next_hidx;
	}

	want_hash = vi->root_hash;
descend:
	/* Descend the tree verifying hash blocks. */
	for (; level > 0; level--) {
		struct page *hpage = hblocks[level - 1].page;
		unsigned long hblock_idx = hblocks[level - 1].index;
		unsigned int hblock_offset_in_page =
			hblocks[level - 1].offset_in_page;
		unsigned int hoffset = hblocks[level - 1].hoffset;

		err = fsverity_hash_block(params, inode, req, hpage,
					  hblock_offset_in_page, real_hash);
		if (err)
			goto out;
		err = cmp_hashes(vi, want_hash, real_hash, data_pos, level - 1);
		if (err)
			goto out;
		/*
		 * Mark the hash block as verified.  This must be atomic and
		 * idempotent, as the same hash block might be verified by
		 * multiple threads concurrently.
		 */
		if (vi->hash_block_verified)
			set_bit(hblock_idx, vi->hash_block_verified);
		else
			SetPageChecked(hpage);
		memcpy_from_page(_want_hash, hpage, hoffset, hsize);
		want_hash = _want_hash;
		put_page(hpage);
	}

	/* Finally, verify the data block. */
	err = fsverity_hash_block(params, inode, req, data_page,
				  dblock_offset_in_page, real_hash);
	if (err)
		goto out;
	err = cmp_hashes(vi, want_hash, real_hash, data_pos, -1);
out:
	for (; level > 0; level--)
		put_page(hblocks[level - 1].page);
=======
				     err, hindex);
			goto out;
		}

		if (PageChecked(hpage)) {
			memcpy_from_page(_want_hash, hpage, hoffset, hsize);
			want_hash = _want_hash;
			put_page(hpage);
			pr_debug_ratelimited("Hash page already checked, want %s:%*phN\n",
					     params->hash_alg->name,
					     hsize, want_hash);
			goto descend;
		}
		pr_debug_ratelimited("Hash page not yet checked\n");
		hpages[level] = hpage;
		hoffsets[level] = hoffset;
	}

	want_hash = vi->root_hash;
	pr_debug("Want root hash: %s:%*phN\n",
		 params->hash_alg->name, hsize, want_hash);
descend:
	/* Descend the tree verifying hash pages */
	for (; level > 0; level--) {
		struct page *hpage = hpages[level - 1];
		unsigned int hoffset = hoffsets[level - 1];

		err = fsverity_hash_page(params, inode, req, hpage, real_hash);
		if (err)
			goto out;
		err = cmp_hashes(vi, want_hash, real_hash, index, level - 1);
		if (err)
			goto out;
		SetPageChecked(hpage);
		memcpy_from_page(_want_hash, hpage, hoffset, hsize);
		want_hash = _want_hash;
		put_page(hpage);
		pr_debug("Verified hash page at level %d, now want %s:%*phN\n",
			 level - 1, params->hash_alg->name, hsize, want_hash);
	}

	/* Finally, verify the data page */
	err = fsverity_hash_page(params, inode, req, data_page, real_hash);
	if (err)
		goto out;
	err = cmp_hashes(vi, want_hash, real_hash, index, -1);
out:
	for (; level > 0; level--)
		put_page(hpages[level - 1]);
>>>>>>> b7ba80a49124 (Commit)

	return err == 0;
}

<<<<<<< HEAD
static bool
verify_data_blocks(struct inode *inode, struct fsverity_info *vi,
		   struct ahash_request *req, struct folio *data_folio,
		   size_t len, size_t offset, unsigned long max_ra_pages)
{
	const unsigned int block_size = vi->tree_params.block_size;
	u64 pos = (u64)data_folio->index << PAGE_SHIFT;

	if (WARN_ON_ONCE(len <= 0 || !IS_ALIGNED(len | offset, block_size)))
		return false;
	if (WARN_ON_ONCE(!folio_test_locked(data_folio) ||
			 folio_test_uptodate(data_folio)))
		return false;
	do {
		struct page *data_page =
			folio_page(data_folio, offset >> PAGE_SHIFT);

		if (!verify_data_block(inode, vi, req, data_page, pos + offset,
				       offset & ~PAGE_MASK, max_ra_pages))
			return false;
		offset += block_size;
		len -= block_size;
	} while (len);
	return true;
}

/**
 * fsverity_verify_blocks() - verify data in a folio
 * @folio: the folio containing the data to verify
 * @len: the length of the data to verify in the folio
 * @offset: the offset of the data to verify in the folio
 *
 * Verify data that has just been read from a verity file.  The data must be
 * located in a pagecache folio that is still locked and not yet uptodate.  The
 * length and offset of the data must be Merkle tree block size aligned.
 *
 * Return: %true if the data is valid, else %false.
 */
bool fsverity_verify_blocks(struct folio *folio, size_t len, size_t offset)
{
	struct inode *inode = folio->mapping->host;
	struct fsverity_info *vi = inode->i_verity_info;
=======
/**
 * fsverity_verify_page() - verify a data page
 * @page: the page to verity
 *
 * Verify a page that has just been read from a verity file.  The page must be a
 * pagecache page that is still locked and not yet uptodate.
 *
 * Return: true if the page is valid, else false.
 */
bool fsverity_verify_page(struct page *page)
{
	struct inode *inode = page->mapping->host;
	const struct fsverity_info *vi = inode->i_verity_info;
>>>>>>> b7ba80a49124 (Commit)
	struct ahash_request *req;
	bool valid;

	/* This allocation never fails, since it's mempool-backed. */
	req = fsverity_alloc_hash_request(vi->tree_params.hash_alg, GFP_NOFS);

<<<<<<< HEAD
	valid = verify_data_blocks(inode, vi, req, folio, len, offset, 0);
=======
	valid = verify_page(inode, vi, req, page, 0);
>>>>>>> b7ba80a49124 (Commit)

	fsverity_free_hash_request(vi->tree_params.hash_alg, req);

	return valid;
}
<<<<<<< HEAD
EXPORT_SYMBOL_GPL(fsverity_verify_blocks);
=======
EXPORT_SYMBOL_GPL(fsverity_verify_page);
>>>>>>> b7ba80a49124 (Commit)

#ifdef CONFIG_BLOCK
/**
 * fsverity_verify_bio() - verify a 'read' bio that has just completed
 * @bio: the bio to verify
 *
<<<<<<< HEAD
 * Verify the bio's data against the file's Merkle tree.  All bio data segments
 * must be aligned to the file's Merkle tree block size.  If any data fails
 * verification, then bio->bi_status is set to an error status.
=======
 * Verify a set of pages that have just been read from a verity file.  The pages
 * must be pagecache pages that are still locked and not yet uptodate.  Pages
 * that fail verification are set to the Error state.  Verification is skipped
 * for pages already in the Error state, e.g. due to fscrypt decryption failure.
>>>>>>> b7ba80a49124 (Commit)
 *
 * This is a helper function for use by the ->readahead() method of filesystems
 * that issue bios to read data directly into the page cache.  Filesystems that
 * populate the page cache without issuing bios (e.g. non block-based
 * filesystems) must instead call fsverity_verify_page() directly on each page.
 * All filesystems must also call fsverity_verify_page() on holes.
 */
void fsverity_verify_bio(struct bio *bio)
{
	struct inode *inode = bio_first_page_all(bio)->mapping->host;
<<<<<<< HEAD
	struct fsverity_info *vi = inode->i_verity_info;
	struct ahash_request *req;
	struct folio_iter fi;
	unsigned long max_ra_pages = 0;

	/* This allocation never fails, since it's mempool-backed. */
	req = fsverity_alloc_hash_request(vi->tree_params.hash_alg, GFP_NOFS);
=======
	const struct fsverity_info *vi = inode->i_verity_info;
	const struct merkle_tree_params *params = &vi->tree_params;
	struct ahash_request *req;
	struct bio_vec *bv;
	struct bvec_iter_all iter_all;
	unsigned long max_ra_pages = 0;

	/* This allocation never fails, since it's mempool-backed. */
	req = fsverity_alloc_hash_request(params->hash_alg, GFP_NOFS);
>>>>>>> b7ba80a49124 (Commit)

	if (bio->bi_opf & REQ_RAHEAD) {
		/*
		 * If this bio is for data readahead, then we also do readahead
		 * of the first (largest) level of the Merkle tree.  Namely,
		 * when a Merkle tree page is read, we also try to piggy-back on
		 * some additional pages -- up to 1/4 the number of data pages.
		 *
		 * This improves sequential read performance, as it greatly
		 * reduces the number of I/O requests made to the Merkle tree.
		 */
<<<<<<< HEAD
		max_ra_pages = bio->bi_iter.bi_size >> (PAGE_SHIFT + 2);
	}

	bio_for_each_folio_all(fi, bio) {
		if (!verify_data_blocks(inode, vi, req, fi.folio, fi.length,
					fi.offset, max_ra_pages)) {
			bio->bi_status = BLK_STS_IOERR;
			break;
		}
	}

	fsverity_free_hash_request(vi->tree_params.hash_alg, req);
=======
		bio_for_each_segment_all(bv, bio, iter_all)
			max_ra_pages++;
		max_ra_pages /= 4;
	}

	bio_for_each_segment_all(bv, bio, iter_all) {
		struct page *page = bv->bv_page;
		unsigned long level0_index = page->index >> params->log_arity;
		unsigned long level0_ra_pages =
			min(max_ra_pages, params->level0_blocks - level0_index);

		if (!PageError(page) &&
		    !verify_page(inode, vi, req, page, level0_ra_pages))
			SetPageError(page);
	}

	fsverity_free_hash_request(params->hash_alg, req);
>>>>>>> b7ba80a49124 (Commit)
}
EXPORT_SYMBOL_GPL(fsverity_verify_bio);
#endif /* CONFIG_BLOCK */

/**
 * fsverity_enqueue_verify_work() - enqueue work on the fs-verity workqueue
 * @work: the work to enqueue
 *
 * Enqueue verification work for asynchronous processing.
 */
void fsverity_enqueue_verify_work(struct work_struct *work)
{
	queue_work(fsverity_read_workqueue, work);
}
EXPORT_SYMBOL_GPL(fsverity_enqueue_verify_work);

int __init fsverity_init_workqueue(void)
{
	/*
<<<<<<< HEAD
	 * Use a high-priority workqueue to prioritize verification work, which
	 * blocks reads from completing, over regular application tasks.
	 *
	 * For performance reasons, don't use an unbound workqueue.  Using an
	 * unbound workqueue for crypto operations causes excessive scheduler
	 * latency on ARM64.
	 */
	fsverity_read_workqueue = alloc_workqueue("fsverity_read_queue",
						  WQ_HIGHPRI,
=======
	 * Use an unbound workqueue to allow bios to be verified in parallel
	 * even when they happen to complete on the same CPU.  This sacrifices
	 * locality, but it's worthwhile since hashing is CPU-intensive.
	 *
	 * Also use a high-priority workqueue to prioritize verification work,
	 * which blocks reads from completing, over regular application tasks.
	 */
	fsverity_read_workqueue = alloc_workqueue("fsverity_read_queue",
						  WQ_UNBOUND | WQ_HIGHPRI,
>>>>>>> b7ba80a49124 (Commit)
						  num_online_cpus());
	if (!fsverity_read_workqueue)
		return -ENOMEM;
	return 0;
}

void __init fsverity_exit_workqueue(void)
{
	destroy_workqueue(fsverity_read_workqueue);
	fsverity_read_workqueue = NULL;
}
