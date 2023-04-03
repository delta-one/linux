/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __PROCFS_FD_H__
#define __PROCFS_FD_H__

#include <linux/fs.h>

extern const struct file_operations proc_fd_operations;
extern const struct inode_operations proc_fd_inode_operations;

extern const struct file_operations proc_fdinfo_operations;
extern const struct inode_operations proc_fdinfo_inode_operations;

<<<<<<< HEAD
extern int proc_fd_permission(struct mnt_idmap *idmap,
=======
extern int proc_fd_permission(struct user_namespace *mnt_userns,
>>>>>>> b7ba80a49124 (Commit)
			      struct inode *inode, int mask);

static inline unsigned int proc_fd(struct inode *inode)
{
	return PROC_I(inode)->fd;
}

#endif /* __PROCFS_FD_H__ */
