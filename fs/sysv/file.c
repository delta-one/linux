// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/fs/sysv/file.c
 *
 *  minix/file.c
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  coh/file.c
 *  Copyright (C) 1993  Pascal Haible, Bruno Haible
 *
 *  sysv/file.c
 *  Copyright (C) 1993  Bruno Haible
 *
 *  SystemV/Coherent regular file handling primitives
 */

#include "sysv.h"

/*
 * We have mostly NULLs here: the current defaults are OK for
 * the coh filesystem.
 */
const struct file_operations sysv_file_operations = {
	.llseek		= generic_file_llseek,
	.read_iter	= generic_file_read_iter,
	.write_iter	= generic_file_write_iter,
	.mmap		= generic_file_mmap,
	.fsync		= generic_file_fsync,
	.splice_read	= generic_file_splice_read,
};

<<<<<<< HEAD
static int sysv_setattr(struct mnt_idmap *idmap,
=======
static int sysv_setattr(struct user_namespace *mnt_userns,
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
		sysv_truncate(inode);
	}

<<<<<<< HEAD
	setattr_copy(&nop_mnt_idmap, inode, attr);
=======
	setattr_copy(&init_user_ns, inode, attr);
>>>>>>> b7ba80a49124 (Commit)
	mark_inode_dirty(inode);
	return 0;
}

const struct inode_operations sysv_file_inode_operations = {
	.setattr	= sysv_setattr,
	.getattr	= sysv_getattr,
};
