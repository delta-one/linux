<<<<<<< HEAD
/* SPDX-License-Identifier: GPL-2.0-only */
=======
>>>>>>> b7ba80a49124 (Commit)
/*
 * Copyright (C) 2004 Red Hat, Inc. All rights reserved.
 *
 * This file is released under the GPL.
 *
 * Multipath.
 */

#ifndef	DM_MPATH_H
#define	DM_MPATH_H

struct dm_dev;

struct dm_path {
	struct dm_dev *dev;	/* Read-only */
	void *pscontext;	/* For path-selector use */
};

/* Callback for hwh_pg_init_fn to use when complete */
<<<<<<< HEAD
void dm_pg_init_complete(struct dm_path *path, unsigned int err_flags);
=======
void dm_pg_init_complete(struct dm_path *path, unsigned err_flags);
>>>>>>> b7ba80a49124 (Commit)

#endif
