// SPDX-License-Identifier: GPL-2.0
/*
 * Simple file system for zoned block devices exposing zones as files.
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 */
#include <linux/module.h>
#include <linux/pagemap.h>
#include <linux/magic.h>
#include <linux/iomap.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/blkdev.h>
#include <linux/statfs.h>
#include <linux/writeback.h>
#include <linux/quotaops.h>
#include <linux/seq_file.h>
#include <linux/parser.h>
#include <linux/uio.h>
#include <linux/mman.h>
#include <linux/sched/mm.h>
#include <linux/crc32.h>
#include <linux/task_io_accounting_ops.h>

#include "zonefs.h"

#define CREATE_TRACE_POINTS
#include "trace.h"

/*
<<<<<<< HEAD
 * Get the name of a zone group directory.
 */
static const char *zonefs_zgroup_name(enum zonefs_ztype ztype)
{
	switch (ztype) {
	case ZONEFS_ZTYPE_CNV:
		return "cnv";
	case ZONEFS_ZTYPE_SEQ:
		return "seq";
	default:
		WARN_ON_ONCE(1);
		return "???";
	}
}

/*
 * Manage the active zone count.
 */
static void zonefs_account_active(struct super_block *sb,
				  struct zonefs_zone *z)
{
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);

	if (zonefs_zone_is_cnv(z))
		return;

	/*
	 * For zones that transitioned to the offline or readonly condition,
	 * we only need to clear the active state.
	 */
	if (z->z_flags & (ZONEFS_ZONE_OFFLINE | ZONEFS_ZONE_READONLY))
		goto out;

	/*
	 * If the zone is active, that is, if it is explicitly open or
	 * partially written, check if it was already accounted as active.
	 */
	if ((z->z_flags & ZONEFS_ZONE_OPEN) ||
	    (z->z_wpoffset > 0 && z->z_wpoffset < z->z_capacity)) {
		if (!(z->z_flags & ZONEFS_ZONE_ACTIVE)) {
			z->z_flags |= ZONEFS_ZONE_ACTIVE;
=======
 * Manage the active zone count. Called with zi->i_truncate_mutex held.
 */
static void zonefs_account_active(struct inode *inode)
{
	struct zonefs_sb_info *sbi = ZONEFS_SB(inode->i_sb);
	struct zonefs_inode_info *zi = ZONEFS_I(inode);

	lockdep_assert_held(&zi->i_truncate_mutex);

	if (zi->i_ztype != ZONEFS_ZTYPE_SEQ)
		return;

	/*
	 * If the zone is active, that is, if it is explicitly open or
	 * partially written, check if it was already accounted as active.
	 */
	if ((zi->i_flags & ZONEFS_ZONE_OPEN) ||
	    (zi->i_wpoffset > 0 && zi->i_wpoffset < zi->i_max_size)) {
		if (!(zi->i_flags & ZONEFS_ZONE_ACTIVE)) {
			zi->i_flags |= ZONEFS_ZONE_ACTIVE;
>>>>>>> b7ba80a49124 (Commit)
			atomic_inc(&sbi->s_active_seq_files);
		}
		return;
	}

<<<<<<< HEAD
out:
	/* The zone is not active. If it was, update the active count */
	if (z->z_flags & ZONEFS_ZONE_ACTIVE) {
		z->z_flags &= ~ZONEFS_ZONE_ACTIVE;
=======
	/* The zone is not active. If it was, update the active count */
	if (zi->i_flags & ZONEFS_ZONE_ACTIVE) {
		zi->i_flags &= ~ZONEFS_ZONE_ACTIVE;
>>>>>>> b7ba80a49124 (Commit)
		atomic_dec(&sbi->s_active_seq_files);
	}
}

<<<<<<< HEAD
/*
 * Manage the active zone count. Called with zi->i_truncate_mutex held.
 */
void zonefs_inode_account_active(struct inode *inode)
{
	lockdep_assert_held(&ZONEFS_I(inode)->i_truncate_mutex);

	return zonefs_account_active(inode->i_sb, zonefs_inode_zone(inode));
}

/*
 * Execute a zone management operation.
 */
static int zonefs_zone_mgmt(struct super_block *sb,
			    struct zonefs_zone *z, enum req_op op)
{
	int ret;

=======
static inline int zonefs_zone_mgmt(struct inode *inode, enum req_op op)
{
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	int ret;

	lockdep_assert_held(&zi->i_truncate_mutex);

>>>>>>> b7ba80a49124 (Commit)
	/*
	 * With ZNS drives, closing an explicitly open zone that has not been
	 * written will change the zone state to "closed", that is, the zone
	 * will remain active. Since this can then cause failure of explicit
	 * open operation on other zones if the drive active zone resources
	 * are exceeded, make sure that the zone does not remain active by
	 * resetting it.
	 */
<<<<<<< HEAD
	if (op == REQ_OP_ZONE_CLOSE && !z->z_wpoffset)
		op = REQ_OP_ZONE_RESET;

	trace_zonefs_zone_mgmt(sb, z, op);
	ret = blkdev_zone_mgmt(sb->s_bdev, op, z->z_sector,
			       z->z_size >> SECTOR_SHIFT, GFP_NOFS);
	if (ret) {
		zonefs_err(sb,
			   "Zone management operation %s at %llu failed %d\n",
			   blk_op_str(op), z->z_sector, ret);
=======
	if (op == REQ_OP_ZONE_CLOSE && !zi->i_wpoffset)
		op = REQ_OP_ZONE_RESET;

	trace_zonefs_zone_mgmt(inode, op);
	ret = blkdev_zone_mgmt(inode->i_sb->s_bdev, op, zi->i_zsector,
			       zi->i_zone_size >> SECTOR_SHIFT, GFP_NOFS);
	if (ret) {
		zonefs_err(inode->i_sb,
			   "Zone management operation %s at %llu failed %d\n",
			   blk_op_str(op), zi->i_zsector, ret);
>>>>>>> b7ba80a49124 (Commit)
		return ret;
	}

	return 0;
}

<<<<<<< HEAD
int zonefs_inode_zone_mgmt(struct inode *inode, enum req_op op)
{
	lockdep_assert_held(&ZONEFS_I(inode)->i_truncate_mutex);

	return zonefs_zone_mgmt(inode->i_sb, zonefs_inode_zone(inode), op);
}

void zonefs_i_size_write(struct inode *inode, loff_t isize)
{
	struct zonefs_zone *z = zonefs_inode_zone(inode);

	i_size_write(inode, isize);

=======
static inline void zonefs_i_size_write(struct inode *inode, loff_t isize)
{
	struct zonefs_inode_info *zi = ZONEFS_I(inode);

	i_size_write(inode, isize);
>>>>>>> b7ba80a49124 (Commit)
	/*
	 * A full zone is no longer open/active and does not need
	 * explicit closing.
	 */
<<<<<<< HEAD
	if (isize >= z->z_capacity) {
		struct zonefs_sb_info *sbi = ZONEFS_SB(inode->i_sb);

		if (z->z_flags & ZONEFS_ZONE_ACTIVE)
			atomic_dec(&sbi->s_active_seq_files);
		z->z_flags &= ~(ZONEFS_ZONE_OPEN | ZONEFS_ZONE_ACTIVE);
	}
}

void zonefs_update_stats(struct inode *inode, loff_t new_isize)
=======
	if (isize >= zi->i_max_size) {
		struct zonefs_sb_info *sbi = ZONEFS_SB(inode->i_sb);

		if (zi->i_flags & ZONEFS_ZONE_ACTIVE)
			atomic_dec(&sbi->s_active_seq_files);
		zi->i_flags &= ~(ZONEFS_ZONE_OPEN | ZONEFS_ZONE_ACTIVE);
	}
}

static int zonefs_read_iomap_begin(struct inode *inode, loff_t offset,
				   loff_t length, unsigned int flags,
				   struct iomap *iomap, struct iomap *srcmap)
{
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	struct super_block *sb = inode->i_sb;
	loff_t isize;

	/*
	 * All blocks are always mapped below EOF. If reading past EOF,
	 * act as if there is a hole up to the file maximum size.
	 */
	mutex_lock(&zi->i_truncate_mutex);
	iomap->bdev = inode->i_sb->s_bdev;
	iomap->offset = ALIGN_DOWN(offset, sb->s_blocksize);
	isize = i_size_read(inode);
	if (iomap->offset >= isize) {
		iomap->type = IOMAP_HOLE;
		iomap->addr = IOMAP_NULL_ADDR;
		iomap->length = length;
	} else {
		iomap->type = IOMAP_MAPPED;
		iomap->addr = (zi->i_zsector << SECTOR_SHIFT) + iomap->offset;
		iomap->length = isize - iomap->offset;
	}
	mutex_unlock(&zi->i_truncate_mutex);

	trace_zonefs_iomap_begin(inode, iomap);

	return 0;
}

static const struct iomap_ops zonefs_read_iomap_ops = {
	.iomap_begin	= zonefs_read_iomap_begin,
};

static int zonefs_write_iomap_begin(struct inode *inode, loff_t offset,
				    loff_t length, unsigned int flags,
				    struct iomap *iomap, struct iomap *srcmap)
{
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	struct super_block *sb = inode->i_sb;
	loff_t isize;

	/* All write I/Os should always be within the file maximum size */
	if (WARN_ON_ONCE(offset + length > zi->i_max_size))
		return -EIO;

	/*
	 * Sequential zones can only accept direct writes. This is already
	 * checked when writes are issued, so warn if we see a page writeback
	 * operation.
	 */
	if (WARN_ON_ONCE(zi->i_ztype == ZONEFS_ZTYPE_SEQ &&
			 !(flags & IOMAP_DIRECT)))
		return -EIO;

	/*
	 * For conventional zones, all blocks are always mapped. For sequential
	 * zones, all blocks after always mapped below the inode size (zone
	 * write pointer) and unwriten beyond.
	 */
	mutex_lock(&zi->i_truncate_mutex);
	iomap->bdev = inode->i_sb->s_bdev;
	iomap->offset = ALIGN_DOWN(offset, sb->s_blocksize);
	iomap->addr = (zi->i_zsector << SECTOR_SHIFT) + iomap->offset;
	isize = i_size_read(inode);
	if (iomap->offset >= isize) {
		iomap->type = IOMAP_UNWRITTEN;
		iomap->length = zi->i_max_size - iomap->offset;
	} else {
		iomap->type = IOMAP_MAPPED;
		iomap->length = isize - iomap->offset;
	}
	mutex_unlock(&zi->i_truncate_mutex);

	trace_zonefs_iomap_begin(inode, iomap);

	return 0;
}

static const struct iomap_ops zonefs_write_iomap_ops = {
	.iomap_begin	= zonefs_write_iomap_begin,
};

static int zonefs_read_folio(struct file *unused, struct folio *folio)
{
	return iomap_read_folio(folio, &zonefs_read_iomap_ops);
}

static void zonefs_readahead(struct readahead_control *rac)
{
	iomap_readahead(rac, &zonefs_read_iomap_ops);
}

/*
 * Map blocks for page writeback. This is used only on conventional zone files,
 * which implies that the page range can only be within the fixed inode size.
 */
static int zonefs_write_map_blocks(struct iomap_writepage_ctx *wpc,
				   struct inode *inode, loff_t offset)
{
	struct zonefs_inode_info *zi = ZONEFS_I(inode);

	if (WARN_ON_ONCE(zi->i_ztype != ZONEFS_ZTYPE_CNV))
		return -EIO;
	if (WARN_ON_ONCE(offset >= i_size_read(inode)))
		return -EIO;

	/* If the mapping is already OK, nothing needs to be done */
	if (offset >= wpc->iomap.offset &&
	    offset < wpc->iomap.offset + wpc->iomap.length)
		return 0;

	return zonefs_write_iomap_begin(inode, offset, zi->i_max_size - offset,
					IOMAP_WRITE, &wpc->iomap, NULL);
}

static const struct iomap_writeback_ops zonefs_writeback_ops = {
	.map_blocks		= zonefs_write_map_blocks,
};

static int zonefs_writepages(struct address_space *mapping,
			     struct writeback_control *wbc)
{
	struct iomap_writepage_ctx wpc = { };

	return iomap_writepages(mapping, wbc, &wpc, &zonefs_writeback_ops);
}

static int zonefs_swap_activate(struct swap_info_struct *sis,
				struct file *swap_file, sector_t *span)
{
	struct inode *inode = file_inode(swap_file);
	struct zonefs_inode_info *zi = ZONEFS_I(inode);

	if (zi->i_ztype != ZONEFS_ZTYPE_CNV) {
		zonefs_err(inode->i_sb,
			   "swap file: not a conventional zone file\n");
		return -EINVAL;
	}

	return iomap_swapfile_activate(sis, swap_file, span,
				       &zonefs_read_iomap_ops);
}

static const struct address_space_operations zonefs_file_aops = {
	.read_folio		= zonefs_read_folio,
	.readahead		= zonefs_readahead,
	.writepages		= zonefs_writepages,
	.dirty_folio		= filemap_dirty_folio,
	.release_folio		= iomap_release_folio,
	.invalidate_folio	= iomap_invalidate_folio,
	.migrate_folio		= filemap_migrate_folio,
	.is_partially_uptodate	= iomap_is_partially_uptodate,
	.error_remove_page	= generic_error_remove_page,
	.direct_IO		= noop_direct_IO,
	.swap_activate		= zonefs_swap_activate,
};

static void zonefs_update_stats(struct inode *inode, loff_t new_isize)
>>>>>>> b7ba80a49124 (Commit)
{
	struct super_block *sb = inode->i_sb;
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	loff_t old_isize = i_size_read(inode);
	loff_t nr_blocks;

	if (new_isize == old_isize)
		return;

	spin_lock(&sbi->s_lock);

	/*
	 * This may be called for an update after an IO error.
	 * So beware of the values seen.
	 */
	if (new_isize < old_isize) {
		nr_blocks = (old_isize - new_isize) >> sb->s_blocksize_bits;
		if (sbi->s_used_blocks > nr_blocks)
			sbi->s_used_blocks -= nr_blocks;
		else
			sbi->s_used_blocks = 0;
	} else {
		sbi->s_used_blocks +=
			(new_isize - old_isize) >> sb->s_blocksize_bits;
		if (sbi->s_used_blocks > sbi->s_blocks)
			sbi->s_used_blocks = sbi->s_blocks;
	}

	spin_unlock(&sbi->s_lock);
}

/*
<<<<<<< HEAD
 * Check a zone condition. Return the amount of written (and still readable)
 * data in the zone.
 */
static loff_t zonefs_check_zone_condition(struct super_block *sb,
					  struct zonefs_zone *z,
					  struct blk_zone *zone)
{
	switch (zone->cond) {
	case BLK_ZONE_COND_OFFLINE:
		zonefs_warn(sb, "Zone %llu: offline zone\n",
			    z->z_sector);
		z->z_flags |= ZONEFS_ZONE_OFFLINE;
		return 0;
	case BLK_ZONE_COND_READONLY:
		/*
		 * The write pointer of read-only zones is invalid, so we cannot
		 * determine the zone wpoffset (inode size). We thus keep the
		 * zone wpoffset as is, which leads to an empty file
		 * (wpoffset == 0) on mount. For a runtime error, this keeps
		 * the inode size as it was when last updated so that the user
		 * can recover data.
		 */
		zonefs_warn(sb, "Zone %llu: read-only zone\n",
			    z->z_sector);
		z->z_flags |= ZONEFS_ZONE_READONLY;
		if (zonefs_zone_is_cnv(z))
			return z->z_capacity;
		return z->z_wpoffset;
	case BLK_ZONE_COND_FULL:
		/* The write pointer of full zones is invalid. */
		return z->z_capacity;
	default:
		if (zonefs_zone_is_cnv(z))
			return z->z_capacity;
=======
 * Check a zone condition and adjust its file inode access permissions for
 * offline and readonly zones. Return the inode size corresponding to the
 * amount of readable data in the zone.
 */
static loff_t zonefs_check_zone_condition(struct inode *inode,
					  struct blk_zone *zone, bool warn,
					  bool mount)
{
	struct zonefs_inode_info *zi = ZONEFS_I(inode);

	switch (zone->cond) {
	case BLK_ZONE_COND_OFFLINE:
		/*
		 * Dead zone: make the inode immutable, disable all accesses
		 * and set the file size to 0 (zone wp set to zone start).
		 */
		if (warn)
			zonefs_warn(inode->i_sb, "inode %lu: offline zone\n",
				    inode->i_ino);
		inode->i_flags |= S_IMMUTABLE;
		inode->i_mode &= ~0777;
		zone->wp = zone->start;
		return 0;
	case BLK_ZONE_COND_READONLY:
		/*
		 * The write pointer of read-only zones is invalid. If such a
		 * zone is found during mount, the file size cannot be retrieved
		 * so we treat the zone as offline (mount == true case).
		 * Otherwise, keep the file size as it was when last updated
		 * so that the user can recover data. In both cases, writes are
		 * always disabled for the zone.
		 */
		if (warn)
			zonefs_warn(inode->i_sb, "inode %lu: read-only zone\n",
				    inode->i_ino);
		inode->i_flags |= S_IMMUTABLE;
		if (mount) {
			zone->cond = BLK_ZONE_COND_OFFLINE;
			inode->i_mode &= ~0777;
			zone->wp = zone->start;
			return 0;
		}
		inode->i_mode &= ~0222;
		return i_size_read(inode);
	case BLK_ZONE_COND_FULL:
		/* The write pointer of full zones is invalid. */
		return zi->i_max_size;
	default:
		if (zi->i_ztype == ZONEFS_ZTYPE_CNV)
			return zi->i_max_size;
>>>>>>> b7ba80a49124 (Commit)
		return (zone->wp - zone->start) << SECTOR_SHIFT;
	}
}

<<<<<<< HEAD
/*
 * Check a zone condition and adjust its inode access permissions for
 * offline and readonly zones.
 */
static void zonefs_inode_update_mode(struct inode *inode)
{
	struct zonefs_zone *z = zonefs_inode_zone(inode);

	if (z->z_flags & ZONEFS_ZONE_OFFLINE) {
		/* Offline zones cannot be read nor written */
		inode->i_flags |= S_IMMUTABLE;
		inode->i_mode &= ~0777;
	} else if (z->z_flags & ZONEFS_ZONE_READONLY) {
		/* Readonly zones cannot be written */
		inode->i_flags |= S_IMMUTABLE;
		if (z->z_flags & ZONEFS_ZONE_INIT_MODE)
			inode->i_mode &= ~0777;
		else
			inode->i_mode &= ~0222;
	}

	z->z_flags &= ~ZONEFS_ZONE_INIT_MODE;
	z->z_mode = inode->i_mode;
}

=======
>>>>>>> b7ba80a49124 (Commit)
struct zonefs_ioerr_data {
	struct inode	*inode;
	bool		write;
};

static int zonefs_io_error_cb(struct blk_zone *zone, unsigned int idx,
			      void *data)
{
	struct zonefs_ioerr_data *err = data;
	struct inode *inode = err->inode;
<<<<<<< HEAD
	struct zonefs_zone *z = zonefs_inode_zone(inode);
=======
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
>>>>>>> b7ba80a49124 (Commit)
	struct super_block *sb = inode->i_sb;
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	loff_t isize, data_size;

	/*
	 * Check the zone condition: if the zone is not "bad" (offline or
	 * read-only), read errors are simply signaled to the IO issuer as long
	 * as there is no inconsistency between the inode size and the amount of
	 * data writen in the zone (data_size).
	 */
<<<<<<< HEAD
	data_size = zonefs_check_zone_condition(sb, z, zone);
	isize = i_size_read(inode);
	if (!(z->z_flags & (ZONEFS_ZONE_READONLY | ZONEFS_ZONE_OFFLINE)) &&
=======
	data_size = zonefs_check_zone_condition(inode, zone, true, false);
	isize = i_size_read(inode);
	if (zone->cond != BLK_ZONE_COND_OFFLINE &&
	    zone->cond != BLK_ZONE_COND_READONLY &&
>>>>>>> b7ba80a49124 (Commit)
	    !err->write && isize == data_size)
		return 0;

	/*
	 * At this point, we detected either a bad zone or an inconsistency
	 * between the inode size and the amount of data written in the zone.
	 * For the latter case, the cause may be a write IO error or an external
	 * action on the device. Two error patterns exist:
	 * 1) The inode size is lower than the amount of data in the zone:
	 *    a write operation partially failed and data was writen at the end
	 *    of the file. This can happen in the case of a large direct IO
	 *    needing several BIOs and/or write requests to be processed.
	 * 2) The inode size is larger than the amount of data in the zone:
	 *    this can happen with a deferred write error with the use of the
	 *    device side write cache after getting successful write IO
	 *    completions. Other possibilities are (a) an external corruption,
	 *    e.g. an application reset the zone directly, or (b) the device
	 *    has a serious problem (e.g. firmware bug).
	 *
	 * In all cases, warn about inode size inconsistency and handle the
	 * IO error according to the zone condition and to the mount options.
	 */
<<<<<<< HEAD
	if (zonefs_zone_is_seq(z) && isize != data_size)
		zonefs_warn(sb,
			    "inode %lu: invalid size %lld (should be %lld)\n",
=======
	if (zi->i_ztype == ZONEFS_ZTYPE_SEQ && isize != data_size)
		zonefs_warn(sb, "inode %lu: invalid size %lld (should be %lld)\n",
>>>>>>> b7ba80a49124 (Commit)
			    inode->i_ino, isize, data_size);

	/*
	 * First handle bad zones signaled by hardware. The mount options
	 * errors=zone-ro and errors=zone-offline result in changing the
	 * zone condition to read-only and offline respectively, as if the
	 * condition was signaled by the hardware.
	 */
<<<<<<< HEAD
	if ((z->z_flags & ZONEFS_ZONE_OFFLINE) ||
	    (sbi->s_mount_opts & ZONEFS_MNTOPT_ERRORS_ZOL)) {
		zonefs_warn(sb, "inode %lu: read/write access disabled\n",
			    inode->i_ino);
		if (!(z->z_flags & ZONEFS_ZONE_OFFLINE))
			z->z_flags |= ZONEFS_ZONE_OFFLINE;
		zonefs_inode_update_mode(inode);
		data_size = 0;
	} else if ((z->z_flags & ZONEFS_ZONE_READONLY) ||
		   (sbi->s_mount_opts & ZONEFS_MNTOPT_ERRORS_ZRO)) {
		zonefs_warn(sb, "inode %lu: write access disabled\n",
			    inode->i_ino);
		if (!(z->z_flags & ZONEFS_ZONE_READONLY))
			z->z_flags |= ZONEFS_ZONE_READONLY;
		zonefs_inode_update_mode(inode);
		data_size = isize;
	} else if (sbi->s_mount_opts & ZONEFS_MNTOPT_ERRORS_RO &&
		   data_size > isize) {
		/* Do not expose garbage data */
		data_size = isize;
=======
	if (zone->cond == BLK_ZONE_COND_OFFLINE ||
	    sbi->s_mount_opts & ZONEFS_MNTOPT_ERRORS_ZOL) {
		zonefs_warn(sb, "inode %lu: read/write access disabled\n",
			    inode->i_ino);
		if (zone->cond != BLK_ZONE_COND_OFFLINE) {
			zone->cond = BLK_ZONE_COND_OFFLINE;
			data_size = zonefs_check_zone_condition(inode, zone,
								false, false);
		}
	} else if (zone->cond == BLK_ZONE_COND_READONLY ||
		   sbi->s_mount_opts & ZONEFS_MNTOPT_ERRORS_ZRO) {
		zonefs_warn(sb, "inode %lu: write access disabled\n",
			    inode->i_ino);
		if (zone->cond != BLK_ZONE_COND_READONLY) {
			zone->cond = BLK_ZONE_COND_READONLY;
			data_size = zonefs_check_zone_condition(inode, zone,
								false, false);
		}
>>>>>>> b7ba80a49124 (Commit)
	}

	/*
	 * If the filesystem is mounted with the explicit-open mount option, we
	 * need to clear the ZONEFS_ZONE_OPEN flag if the zone transitioned to
	 * the read-only or offline condition, to avoid attempting an explicit
	 * close of the zone when the inode file is closed.
	 */
	if ((sbi->s_mount_opts & ZONEFS_MNTOPT_EXPLICIT_OPEN) &&
<<<<<<< HEAD
	    (z->z_flags & (ZONEFS_ZONE_READONLY | ZONEFS_ZONE_OFFLINE)))
		z->z_flags &= ~ZONEFS_ZONE_OPEN;
=======
	    (zone->cond == BLK_ZONE_COND_OFFLINE ||
	     zone->cond == BLK_ZONE_COND_READONLY))
		zi->i_flags &= ~ZONEFS_ZONE_OPEN;
>>>>>>> b7ba80a49124 (Commit)

	/*
	 * If error=remount-ro was specified, any error result in remounting
	 * the volume as read-only.
	 */
	if ((sbi->s_mount_opts & ZONEFS_MNTOPT_ERRORS_RO) && !sb_rdonly(sb)) {
		zonefs_warn(sb, "remounting filesystem read-only\n");
		sb->s_flags |= SB_RDONLY;
	}

	/*
	 * Update block usage stats and the inode size  to prevent access to
	 * invalid data.
	 */
	zonefs_update_stats(inode, data_size);
	zonefs_i_size_write(inode, data_size);
<<<<<<< HEAD
	z->z_wpoffset = data_size;
	zonefs_inode_account_active(inode);
=======
	zi->i_wpoffset = data_size;
	zonefs_account_active(inode);
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

/*
 * When an file IO error occurs, check the file zone to see if there is a change
 * in the zone condition (e.g. offline or read-only). For a failed write to a
 * sequential zone, the zone write pointer position must also be checked to
 * eventually correct the file size and zonefs inode write pointer offset
 * (which can be out of sync with the drive due to partial write failures).
 */
<<<<<<< HEAD
void __zonefs_io_error(struct inode *inode, bool write)
{
	struct zonefs_zone *z = zonefs_inode_zone(inode);
	struct super_block *sb = inode->i_sb;
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	unsigned int noio_flag;
	unsigned int nr_zones = 1;
=======
static void __zonefs_io_error(struct inode *inode, bool write)
{
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	struct super_block *sb = inode->i_sb;
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	unsigned int noio_flag;
	unsigned int nr_zones =
		zi->i_zone_size >> (sbi->s_zone_sectors_shift + SECTOR_SHIFT);
>>>>>>> b7ba80a49124 (Commit)
	struct zonefs_ioerr_data err = {
		.inode = inode,
		.write = write,
	};
	int ret;

	/*
<<<<<<< HEAD
	 * The only files that have more than one zone are conventional zone
	 * files with aggregated conventional zones, for which the inode zone
	 * size is always larger than the device zone size.
	 */
	if (z->z_size > bdev_zone_sectors(sb->s_bdev))
		nr_zones = z->z_size >>
			(sbi->s_zone_sectors_shift + SECTOR_SHIFT);

	/*
=======
>>>>>>> b7ba80a49124 (Commit)
	 * Memory allocations in blkdev_report_zones() can trigger a memory
	 * reclaim which may in turn cause a recursion into zonefs as well as
	 * struct request allocations for the same device. The former case may
	 * end up in a deadlock on the inode truncate mutex, while the latter
	 * may prevent IO forward progress. Executing the report zones under
	 * the GFP_NOIO context avoids both problems.
	 */
	noio_flag = memalloc_noio_save();
<<<<<<< HEAD
	ret = blkdev_report_zones(sb->s_bdev, z->z_sector, nr_zones,
=======
	ret = blkdev_report_zones(sb->s_bdev, zi->i_zsector, nr_zones,
>>>>>>> b7ba80a49124 (Commit)
				  zonefs_io_error_cb, &err);
	if (ret != nr_zones)
		zonefs_err(sb, "Get inode %lu zone information failed %d\n",
			   inode->i_ino, ret);
	memalloc_noio_restore(noio_flag);
}

<<<<<<< HEAD
=======
static void zonefs_io_error(struct inode *inode, bool write)
{
	struct zonefs_inode_info *zi = ZONEFS_I(inode);

	mutex_lock(&zi->i_truncate_mutex);
	__zonefs_io_error(inode, write);
	mutex_unlock(&zi->i_truncate_mutex);
}

static int zonefs_file_truncate(struct inode *inode, loff_t isize)
{
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	loff_t old_isize;
	enum req_op op;
	int ret = 0;

	/*
	 * Only sequential zone files can be truncated and truncation is allowed
	 * only down to a 0 size, which is equivalent to a zone reset, and to
	 * the maximum file size, which is equivalent to a zone finish.
	 */
	if (zi->i_ztype != ZONEFS_ZTYPE_SEQ)
		return -EPERM;

	if (!isize)
		op = REQ_OP_ZONE_RESET;
	else if (isize == zi->i_max_size)
		op = REQ_OP_ZONE_FINISH;
	else
		return -EPERM;

	inode_dio_wait(inode);

	/* Serialize against page faults */
	filemap_invalidate_lock(inode->i_mapping);

	/* Serialize against zonefs_iomap_begin() */
	mutex_lock(&zi->i_truncate_mutex);

	old_isize = i_size_read(inode);
	if (isize == old_isize)
		goto unlock;

	ret = zonefs_zone_mgmt(inode, op);
	if (ret)
		goto unlock;

	/*
	 * If the mount option ZONEFS_MNTOPT_EXPLICIT_OPEN is set,
	 * take care of open zones.
	 */
	if (zi->i_flags & ZONEFS_ZONE_OPEN) {
		/*
		 * Truncating a zone to EMPTY or FULL is the equivalent of
		 * closing the zone. For a truncation to 0, we need to
		 * re-open the zone to ensure new writes can be processed.
		 * For a truncation to the maximum file size, the zone is
		 * closed and writes cannot be accepted anymore, so clear
		 * the open flag.
		 */
		if (!isize)
			ret = zonefs_zone_mgmt(inode, REQ_OP_ZONE_OPEN);
		else
			zi->i_flags &= ~ZONEFS_ZONE_OPEN;
	}

	zonefs_update_stats(inode, isize);
	truncate_setsize(inode, isize);
	zi->i_wpoffset = isize;
	zonefs_account_active(inode);

unlock:
	mutex_unlock(&zi->i_truncate_mutex);
	filemap_invalidate_unlock(inode->i_mapping);

	return ret;
}

static int zonefs_inode_setattr(struct user_namespace *mnt_userns,
				struct dentry *dentry, struct iattr *iattr)
{
	struct inode *inode = d_inode(dentry);
	int ret;

	if (unlikely(IS_IMMUTABLE(inode)))
		return -EPERM;

	ret = setattr_prepare(&init_user_ns, dentry, iattr);
	if (ret)
		return ret;

	/*
	 * Since files and directories cannot be created nor deleted, do not
	 * allow setting any write attributes on the sub-directories grouping
	 * files by zone type.
	 */
	if ((iattr->ia_valid & ATTR_MODE) && S_ISDIR(inode->i_mode) &&
	    (iattr->ia_mode & 0222))
		return -EPERM;

	if (((iattr->ia_valid & ATTR_UID) &&
	     !uid_eq(iattr->ia_uid, inode->i_uid)) ||
	    ((iattr->ia_valid & ATTR_GID) &&
	     !gid_eq(iattr->ia_gid, inode->i_gid))) {
		ret = dquot_transfer(mnt_userns, inode, iattr);
		if (ret)
			return ret;
	}

	if (iattr->ia_valid & ATTR_SIZE) {
		ret = zonefs_file_truncate(inode, iattr->ia_size);
		if (ret)
			return ret;
	}

	setattr_copy(&init_user_ns, inode, iattr);

	return 0;
}

static const struct inode_operations zonefs_file_inode_operations = {
	.setattr	= zonefs_inode_setattr,
};

static int zonefs_file_fsync(struct file *file, loff_t start, loff_t end,
			     int datasync)
{
	struct inode *inode = file_inode(file);
	int ret = 0;

	if (unlikely(IS_IMMUTABLE(inode)))
		return -EPERM;

	/*
	 * Since only direct writes are allowed in sequential files, page cache
	 * flush is needed only for conventional zone files.
	 */
	if (ZONEFS_I(inode)->i_ztype == ZONEFS_ZTYPE_CNV)
		ret = file_write_and_wait_range(file, start, end);
	if (!ret)
		ret = blkdev_issue_flush(inode->i_sb->s_bdev);

	if (ret)
		zonefs_io_error(inode, true);

	return ret;
}

static vm_fault_t zonefs_filemap_page_mkwrite(struct vm_fault *vmf)
{
	struct inode *inode = file_inode(vmf->vma->vm_file);
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	vm_fault_t ret;

	if (unlikely(IS_IMMUTABLE(inode)))
		return VM_FAULT_SIGBUS;

	/*
	 * Sanity check: only conventional zone files can have shared
	 * writeable mappings.
	 */
	if (WARN_ON_ONCE(zi->i_ztype != ZONEFS_ZTYPE_CNV))
		return VM_FAULT_NOPAGE;

	sb_start_pagefault(inode->i_sb);
	file_update_time(vmf->vma->vm_file);

	/* Serialize against truncates */
	filemap_invalidate_lock_shared(inode->i_mapping);
	ret = iomap_page_mkwrite(vmf, &zonefs_write_iomap_ops);
	filemap_invalidate_unlock_shared(inode->i_mapping);

	sb_end_pagefault(inode->i_sb);
	return ret;
}

static const struct vm_operations_struct zonefs_file_vm_ops = {
	.fault		= filemap_fault,
	.map_pages	= filemap_map_pages,
	.page_mkwrite	= zonefs_filemap_page_mkwrite,
};

static int zonefs_file_mmap(struct file *file, struct vm_area_struct *vma)
{
	/*
	 * Conventional zones accept random writes, so their files can support
	 * shared writable mappings. For sequential zone files, only read
	 * mappings are possible since there are no guarantees for write
	 * ordering between msync() and page cache writeback.
	 */
	if (ZONEFS_I(file_inode(file))->i_ztype == ZONEFS_ZTYPE_SEQ &&
	    (vma->vm_flags & VM_SHARED) && (vma->vm_flags & VM_MAYWRITE))
		return -EINVAL;

	file_accessed(file);
	vma->vm_ops = &zonefs_file_vm_ops;

	return 0;
}

static loff_t zonefs_file_llseek(struct file *file, loff_t offset, int whence)
{
	loff_t isize = i_size_read(file_inode(file));

	/*
	 * Seeks are limited to below the zone size for conventional zones
	 * and below the zone write pointer for sequential zones. In both
	 * cases, this limit is the inode size.
	 */
	return generic_file_llseek_size(file, offset, whence, isize, isize);
}

static int zonefs_file_write_dio_end_io(struct kiocb *iocb, ssize_t size,
					int error, unsigned int flags)
{
	struct inode *inode = file_inode(iocb->ki_filp);
	struct zonefs_inode_info *zi = ZONEFS_I(inode);

	if (error) {
		zonefs_io_error(inode, true);
		return error;
	}

	if (size && zi->i_ztype != ZONEFS_ZTYPE_CNV) {
		/*
		 * Note that we may be seeing completions out of order,
		 * but that is not a problem since a write completed
		 * successfully necessarily means that all preceding writes
		 * were also successful. So we can safely increase the inode
		 * size to the write end location.
		 */
		mutex_lock(&zi->i_truncate_mutex);
		if (i_size_read(inode) < iocb->ki_pos + size) {
			zonefs_update_stats(inode, iocb->ki_pos + size);
			zonefs_i_size_write(inode, iocb->ki_pos + size);
		}
		mutex_unlock(&zi->i_truncate_mutex);
	}

	return 0;
}

static const struct iomap_dio_ops zonefs_write_dio_ops = {
	.end_io			= zonefs_file_write_dio_end_io,
};

static ssize_t zonefs_file_dio_append(struct kiocb *iocb, struct iov_iter *from)
{
	struct inode *inode = file_inode(iocb->ki_filp);
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	struct block_device *bdev = inode->i_sb->s_bdev;
	unsigned int max = bdev_max_zone_append_sectors(bdev);
	struct bio *bio;
	ssize_t size;
	int nr_pages;
	ssize_t ret;

	max = ALIGN_DOWN(max << SECTOR_SHIFT, inode->i_sb->s_blocksize);
	iov_iter_truncate(from, max);

	nr_pages = iov_iter_npages(from, BIO_MAX_VECS);
	if (!nr_pages)
		return 0;

	bio = bio_alloc(bdev, nr_pages,
			REQ_OP_ZONE_APPEND | REQ_SYNC | REQ_IDLE, GFP_NOFS);
	bio->bi_iter.bi_sector = zi->i_zsector;
	bio->bi_ioprio = iocb->ki_ioprio;
	if (iocb_is_dsync(iocb))
		bio->bi_opf |= REQ_FUA;

	ret = bio_iov_iter_get_pages(bio, from);
	if (unlikely(ret))
		goto out_release;

	size = bio->bi_iter.bi_size;
	task_io_account_write(size);

	if (iocb->ki_flags & IOCB_HIPRI)
		bio_set_polled(bio, iocb);

	ret = submit_bio_wait(bio);

	zonefs_file_write_dio_end_io(iocb, size, ret, 0);
	trace_zonefs_file_dio_append(inode, size, ret);

out_release:
	bio_release_pages(bio, false);
	bio_put(bio);

	if (ret >= 0) {
		iocb->ki_pos += size;
		return size;
	}

	return ret;
}

/*
 * Do not exceed the LFS limits nor the file zone size. If pos is under the
 * limit it becomes a short access. If it exceeds the limit, return -EFBIG.
 */
static loff_t zonefs_write_check_limits(struct file *file, loff_t pos,
					loff_t count)
{
	struct inode *inode = file_inode(file);
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	loff_t limit = rlimit(RLIMIT_FSIZE);
	loff_t max_size = zi->i_max_size;

	if (limit != RLIM_INFINITY) {
		if (pos >= limit) {
			send_sig(SIGXFSZ, current, 0);
			return -EFBIG;
		}
		count = min(count, limit - pos);
	}

	if (!(file->f_flags & O_LARGEFILE))
		max_size = min_t(loff_t, MAX_NON_LFS, max_size);

	if (unlikely(pos >= max_size))
		return -EFBIG;

	return min(count, max_size - pos);
}

static ssize_t zonefs_write_checks(struct kiocb *iocb, struct iov_iter *from)
{
	struct file *file = iocb->ki_filp;
	struct inode *inode = file_inode(file);
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	loff_t count;

	if (IS_SWAPFILE(inode))
		return -ETXTBSY;

	if (!iov_iter_count(from))
		return 0;

	if ((iocb->ki_flags & IOCB_NOWAIT) && !(iocb->ki_flags & IOCB_DIRECT))
		return -EINVAL;

	if (iocb->ki_flags & IOCB_APPEND) {
		if (zi->i_ztype != ZONEFS_ZTYPE_SEQ)
			return -EINVAL;
		mutex_lock(&zi->i_truncate_mutex);
		iocb->ki_pos = zi->i_wpoffset;
		mutex_unlock(&zi->i_truncate_mutex);
	}

	count = zonefs_write_check_limits(file, iocb->ki_pos,
					  iov_iter_count(from));
	if (count < 0)
		return count;

	iov_iter_truncate(from, count);
	return iov_iter_count(from);
}

/*
 * Handle direct writes. For sequential zone files, this is the only possible
 * write path. For these files, check that the user is issuing writes
 * sequentially from the end of the file. This code assumes that the block layer
 * delivers write requests to the device in sequential order. This is always the
 * case if a block IO scheduler implementing the ELEVATOR_F_ZBD_SEQ_WRITE
 * elevator feature is being used (e.g. mq-deadline). The block layer always
 * automatically select such an elevator for zoned block devices during the
 * device initialization.
 */
static ssize_t zonefs_file_dio_write(struct kiocb *iocb, struct iov_iter *from)
{
	struct inode *inode = file_inode(iocb->ki_filp);
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	struct super_block *sb = inode->i_sb;
	bool sync = is_sync_kiocb(iocb);
	bool append = false;
	ssize_t ret, count;

	/*
	 * For async direct IOs to sequential zone files, refuse IOCB_NOWAIT
	 * as this can cause write reordering (e.g. the first aio gets EAGAIN
	 * on the inode lock but the second goes through but is now unaligned).
	 */
	if (zi->i_ztype == ZONEFS_ZTYPE_SEQ && !sync &&
	    (iocb->ki_flags & IOCB_NOWAIT))
		return -EOPNOTSUPP;

	if (iocb->ki_flags & IOCB_NOWAIT) {
		if (!inode_trylock(inode))
			return -EAGAIN;
	} else {
		inode_lock(inode);
	}

	count = zonefs_write_checks(iocb, from);
	if (count <= 0) {
		ret = count;
		goto inode_unlock;
	}

	if ((iocb->ki_pos | count) & (sb->s_blocksize - 1)) {
		ret = -EINVAL;
		goto inode_unlock;
	}

	/* Enforce sequential writes (append only) in sequential zones */
	if (zi->i_ztype == ZONEFS_ZTYPE_SEQ) {
		mutex_lock(&zi->i_truncate_mutex);
		if (iocb->ki_pos != zi->i_wpoffset) {
			mutex_unlock(&zi->i_truncate_mutex);
			ret = -EINVAL;
			goto inode_unlock;
		}
		mutex_unlock(&zi->i_truncate_mutex);
		append = sync;
	}

	if (append)
		ret = zonefs_file_dio_append(iocb, from);
	else
		ret = iomap_dio_rw(iocb, from, &zonefs_write_iomap_ops,
				   &zonefs_write_dio_ops, 0, NULL, 0);
	if (zi->i_ztype == ZONEFS_ZTYPE_SEQ &&
	    (ret > 0 || ret == -EIOCBQUEUED)) {
		if (ret > 0)
			count = ret;

		/*
		 * Update the zone write pointer offset assuming the write
		 * operation succeeded. If it did not, the error recovery path
		 * will correct it. Also do active seq file accounting.
		 */
		mutex_lock(&zi->i_truncate_mutex);
		zi->i_wpoffset += count;
		zonefs_account_active(inode);
		mutex_unlock(&zi->i_truncate_mutex);
	}

inode_unlock:
	inode_unlock(inode);

	return ret;
}

static ssize_t zonefs_file_buffered_write(struct kiocb *iocb,
					  struct iov_iter *from)
{
	struct inode *inode = file_inode(iocb->ki_filp);
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	ssize_t ret;

	/*
	 * Direct IO writes are mandatory for sequential zone files so that the
	 * write IO issuing order is preserved.
	 */
	if (zi->i_ztype != ZONEFS_ZTYPE_CNV)
		return -EIO;

	if (iocb->ki_flags & IOCB_NOWAIT) {
		if (!inode_trylock(inode))
			return -EAGAIN;
	} else {
		inode_lock(inode);
	}

	ret = zonefs_write_checks(iocb, from);
	if (ret <= 0)
		goto inode_unlock;

	ret = iomap_file_buffered_write(iocb, from, &zonefs_write_iomap_ops);
	if (ret > 0)
		iocb->ki_pos += ret;
	else if (ret == -EIO)
		zonefs_io_error(inode, true);

inode_unlock:
	inode_unlock(inode);
	if (ret > 0)
		ret = generic_write_sync(iocb, ret);

	return ret;
}

static ssize_t zonefs_file_write_iter(struct kiocb *iocb, struct iov_iter *from)
{
	struct inode *inode = file_inode(iocb->ki_filp);

	if (unlikely(IS_IMMUTABLE(inode)))
		return -EPERM;

	if (sb_rdonly(inode->i_sb))
		return -EROFS;

	/* Write operations beyond the zone size are not allowed */
	if (iocb->ki_pos >= ZONEFS_I(inode)->i_max_size)
		return -EFBIG;

	if (iocb->ki_flags & IOCB_DIRECT) {
		ssize_t ret = zonefs_file_dio_write(iocb, from);
		if (ret != -ENOTBLK)
			return ret;
	}

	return zonefs_file_buffered_write(iocb, from);
}

static int zonefs_file_read_dio_end_io(struct kiocb *iocb, ssize_t size,
				       int error, unsigned int flags)
{
	if (error) {
		zonefs_io_error(file_inode(iocb->ki_filp), false);
		return error;
	}

	return 0;
}

static const struct iomap_dio_ops zonefs_read_dio_ops = {
	.end_io			= zonefs_file_read_dio_end_io,
};

static ssize_t zonefs_file_read_iter(struct kiocb *iocb, struct iov_iter *to)
{
	struct inode *inode = file_inode(iocb->ki_filp);
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	struct super_block *sb = inode->i_sb;
	loff_t isize;
	ssize_t ret;

	/* Offline zones cannot be read */
	if (unlikely(IS_IMMUTABLE(inode) && !(inode->i_mode & 0777)))
		return -EPERM;

	if (iocb->ki_pos >= zi->i_max_size)
		return 0;

	if (iocb->ki_flags & IOCB_NOWAIT) {
		if (!inode_trylock_shared(inode))
			return -EAGAIN;
	} else {
		inode_lock_shared(inode);
	}

	/* Limit read operations to written data */
	mutex_lock(&zi->i_truncate_mutex);
	isize = i_size_read(inode);
	if (iocb->ki_pos >= isize) {
		mutex_unlock(&zi->i_truncate_mutex);
		ret = 0;
		goto inode_unlock;
	}
	iov_iter_truncate(to, isize - iocb->ki_pos);
	mutex_unlock(&zi->i_truncate_mutex);

	if (iocb->ki_flags & IOCB_DIRECT) {
		size_t count = iov_iter_count(to);

		if ((iocb->ki_pos | count) & (sb->s_blocksize - 1)) {
			ret = -EINVAL;
			goto inode_unlock;
		}
		file_accessed(iocb->ki_filp);
		ret = iomap_dio_rw(iocb, to, &zonefs_read_iomap_ops,
				   &zonefs_read_dio_ops, 0, NULL, 0);
	} else {
		ret = generic_file_read_iter(iocb, to);
		if (ret == -EIO)
			zonefs_io_error(inode, false);
	}

inode_unlock:
	inode_unlock_shared(inode);

	return ret;
}

/*
 * Write open accounting is done only for sequential files.
 */
static inline bool zonefs_seq_file_need_wro(struct inode *inode,
					    struct file *file)
{
	struct zonefs_inode_info *zi = ZONEFS_I(inode);

	if (zi->i_ztype != ZONEFS_ZTYPE_SEQ)
		return false;

	if (!(file->f_mode & FMODE_WRITE))
		return false;

	return true;
}

static int zonefs_seq_file_write_open(struct inode *inode)
{
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	int ret = 0;

	mutex_lock(&zi->i_truncate_mutex);

	if (!zi->i_wr_refcnt) {
		struct zonefs_sb_info *sbi = ZONEFS_SB(inode->i_sb);
		unsigned int wro = atomic_inc_return(&sbi->s_wro_seq_files);

		if (sbi->s_mount_opts & ZONEFS_MNTOPT_EXPLICIT_OPEN) {

			if (sbi->s_max_wro_seq_files
			    && wro > sbi->s_max_wro_seq_files) {
				atomic_dec(&sbi->s_wro_seq_files);
				ret = -EBUSY;
				goto unlock;
			}

			if (i_size_read(inode) < zi->i_max_size) {
				ret = zonefs_zone_mgmt(inode, REQ_OP_ZONE_OPEN);
				if (ret) {
					atomic_dec(&sbi->s_wro_seq_files);
					goto unlock;
				}
				zi->i_flags |= ZONEFS_ZONE_OPEN;
				zonefs_account_active(inode);
			}
		}
	}

	zi->i_wr_refcnt++;

unlock:
	mutex_unlock(&zi->i_truncate_mutex);

	return ret;
}

static int zonefs_file_open(struct inode *inode, struct file *file)
{
	int ret;

	ret = generic_file_open(inode, file);
	if (ret)
		return ret;

	if (zonefs_seq_file_need_wro(inode, file))
		return zonefs_seq_file_write_open(inode);

	return 0;
}

static void zonefs_seq_file_write_close(struct inode *inode)
{
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	struct super_block *sb = inode->i_sb;
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	int ret = 0;

	mutex_lock(&zi->i_truncate_mutex);

	zi->i_wr_refcnt--;
	if (zi->i_wr_refcnt)
		goto unlock;

	/*
	 * The file zone may not be open anymore (e.g. the file was truncated to
	 * its maximum size or it was fully written). For this case, we only
	 * need to decrement the write open count.
	 */
	if (zi->i_flags & ZONEFS_ZONE_OPEN) {
		ret = zonefs_zone_mgmt(inode, REQ_OP_ZONE_CLOSE);
		if (ret) {
			__zonefs_io_error(inode, false);
			/*
			 * Leaving zones explicitly open may lead to a state
			 * where most zones cannot be written (zone resources
			 * exhausted). So take preventive action by remounting
			 * read-only.
			 */
			if (zi->i_flags & ZONEFS_ZONE_OPEN &&
			    !(sb->s_flags & SB_RDONLY)) {
				zonefs_warn(sb,
					"closing zone at %llu failed %d\n",
					zi->i_zsector, ret);
				zonefs_warn(sb,
					"remounting filesystem read-only\n");
				sb->s_flags |= SB_RDONLY;
			}
			goto unlock;
		}

		zi->i_flags &= ~ZONEFS_ZONE_OPEN;
		zonefs_account_active(inode);
	}

	atomic_dec(&sbi->s_wro_seq_files);

unlock:
	mutex_unlock(&zi->i_truncate_mutex);
}

static int zonefs_file_release(struct inode *inode, struct file *file)
{
	/*
	 * If we explicitly open a zone we must close it again as well, but the
	 * zone management operation can fail (either due to an IO error or as
	 * the zone has gone offline or read-only). Make sure we don't fail the
	 * close(2) for user-space.
	 */
	if (zonefs_seq_file_need_wro(inode, file))
		zonefs_seq_file_write_close(inode);

	return 0;
}

static const struct file_operations zonefs_file_operations = {
	.open		= zonefs_file_open,
	.release	= zonefs_file_release,
	.fsync		= zonefs_file_fsync,
	.mmap		= zonefs_file_mmap,
	.llseek		= zonefs_file_llseek,
	.read_iter	= zonefs_file_read_iter,
	.write_iter	= zonefs_file_write_iter,
	.splice_read	= generic_file_splice_read,
	.splice_write	= iter_file_splice_write,
	.iopoll		= iocb_bio_iopoll,
};

>>>>>>> b7ba80a49124 (Commit)
static struct kmem_cache *zonefs_inode_cachep;

static struct inode *zonefs_alloc_inode(struct super_block *sb)
{
	struct zonefs_inode_info *zi;

	zi = alloc_inode_sb(sb, zonefs_inode_cachep, GFP_KERNEL);
	if (!zi)
		return NULL;

	inode_init_once(&zi->i_vnode);
	mutex_init(&zi->i_truncate_mutex);
	zi->i_wr_refcnt = 0;
<<<<<<< HEAD
=======
	zi->i_flags = 0;
>>>>>>> b7ba80a49124 (Commit)

	return &zi->i_vnode;
}

static void zonefs_free_inode(struct inode *inode)
{
	kmem_cache_free(zonefs_inode_cachep, ZONEFS_I(inode));
}

/*
 * File system stat.
 */
static int zonefs_statfs(struct dentry *dentry, struct kstatfs *buf)
{
	struct super_block *sb = dentry->d_sb;
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	enum zonefs_ztype t;

	buf->f_type = ZONEFS_MAGIC;
	buf->f_bsize = sb->s_blocksize;
	buf->f_namelen = ZONEFS_NAME_MAX;

	spin_lock(&sbi->s_lock);

	buf->f_blocks = sbi->s_blocks;
	if (WARN_ON(sbi->s_used_blocks > sbi->s_blocks))
		buf->f_bfree = 0;
	else
		buf->f_bfree = buf->f_blocks - sbi->s_used_blocks;
	buf->f_bavail = buf->f_bfree;

	for (t = 0; t < ZONEFS_ZTYPE_MAX; t++) {
<<<<<<< HEAD
		if (sbi->s_zgroup[t].g_nr_zones)
			buf->f_files += sbi->s_zgroup[t].g_nr_zones + 1;
=======
		if (sbi->s_nr_files[t])
			buf->f_files += sbi->s_nr_files[t] + 1;
>>>>>>> b7ba80a49124 (Commit)
	}
	buf->f_ffree = 0;

	spin_unlock(&sbi->s_lock);

	buf->f_fsid = uuid_to_fsid(sbi->s_uuid.b);

	return 0;
}

enum {
	Opt_errors_ro, Opt_errors_zro, Opt_errors_zol, Opt_errors_repair,
	Opt_explicit_open, Opt_err,
};

static const match_table_t tokens = {
	{ Opt_errors_ro,	"errors=remount-ro"},
	{ Opt_errors_zro,	"errors=zone-ro"},
	{ Opt_errors_zol,	"errors=zone-offline"},
	{ Opt_errors_repair,	"errors=repair"},
	{ Opt_explicit_open,	"explicit-open" },
	{ Opt_err,		NULL}
};

static int zonefs_parse_options(struct super_block *sb, char *options)
{
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	substring_t args[MAX_OPT_ARGS];
	char *p;

	if (!options)
		return 0;

	while ((p = strsep(&options, ",")) != NULL) {
		int token;

		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case Opt_errors_ro:
			sbi->s_mount_opts &= ~ZONEFS_MNTOPT_ERRORS_MASK;
			sbi->s_mount_opts |= ZONEFS_MNTOPT_ERRORS_RO;
			break;
		case Opt_errors_zro:
			sbi->s_mount_opts &= ~ZONEFS_MNTOPT_ERRORS_MASK;
			sbi->s_mount_opts |= ZONEFS_MNTOPT_ERRORS_ZRO;
			break;
		case Opt_errors_zol:
			sbi->s_mount_opts &= ~ZONEFS_MNTOPT_ERRORS_MASK;
			sbi->s_mount_opts |= ZONEFS_MNTOPT_ERRORS_ZOL;
			break;
		case Opt_errors_repair:
			sbi->s_mount_opts &= ~ZONEFS_MNTOPT_ERRORS_MASK;
			sbi->s_mount_opts |= ZONEFS_MNTOPT_ERRORS_REPAIR;
			break;
		case Opt_explicit_open:
			sbi->s_mount_opts |= ZONEFS_MNTOPT_EXPLICIT_OPEN;
			break;
		default:
			return -EINVAL;
		}
	}

	return 0;
}

static int zonefs_show_options(struct seq_file *seq, struct dentry *root)
{
	struct zonefs_sb_info *sbi = ZONEFS_SB(root->d_sb);

	if (sbi->s_mount_opts & ZONEFS_MNTOPT_ERRORS_RO)
		seq_puts(seq, ",errors=remount-ro");
	if (sbi->s_mount_opts & ZONEFS_MNTOPT_ERRORS_ZRO)
		seq_puts(seq, ",errors=zone-ro");
	if (sbi->s_mount_opts & ZONEFS_MNTOPT_ERRORS_ZOL)
		seq_puts(seq, ",errors=zone-offline");
	if (sbi->s_mount_opts & ZONEFS_MNTOPT_ERRORS_REPAIR)
		seq_puts(seq, ",errors=repair");

	return 0;
}

static int zonefs_remount(struct super_block *sb, int *flags, char *data)
{
	sync_filesystem(sb);

	return zonefs_parse_options(sb, data);
}

<<<<<<< HEAD
static int zonefs_inode_setattr(struct mnt_idmap *idmap,
				struct dentry *dentry, struct iattr *iattr)
{
	struct inode *inode = d_inode(dentry);
	int ret;

	if (unlikely(IS_IMMUTABLE(inode)))
		return -EPERM;

	ret = setattr_prepare(&nop_mnt_idmap, dentry, iattr);
	if (ret)
		return ret;

	/*
	 * Since files and directories cannot be created nor deleted, do not
	 * allow setting any write attributes on the sub-directories grouping
	 * files by zone type.
	 */
	if ((iattr->ia_valid & ATTR_MODE) && S_ISDIR(inode->i_mode) &&
	    (iattr->ia_mode & 0222))
		return -EPERM;

	if (((iattr->ia_valid & ATTR_UID) &&
	     !uid_eq(iattr->ia_uid, inode->i_uid)) ||
	    ((iattr->ia_valid & ATTR_GID) &&
	     !gid_eq(iattr->ia_gid, inode->i_gid))) {
		ret = dquot_transfer(&nop_mnt_idmap, inode, iattr);
		if (ret)
			return ret;
	}

	if (iattr->ia_valid & ATTR_SIZE) {
		ret = zonefs_file_truncate(inode, iattr->ia_size);
		if (ret)
			return ret;
	}

	setattr_copy(&nop_mnt_idmap, inode, iattr);

	if (S_ISREG(inode->i_mode)) {
		struct zonefs_zone *z = zonefs_inode_zone(inode);

		z->z_mode = inode->i_mode;
		z->z_uid = inode->i_uid;
		z->z_gid = inode->i_gid;
	}

	return 0;
}

static const struct inode_operations zonefs_file_inode_operations = {
	.setattr	= zonefs_inode_setattr,
};

static long zonefs_fname_to_fno(const struct qstr *fname)
{
	const char *name = fname->name;
	unsigned int len = fname->len;
	long fno = 0, shift = 1;
	const char *rname;
	char c = *name;
	unsigned int i;

	/*
	 * File names are always a base-10 number string without any
	 * leading 0s.
	 */
	if (!isdigit(c))
		return -ENOENT;

	if (len > 1 && c == '0')
		return -ENOENT;

	if (len == 1)
		return c - '0';

	for (i = 0, rname = name + len - 1; i < len; i++, rname--) {
		c = *rname;
		if (!isdigit(c))
			return -ENOENT;
		fno += (c - '0') * shift;
		shift *= 10;
	}

	return fno;
}

static struct inode *zonefs_get_file_inode(struct inode *dir,
					   struct dentry *dentry)
{
	struct zonefs_zone_group *zgroup = dir->i_private;
	struct super_block *sb = dir->i_sb;
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	struct zonefs_zone *z;
	struct inode *inode;
	ino_t ino;
	long fno;

	/* Get the file number from the file name */
	fno = zonefs_fname_to_fno(&dentry->d_name);
	if (fno < 0)
		return ERR_PTR(fno);

	if (!zgroup->g_nr_zones || fno >= zgroup->g_nr_zones)
		return ERR_PTR(-ENOENT);

	z = &zgroup->g_zones[fno];
	ino = z->z_sector >> sbi->s_zone_sectors_shift;
	inode = iget_locked(sb, ino);
	if (!inode)
		return ERR_PTR(-ENOMEM);
	if (!(inode->i_state & I_NEW)) {
		WARN_ON_ONCE(inode->i_private != z);
		return inode;
	}

	inode->i_ino = ino;
	inode->i_mode = z->z_mode;
	inode->i_ctime = inode->i_mtime = inode->i_atime = dir->i_ctime;
	inode->i_uid = z->z_uid;
	inode->i_gid = z->z_gid;
	inode->i_size = z->z_wpoffset;
	inode->i_blocks = z->z_capacity >> SECTOR_SHIFT;
	inode->i_private = z;
=======
static const struct super_operations zonefs_sops = {
	.alloc_inode	= zonefs_alloc_inode,
	.free_inode	= zonefs_free_inode,
	.statfs		= zonefs_statfs,
	.remount_fs	= zonefs_remount,
	.show_options	= zonefs_show_options,
};

static const struct inode_operations zonefs_dir_inode_operations = {
	.lookup		= simple_lookup,
	.setattr	= zonefs_inode_setattr,
};

static void zonefs_init_dir_inode(struct inode *parent, struct inode *inode,
				  enum zonefs_ztype type)
{
	struct super_block *sb = parent->i_sb;

	inode->i_ino = bdev_nr_zones(sb->s_bdev) + type + 1;
	inode_init_owner(&init_user_ns, inode, parent, S_IFDIR | 0555);
	inode->i_op = &zonefs_dir_inode_operations;
	inode->i_fop = &simple_dir_operations;
	set_nlink(inode, 2);
	inc_nlink(parent);
}

static int zonefs_init_file_inode(struct inode *inode, struct blk_zone *zone,
				  enum zonefs_ztype type)
{
	struct super_block *sb = inode->i_sb;
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	struct zonefs_inode_info *zi = ZONEFS_I(inode);
	int ret = 0;

	inode->i_ino = zone->start >> sbi->s_zone_sectors_shift;
	inode->i_mode = S_IFREG | sbi->s_perm;

	zi->i_ztype = type;
	zi->i_zsector = zone->start;
	zi->i_zone_size = zone->len << SECTOR_SHIFT;

	zi->i_max_size = min_t(loff_t, MAX_LFS_FILESIZE,
			       zone->capacity << SECTOR_SHIFT);
	zi->i_wpoffset = zonefs_check_zone_condition(inode, zone, true, true);

	inode->i_uid = sbi->s_uid;
	inode->i_gid = sbi->s_gid;
	inode->i_size = zi->i_wpoffset;
	inode->i_blocks = zi->i_max_size >> SECTOR_SHIFT;
>>>>>>> b7ba80a49124 (Commit)

	inode->i_op = &zonefs_file_inode_operations;
	inode->i_fop = &zonefs_file_operations;
	inode->i_mapping->a_ops = &zonefs_file_aops;

<<<<<<< HEAD
	/* Update the inode access rights depending on the zone condition */
	zonefs_inode_update_mode(inode);

	unlock_new_inode(inode);

	return inode;
}

static struct inode *zonefs_get_zgroup_inode(struct super_block *sb,
					     enum zonefs_ztype ztype)
{
	struct inode *root = d_inode(sb->s_root);
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	struct inode *inode;
	ino_t ino = bdev_nr_zones(sb->s_bdev) + ztype + 1;

	inode = iget_locked(sb, ino);
	if (!inode)
		return ERR_PTR(-ENOMEM);
	if (!(inode->i_state & I_NEW))
		return inode;

	inode->i_ino = ino;
	inode_init_owner(&nop_mnt_idmap, inode, root, S_IFDIR | 0555);
	inode->i_size = sbi->s_zgroup[ztype].g_nr_zones;
	inode->i_ctime = inode->i_mtime = inode->i_atime = root->i_ctime;
	inode->i_private = &sbi->s_zgroup[ztype];
	set_nlink(inode, 2);

	inode->i_op = &zonefs_dir_inode_operations;
	inode->i_fop = &zonefs_dir_operations;

	unlock_new_inode(inode);

	return inode;
}


static struct inode *zonefs_get_dir_inode(struct inode *dir,
					  struct dentry *dentry)
{
	struct super_block *sb = dir->i_sb;
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	const char *name = dentry->d_name.name;
	enum zonefs_ztype ztype;

	/*
	 * We only need to check for the "seq" directory and
	 * the "cnv" directory if we have conventional zones.
	 */
	if (dentry->d_name.len != 3)
		return ERR_PTR(-ENOENT);

	for (ztype = 0; ztype < ZONEFS_ZTYPE_MAX; ztype++) {
		if (sbi->s_zgroup[ztype].g_nr_zones &&
		    memcmp(name, zonefs_zgroup_name(ztype), 3) == 0)
			break;
	}
	if (ztype == ZONEFS_ZTYPE_MAX)
		return ERR_PTR(-ENOENT);

	return zonefs_get_zgroup_inode(sb, ztype);
}

static struct dentry *zonefs_lookup(struct inode *dir, struct dentry *dentry,
				    unsigned int flags)
{
	struct inode *inode;

	if (dentry->d_name.len > ZONEFS_NAME_MAX)
		return ERR_PTR(-ENAMETOOLONG);

	if (dir == d_inode(dir->i_sb->s_root))
		inode = zonefs_get_dir_inode(dir, dentry);
	else
		inode = zonefs_get_file_inode(dir, dentry);
	if (IS_ERR(inode))
		return ERR_CAST(inode);

	return d_splice_alias(inode, dentry);
}

static int zonefs_readdir_root(struct file *file, struct dir_context *ctx)
{
	struct inode *inode = file_inode(file);
	struct super_block *sb = inode->i_sb;
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	enum zonefs_ztype ztype = ZONEFS_ZTYPE_CNV;
	ino_t base_ino = bdev_nr_zones(sb->s_bdev) + 1;

	if (ctx->pos >= inode->i_size)
		return 0;

	if (!dir_emit_dots(file, ctx))
		return 0;

	if (ctx->pos == 2) {
		if (!sbi->s_zgroup[ZONEFS_ZTYPE_CNV].g_nr_zones)
			ztype = ZONEFS_ZTYPE_SEQ;

		if (!dir_emit(ctx, zonefs_zgroup_name(ztype), 3,
			      base_ino + ztype, DT_DIR))
			return 0;
		ctx->pos++;
	}

	if (ctx->pos == 3 && ztype != ZONEFS_ZTYPE_SEQ) {
		ztype = ZONEFS_ZTYPE_SEQ;
		if (!dir_emit(ctx, zonefs_zgroup_name(ztype), 3,
			      base_ino + ztype, DT_DIR))
			return 0;
		ctx->pos++;
	}

	return 0;
}

static int zonefs_readdir_zgroup(struct file *file,
				 struct dir_context *ctx)
{
	struct inode *inode = file_inode(file);
	struct zonefs_zone_group *zgroup = inode->i_private;
	struct super_block *sb = inode->i_sb;
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	struct zonefs_zone *z;
	int fname_len;
	char *fname;
	ino_t ino;
	int f;

	/*
	 * The size of zone group directories is equal to the number
	 * of zone files in the group and does note include the "." and
	 * ".." entries. Hence the "+ 2" here.
	 */
	if (ctx->pos >= inode->i_size + 2)
		return 0;

	if (!dir_emit_dots(file, ctx))
		return 0;

	fname = kmalloc(ZONEFS_NAME_MAX, GFP_KERNEL);
	if (!fname)
		return -ENOMEM;

	for (f = ctx->pos - 2; f < zgroup->g_nr_zones; f++) {
		z = &zgroup->g_zones[f];
		ino = z->z_sector >> sbi->s_zone_sectors_shift;
		fname_len = snprintf(fname, ZONEFS_NAME_MAX - 1, "%u", f);
		if (!dir_emit(ctx, fname, fname_len, ino, DT_REG))
			break;
		ctx->pos++;
	}

	kfree(fname);

	return 0;
}

static int zonefs_readdir(struct file *file, struct dir_context *ctx)
{
	struct inode *inode = file_inode(file);

	if (inode == d_inode(inode->i_sb->s_root))
		return zonefs_readdir_root(file, ctx);

	return zonefs_readdir_zgroup(file, ctx);
}

const struct inode_operations zonefs_dir_inode_operations = {
	.lookup		= zonefs_lookup,
	.setattr	= zonefs_inode_setattr,
};

const struct file_operations zonefs_dir_operations = {
	.llseek		= generic_file_llseek,
	.read		= generic_read_dir,
	.iterate_shared	= zonefs_readdir,
};

struct zonefs_zone_data {
	struct super_block	*sb;
	unsigned int		nr_zones[ZONEFS_ZTYPE_MAX];
	sector_t		cnv_zone_start;
	struct blk_zone		*zones;
};

=======
	sb->s_maxbytes = max(zi->i_max_size, sb->s_maxbytes);
	sbi->s_blocks += zi->i_max_size >> sb->s_blocksize_bits;
	sbi->s_used_blocks += zi->i_wpoffset >> sb->s_blocksize_bits;

	mutex_lock(&zi->i_truncate_mutex);

	/*
	 * For sequential zones, make sure that any open zone is closed first
	 * to ensure that the initial number of open zones is 0, in sync with
	 * the open zone accounting done when the mount option
	 * ZONEFS_MNTOPT_EXPLICIT_OPEN is used.
	 */
	if (type == ZONEFS_ZTYPE_SEQ &&
	    (zone->cond == BLK_ZONE_COND_IMP_OPEN ||
	     zone->cond == BLK_ZONE_COND_EXP_OPEN)) {
		ret = zonefs_zone_mgmt(inode, REQ_OP_ZONE_CLOSE);
		if (ret)
			goto unlock;
	}

	zonefs_account_active(inode);

unlock:
	mutex_unlock(&zi->i_truncate_mutex);

	return ret;
}

static struct dentry *zonefs_create_inode(struct dentry *parent,
					const char *name, struct blk_zone *zone,
					enum zonefs_ztype type)
{
	struct inode *dir = d_inode(parent);
	struct dentry *dentry;
	struct inode *inode;
	int ret;

	dentry = d_alloc_name(parent, name);
	if (!dentry)
		return NULL;

	inode = new_inode(parent->d_sb);
	if (!inode)
		goto dput;

	inode->i_ctime = inode->i_mtime = inode->i_atime = dir->i_ctime;
	if (zone) {
		ret = zonefs_init_file_inode(inode, zone, type);
		if (ret) {
			iput(inode);
			goto dput;
		}
	} else {
		zonefs_init_dir_inode(dir, inode, type);
	}

	d_add(dentry, inode);
	dir->i_size++;

	return dentry;

dput:
	dput(dentry);

	return NULL;
}

struct zonefs_zone_data {
	struct super_block	*sb;
	unsigned int		nr_zones[ZONEFS_ZTYPE_MAX];
	struct blk_zone		*zones;
};

/*
 * Create a zone group and populate it with zone files.
 */
static int zonefs_create_zgroup(struct zonefs_zone_data *zd,
				enum zonefs_ztype type)
{
	struct super_block *sb = zd->sb;
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	struct blk_zone *zone, *next, *end;
	const char *zgroup_name;
	char *file_name;
	struct dentry *dir;
	unsigned int n = 0;
	int ret;

	/* If the group is empty, there is nothing to do */
	if (!zd->nr_zones[type])
		return 0;

	file_name = kmalloc(ZONEFS_NAME_MAX, GFP_KERNEL);
	if (!file_name)
		return -ENOMEM;

	if (type == ZONEFS_ZTYPE_CNV)
		zgroup_name = "cnv";
	else
		zgroup_name = "seq";

	dir = zonefs_create_inode(sb->s_root, zgroup_name, NULL, type);
	if (!dir) {
		ret = -ENOMEM;
		goto free;
	}

	/*
	 * The first zone contains the super block: skip it.
	 */
	end = zd->zones + bdev_nr_zones(sb->s_bdev);
	for (zone = &zd->zones[1]; zone < end; zone = next) {

		next = zone + 1;
		if (zonefs_zone_type(zone) != type)
			continue;

		/*
		 * For conventional zones, contiguous zones can be aggregated
		 * together to form larger files. Note that this overwrites the
		 * length of the first zone of the set of contiguous zones
		 * aggregated together. If one offline or read-only zone is
		 * found, assume that all zones aggregated have the same
		 * condition.
		 */
		if (type == ZONEFS_ZTYPE_CNV &&
		    (sbi->s_features & ZONEFS_F_AGGRCNV)) {
			for (; next < end; next++) {
				if (zonefs_zone_type(next) != type)
					break;
				zone->len += next->len;
				zone->capacity += next->capacity;
				if (next->cond == BLK_ZONE_COND_READONLY &&
				    zone->cond != BLK_ZONE_COND_OFFLINE)
					zone->cond = BLK_ZONE_COND_READONLY;
				else if (next->cond == BLK_ZONE_COND_OFFLINE)
					zone->cond = BLK_ZONE_COND_OFFLINE;
			}
			if (zone->capacity != zone->len) {
				zonefs_err(sb, "Invalid conventional zone capacity\n");
				ret = -EINVAL;
				goto free;
			}
		}

		/*
		 * Use the file number within its group as file name.
		 */
		snprintf(file_name, ZONEFS_NAME_MAX - 1, "%u", n);
		if (!zonefs_create_inode(dir, file_name, zone, type)) {
			ret = -ENOMEM;
			goto free;
		}

		n++;
	}

	zonefs_info(sb, "Zone group \"%s\" has %u file%s\n",
		    zgroup_name, n, n > 1 ? "s" : "");

	sbi->s_nr_files[type] = n;
	ret = 0;

free:
	kfree(file_name);

	return ret;
}

>>>>>>> b7ba80a49124 (Commit)
static int zonefs_get_zone_info_cb(struct blk_zone *zone, unsigned int idx,
				   void *data)
{
	struct zonefs_zone_data *zd = data;
<<<<<<< HEAD
	struct super_block *sb = zd->sb;
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);

	/*
	 * We do not care about the first zone: it contains the super block
	 * and not exposed as a file.
	 */
	if (!idx)
		return 0;

	/*
	 * Count the number of zones that will be exposed as files.
	 * For sequential zones, we always have as many files as zones.
	 * FOr conventional zones, the number of files depends on if we have
	 * conventional zones aggregation enabled.
	 */
	switch (zone->type) {
	case BLK_ZONE_TYPE_CONVENTIONAL:
		if (sbi->s_features & ZONEFS_F_AGGRCNV) {
			/* One file per set of contiguous conventional zones */
			if (!(sbi->s_zgroup[ZONEFS_ZTYPE_CNV].g_nr_zones) ||
			    zone->start != zd->cnv_zone_start)
				sbi->s_zgroup[ZONEFS_ZTYPE_CNV].g_nr_zones++;
			zd->cnv_zone_start = zone->start + zone->len;
		} else {
			/* One file per zone */
			sbi->s_zgroup[ZONEFS_ZTYPE_CNV].g_nr_zones++;
		}
		break;
	case BLK_ZONE_TYPE_SEQWRITE_REQ:
	case BLK_ZONE_TYPE_SEQWRITE_PREF:
		sbi->s_zgroup[ZONEFS_ZTYPE_SEQ].g_nr_zones++;
=======

	/*
	 * Count the number of usable zones: the first zone at index 0 contains
	 * the super block and is ignored.
	 */
	switch (zone->type) {
	case BLK_ZONE_TYPE_CONVENTIONAL:
		zone->wp = zone->start + zone->len;
		if (idx)
			zd->nr_zones[ZONEFS_ZTYPE_CNV]++;
		break;
	case BLK_ZONE_TYPE_SEQWRITE_REQ:
	case BLK_ZONE_TYPE_SEQWRITE_PREF:
		if (idx)
			zd->nr_zones[ZONEFS_ZTYPE_SEQ]++;
>>>>>>> b7ba80a49124 (Commit)
		break;
	default:
		zonefs_err(zd->sb, "Unsupported zone type 0x%x\n",
			   zone->type);
		return -EIO;
	}

	memcpy(&zd->zones[idx], zone, sizeof(struct blk_zone));

	return 0;
}

static int zonefs_get_zone_info(struct zonefs_zone_data *zd)
{
	struct block_device *bdev = zd->sb->s_bdev;
	int ret;

	zd->zones = kvcalloc(bdev_nr_zones(bdev), sizeof(struct blk_zone),
			     GFP_KERNEL);
	if (!zd->zones)
		return -ENOMEM;

	/* Get zones information from the device */
	ret = blkdev_report_zones(bdev, 0, BLK_ALL_ZONES,
				  zonefs_get_zone_info_cb, zd);
	if (ret < 0) {
		zonefs_err(zd->sb, "Zone report failed %d\n", ret);
		return ret;
	}

	if (ret != bdev_nr_zones(bdev)) {
		zonefs_err(zd->sb, "Invalid zone report (%d/%u zones)\n",
			   ret, bdev_nr_zones(bdev));
		return -EIO;
	}

	return 0;
}

<<<<<<< HEAD
static inline void zonefs_free_zone_info(struct zonefs_zone_data *zd)
=======
static inline void zonefs_cleanup_zone_info(struct zonefs_zone_data *zd)
>>>>>>> b7ba80a49124 (Commit)
{
	kvfree(zd->zones);
}

/*
<<<<<<< HEAD
 * Create a zone group and populate it with zone files.
 */
static int zonefs_init_zgroup(struct super_block *sb,
			      struct zonefs_zone_data *zd,
			      enum zonefs_ztype ztype)
{
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	struct zonefs_zone_group *zgroup = &sbi->s_zgroup[ztype];
	struct blk_zone *zone, *next, *end;
	struct zonefs_zone *z;
	unsigned int n = 0;
	int ret;

	/* Allocate the zone group. If it is empty, we have nothing to do. */
	if (!zgroup->g_nr_zones)
		return 0;

	zgroup->g_zones = kvcalloc(zgroup->g_nr_zones,
				   sizeof(struct zonefs_zone), GFP_KERNEL);
	if (!zgroup->g_zones)
		return -ENOMEM;

	/*
	 * Initialize the zone groups using the device zone information.
	 * We always skip the first zone as it contains the super block
	 * and is not use to back a file.
	 */
	end = zd->zones + bdev_nr_zones(sb->s_bdev);
	for (zone = &zd->zones[1]; zone < end; zone = next) {

		next = zone + 1;
		if (zonefs_zone_type(zone) != ztype)
			continue;

		if (WARN_ON_ONCE(n >= zgroup->g_nr_zones))
			return -EINVAL;

		/*
		 * For conventional zones, contiguous zones can be aggregated
		 * together to form larger files. Note that this overwrites the
		 * length of the first zone of the set of contiguous zones
		 * aggregated together. If one offline or read-only zone is
		 * found, assume that all zones aggregated have the same
		 * condition.
		 */
		if (ztype == ZONEFS_ZTYPE_CNV &&
		    (sbi->s_features & ZONEFS_F_AGGRCNV)) {
			for (; next < end; next++) {
				if (zonefs_zone_type(next) != ztype)
					break;
				zone->len += next->len;
				zone->capacity += next->capacity;
				if (next->cond == BLK_ZONE_COND_READONLY &&
				    zone->cond != BLK_ZONE_COND_OFFLINE)
					zone->cond = BLK_ZONE_COND_READONLY;
				else if (next->cond == BLK_ZONE_COND_OFFLINE)
					zone->cond = BLK_ZONE_COND_OFFLINE;
			}
		}

		z = &zgroup->g_zones[n];
		if (ztype == ZONEFS_ZTYPE_CNV)
			z->z_flags |= ZONEFS_ZONE_CNV;
		z->z_sector = zone->start;
		z->z_size = zone->len << SECTOR_SHIFT;
		if (z->z_size > bdev_zone_sectors(sb->s_bdev) << SECTOR_SHIFT &&
		    !(sbi->s_features & ZONEFS_F_AGGRCNV)) {
			zonefs_err(sb,
				"Invalid zone size %llu (device zone sectors %llu)\n",
				z->z_size,
				bdev_zone_sectors(sb->s_bdev) << SECTOR_SHIFT);
			return -EINVAL;
		}

		z->z_capacity = min_t(loff_t, MAX_LFS_FILESIZE,
				      zone->capacity << SECTOR_SHIFT);
		z->z_wpoffset = zonefs_check_zone_condition(sb, z, zone);

		z->z_mode = S_IFREG | sbi->s_perm;
		z->z_uid = sbi->s_uid;
		z->z_gid = sbi->s_gid;

		/*
		 * Let zonefs_inode_update_mode() know that we will need
		 * special initialization of the inode mode the first time
		 * it is accessed.
		 */
		z->z_flags |= ZONEFS_ZONE_INIT_MODE;

		sb->s_maxbytes = max(z->z_capacity, sb->s_maxbytes);
		sbi->s_blocks += z->z_capacity >> sb->s_blocksize_bits;
		sbi->s_used_blocks += z->z_wpoffset >> sb->s_blocksize_bits;

		/*
		 * For sequential zones, make sure that any open zone is closed
		 * first to ensure that the initial number of open zones is 0,
		 * in sync with the open zone accounting done when the mount
		 * option ZONEFS_MNTOPT_EXPLICIT_OPEN is used.
		 */
		if (ztype == ZONEFS_ZTYPE_SEQ &&
		    (zone->cond == BLK_ZONE_COND_IMP_OPEN ||
		     zone->cond == BLK_ZONE_COND_EXP_OPEN)) {
			ret = zonefs_zone_mgmt(sb, z, REQ_OP_ZONE_CLOSE);
			if (ret)
				return ret;
		}

		zonefs_account_active(sb, z);

		n++;
	}

	if (WARN_ON_ONCE(n != zgroup->g_nr_zones))
		return -EINVAL;

	zonefs_info(sb, "Zone group \"%s\" has %u file%s\n",
		    zonefs_zgroup_name(ztype),
		    zgroup->g_nr_zones,
		    zgroup->g_nr_zones > 1 ? "s" : "");

	return 0;
}

static void zonefs_free_zgroups(struct super_block *sb)
{
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	enum zonefs_ztype ztype;

	if (!sbi)
		return;

	for (ztype = 0; ztype < ZONEFS_ZTYPE_MAX; ztype++) {
		kvfree(sbi->s_zgroup[ztype].g_zones);
		sbi->s_zgroup[ztype].g_zones = NULL;
	}
}

/*
 * Create a zone group and populate it with zone files.
 */
static int zonefs_init_zgroups(struct super_block *sb)
{
	struct zonefs_zone_data zd;
	enum zonefs_ztype ztype;
	int ret;

	/* First get the device zone information */
	memset(&zd, 0, sizeof(struct zonefs_zone_data));
	zd.sb = sb;
	ret = zonefs_get_zone_info(&zd);
	if (ret)
		goto cleanup;

	/* Allocate and initialize the zone groups */
	for (ztype = 0; ztype < ZONEFS_ZTYPE_MAX; ztype++) {
		ret = zonefs_init_zgroup(sb, &zd, ztype);
		if (ret) {
			zonefs_info(sb,
				    "Zone group \"%s\" initialization failed\n",
				    zonefs_zgroup_name(ztype));
			break;
		}
	}

cleanup:
	zonefs_free_zone_info(&zd);
	if (ret)
		zonefs_free_zgroups(sb);

	return ret;
}

/*
=======
>>>>>>> b7ba80a49124 (Commit)
 * Read super block information from the device.
 */
static int zonefs_read_super(struct super_block *sb)
{
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	struct zonefs_super *super;
	u32 crc, stored_crc;
	struct page *page;
	struct bio_vec bio_vec;
	struct bio bio;
	int ret;

	page = alloc_page(GFP_KERNEL);
	if (!page)
		return -ENOMEM;

	bio_init(&bio, sb->s_bdev, &bio_vec, 1, REQ_OP_READ);
	bio.bi_iter.bi_sector = 0;
	bio_add_page(&bio, page, PAGE_SIZE, 0);

	ret = submit_bio_wait(&bio);
	if (ret)
		goto free_page;

	super = page_address(page);

	ret = -EINVAL;
	if (le32_to_cpu(super->s_magic) != ZONEFS_MAGIC)
		goto free_page;

	stored_crc = le32_to_cpu(super->s_crc);
	super->s_crc = 0;
	crc = crc32(~0U, (unsigned char *)super, sizeof(struct zonefs_super));
	if (crc != stored_crc) {
		zonefs_err(sb, "Invalid checksum (Expected 0x%08x, got 0x%08x)",
			   crc, stored_crc);
		goto free_page;
	}

	sbi->s_features = le64_to_cpu(super->s_features);
	if (sbi->s_features & ~ZONEFS_F_DEFINED_FEATURES) {
		zonefs_err(sb, "Unknown features set 0x%llx\n",
			   sbi->s_features);
		goto free_page;
	}

	if (sbi->s_features & ZONEFS_F_UID) {
		sbi->s_uid = make_kuid(current_user_ns(),
				       le32_to_cpu(super->s_uid));
		if (!uid_valid(sbi->s_uid)) {
			zonefs_err(sb, "Invalid UID feature\n");
			goto free_page;
		}
	}

	if (sbi->s_features & ZONEFS_F_GID) {
		sbi->s_gid = make_kgid(current_user_ns(),
				       le32_to_cpu(super->s_gid));
		if (!gid_valid(sbi->s_gid)) {
			zonefs_err(sb, "Invalid GID feature\n");
			goto free_page;
		}
	}

	if (sbi->s_features & ZONEFS_F_PERM)
		sbi->s_perm = le32_to_cpu(super->s_perm);

	if (memchr_inv(super->s_reserved, 0, sizeof(super->s_reserved))) {
		zonefs_err(sb, "Reserved area is being used\n");
		goto free_page;
	}

	import_uuid(&sbi->s_uuid, super->s_uuid);
	ret = 0;

free_page:
	__free_page(page);

	return ret;
}

<<<<<<< HEAD
static const struct super_operations zonefs_sops = {
	.alloc_inode	= zonefs_alloc_inode,
	.free_inode	= zonefs_free_inode,
	.statfs		= zonefs_statfs,
	.remount_fs	= zonefs_remount,
	.show_options	= zonefs_show_options,
};

static int zonefs_get_zgroup_inodes(struct super_block *sb)
{
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	struct inode *dir_inode;
	enum zonefs_ztype ztype;

	for (ztype = 0; ztype < ZONEFS_ZTYPE_MAX; ztype++) {
		if (!sbi->s_zgroup[ztype].g_nr_zones)
			continue;

		dir_inode = zonefs_get_zgroup_inode(sb, ztype);
		if (IS_ERR(dir_inode))
			return PTR_ERR(dir_inode);

		sbi->s_zgroup[ztype].g_inode = dir_inode;
	}

	return 0;
}

static void zonefs_release_zgroup_inodes(struct super_block *sb)
{
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);
	enum zonefs_ztype ztype;

	if (!sbi)
		return;

	for (ztype = 0; ztype < ZONEFS_ZTYPE_MAX; ztype++) {
		if (sbi->s_zgroup[ztype].g_inode) {
			iput(sbi->s_zgroup[ztype].g_inode);
			sbi->s_zgroup[ztype].g_inode = NULL;
		}
	}
}

=======
>>>>>>> b7ba80a49124 (Commit)
/*
 * Check that the device is zoned. If it is, get the list of zones and create
 * sub-directories and files according to the device zone configuration and
 * format options.
 */
static int zonefs_fill_super(struct super_block *sb, void *data, int silent)
{
<<<<<<< HEAD
	struct zonefs_sb_info *sbi;
	struct inode *inode;
	enum zonefs_ztype ztype;
=======
	struct zonefs_zone_data zd;
	struct zonefs_sb_info *sbi;
	struct inode *inode;
	enum zonefs_ztype t;
>>>>>>> b7ba80a49124 (Commit)
	int ret;

	if (!bdev_is_zoned(sb->s_bdev)) {
		zonefs_err(sb, "Not a zoned block device\n");
		return -EINVAL;
	}

	/*
	 * Initialize super block information: the maximum file size is updated
	 * when the zone files are created so that the format option
	 * ZONEFS_F_AGGRCNV which increases the maximum file size of a file
	 * beyond the zone size is taken into account.
	 */
	sbi = kzalloc(sizeof(*sbi), GFP_KERNEL);
	if (!sbi)
		return -ENOMEM;

	spin_lock_init(&sbi->s_lock);
	sb->s_fs_info = sbi;
	sb->s_magic = ZONEFS_MAGIC;
	sb->s_maxbytes = 0;
	sb->s_op = &zonefs_sops;
	sb->s_time_gran	= 1;

	/*
	 * The block size is set to the device zone write granularity to ensure
	 * that write operations are always aligned according to the device
	 * interface constraints.
	 */
	sb_set_blocksize(sb, bdev_zone_write_granularity(sb->s_bdev));
	sbi->s_zone_sectors_shift = ilog2(bdev_zone_sectors(sb->s_bdev));
	sbi->s_uid = GLOBAL_ROOT_UID;
	sbi->s_gid = GLOBAL_ROOT_GID;
	sbi->s_perm = 0640;
	sbi->s_mount_opts = ZONEFS_MNTOPT_ERRORS_RO;

	atomic_set(&sbi->s_wro_seq_files, 0);
	sbi->s_max_wro_seq_files = bdev_max_open_zones(sb->s_bdev);
	atomic_set(&sbi->s_active_seq_files, 0);
	sbi->s_max_active_seq_files = bdev_max_active_zones(sb->s_bdev);

	ret = zonefs_read_super(sb);
	if (ret)
		return ret;

	ret = zonefs_parse_options(sb, data);
	if (ret)
		return ret;

<<<<<<< HEAD
=======
	memset(&zd, 0, sizeof(struct zonefs_zone_data));
	zd.sb = sb;
	ret = zonefs_get_zone_info(&zd);
	if (ret)
		goto cleanup;

	ret = zonefs_sysfs_register(sb);
	if (ret)
		goto cleanup;

>>>>>>> b7ba80a49124 (Commit)
	zonefs_info(sb, "Mounting %u zones", bdev_nr_zones(sb->s_bdev));

	if (!sbi->s_max_wro_seq_files &&
	    !sbi->s_max_active_seq_files &&
	    sbi->s_mount_opts & ZONEFS_MNTOPT_EXPLICIT_OPEN) {
		zonefs_info(sb,
			"No open and active zone limits. Ignoring explicit_open mount option\n");
		sbi->s_mount_opts &= ~ZONEFS_MNTOPT_EXPLICIT_OPEN;
	}

<<<<<<< HEAD
	/* Initialize the zone groups */
	ret = zonefs_init_zgroups(sb);
	if (ret)
		goto cleanup;

	/* Create the root directory inode */
=======
	/* Create root directory inode */
>>>>>>> b7ba80a49124 (Commit)
	ret = -ENOMEM;
	inode = new_inode(sb);
	if (!inode)
		goto cleanup;

	inode->i_ino = bdev_nr_zones(sb->s_bdev);
	inode->i_mode = S_IFDIR | 0555;
	inode->i_ctime = inode->i_mtime = inode->i_atime = current_time(inode);
	inode->i_op = &zonefs_dir_inode_operations;
<<<<<<< HEAD
	inode->i_fop = &zonefs_dir_operations;
	inode->i_size = 2;
	set_nlink(inode, 2);
	for (ztype = 0; ztype < ZONEFS_ZTYPE_MAX; ztype++) {
		if (sbi->s_zgroup[ztype].g_nr_zones) {
			inc_nlink(inode);
			inode->i_size++;
		}
	}
=======
	inode->i_fop = &simple_dir_operations;
	set_nlink(inode, 2);
>>>>>>> b7ba80a49124 (Commit)

	sb->s_root = d_make_root(inode);
	if (!sb->s_root)
		goto cleanup;

<<<<<<< HEAD
	/*
	 * Take a reference on the zone groups directory inodes
	 * to keep them in the inode cache.
	 */
	ret = zonefs_get_zgroup_inodes(sb);
	if (ret)
		goto cleanup;

	ret = zonefs_sysfs_register(sb);
	if (ret)
		goto cleanup;

	return 0;

cleanup:
	zonefs_release_zgroup_inodes(sb);
	zonefs_free_zgroups(sb);
=======
	/* Create and populate files in zone groups directories */
	for (t = 0; t < ZONEFS_ZTYPE_MAX; t++) {
		ret = zonefs_create_zgroup(&zd, t);
		if (ret)
			break;
	}

cleanup:
	zonefs_cleanup_zone_info(&zd);
>>>>>>> b7ba80a49124 (Commit)

	return ret;
}

static struct dentry *zonefs_mount(struct file_system_type *fs_type,
				   int flags, const char *dev_name, void *data)
{
	return mount_bdev(fs_type, flags, dev_name, data, zonefs_fill_super);
}

static void zonefs_kill_super(struct super_block *sb)
{
	struct zonefs_sb_info *sbi = ZONEFS_SB(sb);

<<<<<<< HEAD
	/* Release the reference on the zone group directory inodes */
	zonefs_release_zgroup_inodes(sb);

	kill_block_super(sb);

	zonefs_sysfs_unregister(sb);
	zonefs_free_zgroups(sb);
=======
	if (sb->s_root)
		d_genocide(sb->s_root);

	zonefs_sysfs_unregister(sb);
	kill_block_super(sb);
>>>>>>> b7ba80a49124 (Commit)
	kfree(sbi);
}

/*
 * File system definition and registration.
 */
static struct file_system_type zonefs_type = {
	.owner		= THIS_MODULE,
	.name		= "zonefs",
	.mount		= zonefs_mount,
	.kill_sb	= zonefs_kill_super,
	.fs_flags	= FS_REQUIRES_DEV,
};

static int __init zonefs_init_inodecache(void)
{
	zonefs_inode_cachep = kmem_cache_create("zonefs_inode_cache",
			sizeof(struct zonefs_inode_info), 0,
			(SLAB_RECLAIM_ACCOUNT | SLAB_MEM_SPREAD | SLAB_ACCOUNT),
			NULL);
	if (zonefs_inode_cachep == NULL)
		return -ENOMEM;
	return 0;
}

static void zonefs_destroy_inodecache(void)
{
	/*
	 * Make sure all delayed rcu free inodes are flushed before we
	 * destroy the inode cache.
	 */
	rcu_barrier();
	kmem_cache_destroy(zonefs_inode_cachep);
}

static int __init zonefs_init(void)
{
	int ret;

	BUILD_BUG_ON(sizeof(struct zonefs_super) != ZONEFS_SUPER_SIZE);

	ret = zonefs_init_inodecache();
	if (ret)
		return ret;

<<<<<<< HEAD
	ret = zonefs_sysfs_init();
	if (ret)
		goto destroy_inodecache;

	ret = register_filesystem(&zonefs_type);
	if (ret)
		goto sysfs_exit;

	return 0;

sysfs_exit:
	zonefs_sysfs_exit();
=======
	ret = register_filesystem(&zonefs_type);
	if (ret)
		goto destroy_inodecache;

	ret = zonefs_sysfs_init();
	if (ret)
		goto unregister_fs;

	return 0;

unregister_fs:
	unregister_filesystem(&zonefs_type);
>>>>>>> b7ba80a49124 (Commit)
destroy_inodecache:
	zonefs_destroy_inodecache();

	return ret;
}

static void __exit zonefs_exit(void)
{
<<<<<<< HEAD
	unregister_filesystem(&zonefs_type);
	zonefs_sysfs_exit();
	zonefs_destroy_inodecache();
=======
	zonefs_sysfs_exit();
	zonefs_destroy_inodecache();
	unregister_filesystem(&zonefs_type);
>>>>>>> b7ba80a49124 (Commit)
}

MODULE_AUTHOR("Damien Le Moal");
MODULE_DESCRIPTION("Zone file system for zoned block devices");
MODULE_LICENSE("GPL");
MODULE_ALIAS_FS("zonefs");
module_init(zonefs_init);
module_exit(zonefs_exit);
