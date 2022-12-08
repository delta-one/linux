/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2019 IBM Corporation
 * Author: Nayna Jain
 *
 * PowerPC secure variable operations.
 */
#ifndef SECVAR_OPS_H
#define SECVAR_OPS_H

#include <linux/types.h>
#include <linux/errno.h>
<<<<<<< HEAD
#include <linux/sysfs.h>
=======
>>>>>>> b7ba80a49124 (Commit)

extern const struct secvar_operations *secvar_ops;

struct secvar_operations {
<<<<<<< HEAD
	int (*get)(const char *key, u64 key_len, u8 *data, u64 *data_size);
	int (*get_next)(const char *key, u64 *key_len, u64 keybufsize);
	int (*set)(const char *key, u64 key_len, u8 *data, u64 data_size);
	ssize_t (*format)(char *buf, size_t bufsize);
	int (*max_size)(u64 *max_size);
	const struct attribute **config_attrs;

	// NULL-terminated array of fixed variable names
	// Only used if get_next() isn't provided
	const char * const *var_names;
=======
	int (*get)(const char *key, uint64_t key_len, u8 *data,
		   uint64_t *data_size);
	int (*get_next)(const char *key, uint64_t *key_len,
			uint64_t keybufsize);
	int (*set)(const char *key, uint64_t key_len, u8 *data,
		   uint64_t data_size);
>>>>>>> b7ba80a49124 (Commit)
};

#ifdef CONFIG_PPC_SECURE_BOOT

<<<<<<< HEAD
int set_secvar_ops(const struct secvar_operations *ops);

#else

static inline int set_secvar_ops(const struct secvar_operations *ops) { return 0; }
=======
extern void set_secvar_ops(const struct secvar_operations *ops);

#else

static inline void set_secvar_ops(const struct secvar_operations *ops) { }
>>>>>>> b7ba80a49124 (Commit)

#endif

#endif
