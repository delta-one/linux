// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/fs/minix/file.c
 *
 *  Copyright (C) 1991, 1992 Linus Torvalds
 *
 *  minix regular file handling primitives
 */

#include "minix.h"

/*
 * We have mostly NULLs here: the current defaults are OK for
 * the minix filesystem.
 */
const struct file_operations minix_file_operations = {
	.llseek		= generic_file_llseek,
	.read_iter	= generic_file_read_iter,
	.write_iter	= generic_file_write_iter,
	.mmap		= generic_file_mmap,
	.fsync		= generic_file_fsync,
	.splice_read	= generic_file_splice_read,
};

<<<<<<< HEAD
static int minix_setattr(struct mnt_idmap *idmap,
=======
static int minix_setattr(struct user_namespace *mnt_userns,
>>>>>>> b7ba80a49124 (Commit)
			 struct dentry *dentry, struct iattr *attr)
{
	struct inode *inode = d_inode(dentry);
	int error;

<<<<<<< HEAD
	error = setattr_prepare(&nop_mnt_idmap, dentry, attr);
=======
	error = setattr_prepare(&init_user_ns, dentry, attr);
>>>>>>> b7ba80a49124 (Commit)
	if (error)
		return error;

	if ((attr->ia_valid & ATTR_SIZE) &&
	    attr->ia_size != i_size_read(inode)) {
		error = inode_newsize_ok(inode, attr->ia_size);
		if (error)
			return error;

		truncate_setsize(inode, attr->ia_size);
		minix_truncate(inode);
	}

<<<<<<< HEAD
	setattr_copy(&nop_mnt_idmap, inode, attr);
=======
	setattr_copy(&init_user_ns, inode, attr);
>>>>>>> b7ba80a49124 (Commit)
	mark_inode_dirty(inode);
	return 0;
}

const struct inode_operations minix_file_inode_operations = {
	.setattr	= minix_setattr,
	.getattr	= minix_getattr,
};
