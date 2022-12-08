// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2013
 * Phillip Lougher <phillip@squashfs.org.uk>
 */

#include <linux/types.h>
#include <linux/slab.h>
#include <linux/percpu.h>
#include <linux/buffer_head.h>
#include <linux/local_lock.h>

#include "squashfs_fs.h"
#include "squashfs_fs_sb.h"
#include "decompressor.h"
#include "squashfs.h"

/*
 * This file implements multi-threaded decompression using percpu
 * variables, one thread per cpu core.
 */

struct squashfs_stream {
	void			*stream;
	local_lock_t	lock;
};

<<<<<<< HEAD
static void *squashfs_decompressor_create(struct squashfs_sb_info *msblk,
=======
void *squashfs_decompressor_create(struct squashfs_sb_info *msblk,
>>>>>>> b7ba80a49124 (Commit)
						void *comp_opts)
{
	struct squashfs_stream *stream;
	struct squashfs_stream __percpu *percpu;
	int err, cpu;

	percpu = alloc_percpu(struct squashfs_stream);
	if (percpu == NULL)
		return ERR_PTR(-ENOMEM);

	for_each_possible_cpu(cpu) {
		stream = per_cpu_ptr(percpu, cpu);
		stream->stream = msblk->decompressor->init(msblk, comp_opts);
		if (IS_ERR(stream->stream)) {
			err = PTR_ERR(stream->stream);
			goto out;
		}
		local_lock_init(&stream->lock);
	}

	kfree(comp_opts);
	return (__force void *) percpu;

out:
	for_each_possible_cpu(cpu) {
		stream = per_cpu_ptr(percpu, cpu);
		if (!IS_ERR_OR_NULL(stream->stream))
			msblk->decompressor->free(stream->stream);
	}
	free_percpu(percpu);
	return ERR_PTR(err);
}

<<<<<<< HEAD
static void squashfs_decompressor_destroy(struct squashfs_sb_info *msblk)
=======
void squashfs_decompressor_destroy(struct squashfs_sb_info *msblk)
>>>>>>> b7ba80a49124 (Commit)
{
	struct squashfs_stream __percpu *percpu =
			(struct squashfs_stream __percpu *) msblk->stream;
	struct squashfs_stream *stream;
	int cpu;

	if (msblk->stream) {
		for_each_possible_cpu(cpu) {
			stream = per_cpu_ptr(percpu, cpu);
			msblk->decompressor->free(stream->stream);
		}
		free_percpu(percpu);
	}
}

<<<<<<< HEAD
static int squashfs_decompress(struct squashfs_sb_info *msblk, struct bio *bio,
	int offset, int length, struct squashfs_page_actor *output)
{
	struct squashfs_stream *stream;
	struct squashfs_stream __percpu *percpu =
			(struct squashfs_stream __percpu *) msblk->stream;
	int res;

	local_lock(&percpu->lock);
	stream = this_cpu_ptr(percpu);
=======
int squashfs_decompress(struct squashfs_sb_info *msblk, struct bio *bio,
	int offset, int length, struct squashfs_page_actor *output)
{
	struct squashfs_stream *stream;
	int res;

	local_lock(&msblk->stream->lock);
	stream = this_cpu_ptr(msblk->stream);
>>>>>>> b7ba80a49124 (Commit)

	res = msblk->decompressor->decompress(msblk, stream->stream, bio,
					      offset, length, output);

<<<<<<< HEAD
	local_unlock(&percpu->lock);
=======
	local_unlock(&msblk->stream->lock);
>>>>>>> b7ba80a49124 (Commit)

	if (res < 0)
		ERROR("%s decompression failed, data probably corrupt\n",
			msblk->decompressor->name);

	return res;
}

<<<<<<< HEAD
static int squashfs_max_decompressors(void)
{
	return num_possible_cpus();
}

const struct squashfs_decompressor_thread_ops squashfs_decompressor_percpu = {
	.create = squashfs_decompressor_create,
	.destroy = squashfs_decompressor_destroy,
	.decompress = squashfs_decompress,
	.max_decompressors = squashfs_max_decompressors,
};
=======
int squashfs_max_decompressors(void)
{
	return num_possible_cpus();
}
>>>>>>> b7ba80a49124 (Commit)
