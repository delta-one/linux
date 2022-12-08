/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __PERF_PRINT_EVENTS_H
#define __PERF_PRINT_EVENTS_H

<<<<<<< HEAD
#include <linux/perf_event.h>
=======
>>>>>>> b7ba80a49124 (Commit)
#include <stdbool.h>

struct event_symbol;

<<<<<<< HEAD
struct print_callbacks {
	void (*print_start)(void *print_state);
	void (*print_end)(void *print_state);
	void (*print_event)(void *print_state, const char *topic,
			const char *pmu_name,
			const char *event_name, const char *event_alias,
			const char *scale_unit,
			bool deprecated, const char *event_type_desc,
			const char *desc, const char *long_desc,
			const char *encoding_desc);
	void (*print_metric)(void *print_state,
			const char *group,
			const char *name,
			const char *desc,
			const char *long_desc,
			const char *expr,
			const char *threshold,
			const char *unit);
};

/** Print all events, the default when no options are specified. */
void print_events(const struct print_callbacks *print_cb, void *print_state);
int print_hwcache_events(const struct print_callbacks *print_cb, void *print_state);
void print_sdt_events(const struct print_callbacks *print_cb, void *print_state);
void print_symbol_events(const struct print_callbacks *print_cb, void *print_state,
			 unsigned int type, const struct event_symbol *syms,
			 unsigned int max);
void print_tool_events(const struct print_callbacks *print_cb, void *print_state);
void print_tracepoint_events(const struct print_callbacks *print_cb, void *print_state);
=======
void print_events(const char *event_glob, bool name_only, bool quiet_flag,
		  bool long_desc, bool details_flag, bool deprecated,
		  const char *pmu_name);
int print_hwcache_events(const char *event_glob, bool name_only);
void print_sdt_events(const char *subsys_glob, const char *event_glob,
		      bool name_only);
void print_symbol_events(const char *event_glob, unsigned int type,
			 struct event_symbol *syms, unsigned int max,
			 bool name_only);
void print_tool_events(const char *event_glob, bool name_only);
void print_tracepoint_events(const char *subsys_glob, const char *event_glob,
			     bool name_only);
>>>>>>> b7ba80a49124 (Commit)

#endif /* __PERF_PRINT_EVENTS_H */
