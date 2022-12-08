/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Device Mapper Uevent Support
 *
 * Copyright IBM Corporation, 2007
<<<<<<< HEAD
 *	Author: Mike Anderson <andmike@linux.vnet.ibm.com>
=======
 * 	Author: Mike Anderson <andmike@linux.vnet.ibm.com>
>>>>>>> b7ba80a49124 (Commit)
 */
#ifndef DM_UEVENT_H
#define DM_UEVENT_H

enum dm_uevent_type {
	DM_UEVENT_PATH_FAILED,
	DM_UEVENT_PATH_REINSTATED,
};

#ifdef CONFIG_DM_UEVENT

extern int dm_uevent_init(void);
extern void dm_uevent_exit(void);
extern void dm_send_uevents(struct list_head *events, struct kobject *kobj);
extern void dm_path_uevent(enum dm_uevent_type event_type,
			   struct dm_target *ti, const char *path,
<<<<<<< HEAD
			   unsigned int nr_valid_paths);
=======
			   unsigned nr_valid_paths);
>>>>>>> b7ba80a49124 (Commit)

#else

static inline int dm_uevent_init(void)
{
	return 0;
}
static inline void dm_uevent_exit(void)
{
}
static inline void dm_send_uevents(struct list_head *events,
				   struct kobject *kobj)
{
}
static inline void dm_path_uevent(enum dm_uevent_type event_type,
				  struct dm_target *ti, const char *path,
<<<<<<< HEAD
				  unsigned int nr_valid_paths)
=======
				  unsigned nr_valid_paths)
>>>>>>> b7ba80a49124 (Commit)
{
}

#endif	/* CONFIG_DM_UEVENT */

#endif	/* DM_UEVENT_H */
