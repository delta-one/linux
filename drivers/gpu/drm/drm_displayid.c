// SPDX-License-Identifier: MIT
/*
 * Copyright © 2021 Intel Corporation
 */

#include <drm/drm_displayid.h>
#include <drm/drm_edid.h>
#include <drm/drm_print.h>

<<<<<<< HEAD
static const struct displayid_header *
displayid_get_header(const u8 *displayid, int length, int index)
{
	const struct displayid_header *base;

	if (sizeof(*base) > length - index)
		return ERR_PTR(-EINVAL);

	base = (const struct displayid_header *)&displayid[index];

	return base;
}

static const struct displayid_header *
validate_displayid(const u8 *displayid, int length, int idx)
=======
static int validate_displayid(const u8 *displayid, int length, int idx)
>>>>>>> b7ba80a49124 (Commit)
{
	int i, dispid_length;
	u8 csum = 0;
	const struct displayid_header *base;

<<<<<<< HEAD
	base = displayid_get_header(displayid, length, idx);
	if (IS_ERR(base))
		return base;
=======
	base = (const struct displayid_header *)&displayid[idx];
>>>>>>> b7ba80a49124 (Commit)

	DRM_DEBUG_KMS("base revision 0x%x, length %d, %d %d\n",
		      base->rev, base->bytes, base->prod_id, base->ext_count);

	/* +1 for DispID checksum */
	dispid_length = sizeof(*base) + base->bytes + 1;
	if (dispid_length > length - idx)
<<<<<<< HEAD
		return ERR_PTR(-EINVAL);
=======
		return -EINVAL;
>>>>>>> b7ba80a49124 (Commit)

	for (i = 0; i < dispid_length; i++)
		csum += displayid[idx + i];
	if (csum) {
		DRM_NOTE("DisplayID checksum invalid, remainder is %d\n", csum);
<<<<<<< HEAD
		return ERR_PTR(-EINVAL);
	}

	return base;
=======
		return -EINVAL;
	}

	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

static const u8 *drm_find_displayid_extension(const struct drm_edid *drm_edid,
					      int *length, int *idx,
					      int *ext_index)
{
	const u8 *displayid = drm_find_edid_extension(drm_edid, DISPLAYID_EXT, ext_index);
	const struct displayid_header *base;
<<<<<<< HEAD
=======
	int ret;
>>>>>>> b7ba80a49124 (Commit)

	if (!displayid)
		return NULL;

	/* EDID extensions block checksum isn't for us */
	*length = EDID_LENGTH - 1;
	*idx = 1;

<<<<<<< HEAD
	base = validate_displayid(displayid, *length, *idx);
	if (IS_ERR(base))
		return NULL;

=======
	ret = validate_displayid(displayid, *length, *idx);
	if (ret)
		return NULL;

	base = (const struct displayid_header *)&displayid[*idx];
>>>>>>> b7ba80a49124 (Commit)
	*length = *idx + sizeof(*base) + base->bytes;

	return displayid;
}

void displayid_iter_edid_begin(const struct drm_edid *drm_edid,
			       struct displayid_iter *iter)
{
	memset(iter, 0, sizeof(*iter));

	iter->drm_edid = drm_edid;
}

static const struct displayid_block *
displayid_iter_block(const struct displayid_iter *iter)
{
	const struct displayid_block *block;

	if (!iter->section)
		return NULL;

	block = (const struct displayid_block *)&iter->section[iter->idx];

	if (iter->idx + sizeof(*block) <= iter->length &&
	    iter->idx + sizeof(*block) + block->num_bytes <= iter->length)
		return block;

	return NULL;
}

const struct displayid_block *
__displayid_iter_next(struct displayid_iter *iter)
{
	const struct displayid_block *block;

	if (!iter->drm_edid)
		return NULL;

	if (iter->section) {
		/* current block should always be valid */
		block = displayid_iter_block(iter);
		if (WARN_ON(!block)) {
			iter->section = NULL;
			iter->drm_edid = NULL;
			return NULL;
		}

		/* next block in section */
		iter->idx += sizeof(*block) + block->num_bytes;

		block = displayid_iter_block(iter);
		if (block)
			return block;
	}

	for (;;) {
<<<<<<< HEAD
		/* The first section we encounter is the base section */
		bool base_section = !iter->section;

=======
>>>>>>> b7ba80a49124 (Commit)
		iter->section = drm_find_displayid_extension(iter->drm_edid,
							     &iter->length,
							     &iter->idx,
							     &iter->ext_index);
		if (!iter->section) {
			iter->drm_edid = NULL;
			return NULL;
		}

<<<<<<< HEAD
		/* Save the structure version and primary use case. */
		if (base_section) {
			const struct displayid_header *base;

			base = displayid_get_header(iter->section, iter->length,
						    iter->idx);
			if (!IS_ERR(base)) {
				iter->version = base->rev;
				iter->primary_use = base->prod_id;
			}
		}

=======
>>>>>>> b7ba80a49124 (Commit)
		iter->idx += sizeof(struct displayid_header);

		block = displayid_iter_block(iter);
		if (block)
			return block;
	}
}

void displayid_iter_end(struct displayid_iter *iter)
{
	memset(iter, 0, sizeof(*iter));
}
<<<<<<< HEAD

/* DisplayID Structure Version/Revision from the Base Section. */
u8 displayid_version(const struct displayid_iter *iter)
{
	return iter->version;
}

/*
 * DisplayID Primary Use Case (2.0+) or Product Type Identifier (1.0-1.3) from
 * the Base Section.
 */
u8 displayid_primary_use(const struct displayid_iter *iter)
{
	return iter->primary_use;
}
=======
>>>>>>> b7ba80a49124 (Commit)
