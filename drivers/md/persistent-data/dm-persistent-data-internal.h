<<<<<<< HEAD
/* SPDX-License-Identifier: GPL-2.0-only */
=======
>>>>>>> b7ba80a49124 (Commit)
/*
 * Copyright (C) 2011 Red Hat, Inc.
 *
 * This file is released under the GPL.
 */

#ifndef _DM_PERSISTENT_DATA_INTERNAL_H
#define _DM_PERSISTENT_DATA_INTERNAL_H

#include "dm-block-manager.h"

<<<<<<< HEAD
static inline unsigned int dm_hash_block(dm_block_t b, unsigned int hash_mask)
{
	const unsigned int BIG_PRIME = 4294967291UL;

	return (((unsigned int) b) * BIG_PRIME) & hash_mask;
=======
static inline unsigned dm_hash_block(dm_block_t b, unsigned hash_mask)
{
	const unsigned BIG_PRIME = 4294967291UL;

	return (((unsigned) b) * BIG_PRIME) & hash_mask;
>>>>>>> b7ba80a49124 (Commit)
}

#endif	/* _PERSISTENT_DATA_INTERNAL_H */
