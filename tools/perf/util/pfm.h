/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Support for libpfm4 event encoding.
 *
 * Copyright 2020 Google LLC.
 */
#ifndef __PERF_PFM_H
#define __PERF_PFM_H

<<<<<<< HEAD
#include "print-events.h"
=======
>>>>>>> b7ba80a49124 (Commit)
#include <subcmd/parse-options.h>

#ifdef HAVE_LIBPFM
int parse_libpfm_events_option(const struct option *opt, const char *str,
			int unset);

<<<<<<< HEAD
void print_libpfm_events(const struct print_callbacks *print_cb, void *print_state);
=======
void print_libpfm_events(bool name_only, bool long_desc);
>>>>>>> b7ba80a49124 (Commit)

#else
#include <linux/compiler.h>

static inline int parse_libpfm_events_option(
	const struct option *opt __maybe_unused,
	const char *str __maybe_unused,
	int unset __maybe_unused)
{
	return 0;
}

<<<<<<< HEAD
static inline void print_libpfm_events(const struct print_callbacks *print_cb __maybe_unused,
				       void *print_state __maybe_unused)
=======
static inline void print_libpfm_events(bool name_only __maybe_unused,
				       bool long_desc __maybe_unused)
>>>>>>> b7ba80a49124 (Commit)
{
}

#endif


#endif /* __PERF_PFM_H */
