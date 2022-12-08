/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __PMU_H
#define __PMU_H

#include <linux/bitmap.h>
#include <linux/compiler.h>
#include <linux/perf_event.h>
#include <linux/list.h>
#include <stdbool.h>
<<<<<<< HEAD
#include <stdio.h>
=======
>>>>>>> b7ba80a49124 (Commit)
#include "parse-events.h"
#include "pmu-events/pmu-events.h"

struct evsel_config_term;
struct perf_cpu_map;
<<<<<<< HEAD
struct print_callbacks;
=======
>>>>>>> b7ba80a49124 (Commit)

enum {
	PERF_PMU_FORMAT_VALUE_CONFIG,
	PERF_PMU_FORMAT_VALUE_CONFIG1,
	PERF_PMU_FORMAT_VALUE_CONFIG2,
<<<<<<< HEAD
	PERF_PMU_FORMAT_VALUE_CONFIG_END,
};

#define PERF_PMU_FORMAT_BITS 64
=======
};

#define PERF_PMU_FORMAT_BITS 64
#define EVENT_SOURCE_DEVICE_PATH "/bus/event_source/devices/"
#define CPUS_TEMPLATE_CPU	"%s/bus/event_source/devices/%s/cpus"
>>>>>>> b7ba80a49124 (Commit)
#define MAX_PMU_NAME_LEN 128

struct perf_event_attr;

struct perf_pmu_caps {
	char *name;
	char *value;
	struct list_head list;
};

<<<<<<< HEAD
/**
 * struct perf_pmu - hi
 */
struct perf_pmu {
	/** @name: The name of the PMU such as "cpu". */
	char *name;
	/**
	 * @alias_name: Optional alternate name for the PMU determined in
	 * architecture specific code.
	 */
	char *alias_name;
	/**
	 * @id: Optional PMU identifier read from
	 * <sysfs>/bus/event_source/devices/<name>/identifier.
	 */
	char *id;
	/**
	 * @type: Perf event attributed type value, read from
	 * <sysfs>/bus/event_source/devices/<name>/type.
	 */
	__u32 type;
	/**
	 * @selectable: Can the PMU name be selected as if it were an event?
	 */
	bool selectable;
	/**
	 * @is_uncore: Is the PMU not within the CPU core? Determined by the
	 * presence of <sysfs>/bus/event_source/devices/<name>/cpumask.
	 */
	bool is_uncore;
	/**
	 * @auxtrace: Are events auxiliary events? Determined in architecture
	 * specific code.
	 */
	bool auxtrace;
	/**
	 * @max_precise: Number of levels of :ppp precision supported by the
	 * PMU, read from
	 * <sysfs>/bus/event_source/devices/<name>/caps/max_precise.
	 */
	int max_precise;
	/**
	 * @default_config: Optional default perf_event_attr determined in
	 * architecture specific code.
	 */
	struct perf_event_attr *default_config;
	/**
	 * @cpus: Empty or the contents of either of:
	 * <sysfs>/bus/event_source/devices/<name>/cpumask.
	 * <sysfs>/bus/event_source/devices/<cpu>/cpus.
	 */
	struct perf_cpu_map *cpus;
	/**
	 * @format: Holds the contents of files read from
	 * <sysfs>/bus/event_source/devices/<name>/format/. The contents specify
	 * which event parameter changes what config, config1 or config2 bits.
	 */
	struct list_head format;
	/**
	 * @aliases: List of struct perf_pmu_alias. Each alias corresponds to an
	 * event read from <sysfs>/bus/event_source/devices/<name>/events/ or
	 * from json events in pmu-events.c.
	 */
	struct list_head aliases;
	/** @caps_initialized: Has the list caps been initialized? */
	bool caps_initialized;
	/** @nr_caps: The length of the list caps. */
	u32 nr_caps;
	/**
	 * @caps: Holds the contents of files read from
	 * <sysfs>/bus/event_source/devices/<name>/caps/.
	 *
	 * The contents are pairs of the filename with the value of its
	 * contents, for example, max_precise (see above) may have a value of 3.
	 */
	struct list_head caps;
	/** @list: Element on pmus list in pmu.c. */
	struct list_head list;
	/** @hybrid_list: Element on perf_pmu__hybrid_pmus. */
	struct list_head hybrid_list;

	/**
	 * @missing_features: Features to inhibit when events on this PMU are
	 * opened.
	 */
	struct {
		/**
		 * @exclude_guest: Disables perf_event_attr exclude_guest and
		 * exclude_host.
		 */
=======
struct perf_pmu {
	char *name;
	char *alias_name;
	char *id;
	__u32 type;
	bool selectable;
	bool is_uncore;
	bool is_hybrid;
	bool auxtrace;
	int max_precise;
	struct perf_event_attr *default_config;
	struct perf_cpu_map *cpus;
	struct list_head format;  /* HEAD struct perf_pmu_format -> list */
	struct list_head aliases; /* HEAD struct perf_pmu_alias -> list */
	bool caps_initialized;
	u32 nr_caps;
	struct list_head caps;    /* HEAD struct perf_pmu_caps -> list */
	struct list_head list;    /* ELEM */
	struct list_head hybrid_list;

	struct {
>>>>>>> b7ba80a49124 (Commit)
		bool exclude_guest;
	} missing_features;
};

<<<<<<< HEAD
/** @perf_pmu__fake: A special global PMU used for testing. */
=======
>>>>>>> b7ba80a49124 (Commit)
extern struct perf_pmu perf_pmu__fake;

struct perf_pmu_info {
	const char *unit;
<<<<<<< HEAD
=======
	const char *metric_expr;
	const char *metric_name;
>>>>>>> b7ba80a49124 (Commit)
	double scale;
	bool per_pkg;
	bool snapshot;
};

#define UNIT_MAX_LEN	31 /* max length for event unit name */

<<<<<<< HEAD
/**
 * struct perf_pmu_alias - An event either read from sysfs or builtin in
 * pmu-events.c, created by parsing the pmu-events json files.
 */
struct perf_pmu_alias {
	/** @name: Name of the event like "mem-loads". */
	char *name;
	/** @desc: Optional short description of the event. */
	char *desc;
	/** @long_desc: Optional long description. */
	char *long_desc;
	/**
	 * @topic: Optional topic such as cache or pipeline, particularly for
	 * json events.
	 */
	char *topic;
	/**
	 * @str: Comma separated parameter list like
	 * "event=0xcd,umask=0x1,ldlat=0x3".
	 */
	char *str;
	/** @terms: Owned list of the original parsed parameters. */
	struct list_head terms;
	/** @list: List element of struct perf_pmu aliases. */
	struct list_head list;
	/** @unit: Units for the event, such as bytes or cache lines. */
	char unit[UNIT_MAX_LEN+1];
	/** @scale: Value to scale read counter values by. */
	double scale;
	/**
	 * @per_pkg: Does the file
	 * <sysfs>/bus/event_source/devices/<pmu_name>/events/<name>.per-pkg or
	 * equivalent json value exist and have the value 1.
	 */
	bool per_pkg;
	/**
	 * @snapshot: Does the file
	 * <sysfs>/bus/event_source/devices/<pmu_name>/events/<name>.snapshot
	 * exist and have the value 1.
	 */
	bool snapshot;
	/**
	 * @deprecated: Is the event hidden and so not shown in perf list by
	 * default.
	 */
	bool deprecated;
	/** @pmu_name: The name copied from struct perf_pmu. */
=======
struct perf_pmu_alias {
	char *name;
	char *desc;
	char *long_desc;
	char *topic;
	char *str;
	struct list_head terms; /* HEAD struct parse_events_term -> list */
	struct list_head list;  /* ELEM */
	char unit[UNIT_MAX_LEN+1];
	double scale;
	bool per_pkg;
	bool snapshot;
	bool deprecated;
	char *metric_expr;
	char *metric_name;
>>>>>>> b7ba80a49124 (Commit)
	char *pmu_name;
};

struct perf_pmu *perf_pmu__find(const char *name);
struct perf_pmu *perf_pmu__find_by_type(unsigned int type);
void pmu_add_sys_aliases(struct list_head *head, struct perf_pmu *pmu);
int perf_pmu__config(struct perf_pmu *pmu, struct perf_event_attr *attr,
		     struct list_head *head_terms,
		     struct parse_events_error *error);
int perf_pmu__config_terms(const char *pmu_name, struct list_head *formats,
			   struct perf_event_attr *attr,
			   struct list_head *head_terms,
			   bool zero, struct parse_events_error *error);
__u64 perf_pmu__format_bits(struct list_head *formats, const char *name);
int perf_pmu__format_type(struct list_head *formats, const char *name);
int perf_pmu__check_alias(struct perf_pmu *pmu, struct list_head *head_terms,
			  struct perf_pmu_info *info);
struct list_head *perf_pmu__alias(struct perf_pmu *pmu,
				  struct list_head *head_terms);
void perf_pmu_error(struct list_head *list, char *name, char const *msg);

int perf_pmu__new_format(struct list_head *list, char *name,
			 int config, unsigned long *bits);
void perf_pmu__set_format(unsigned long *bits, long from, long to);
int perf_pmu__format_parse(char *dir, struct list_head *head);
void perf_pmu__del_formats(struct list_head *formats);

struct perf_pmu *perf_pmu__scan(struct perf_pmu *pmu);

bool is_pmu_core(const char *name);
<<<<<<< HEAD
void print_pmu_events(const struct print_callbacks *print_cb, void *print_state);
=======
void print_pmu_events(const char *event_glob, bool name_only, bool quiet,
		      bool long_desc, bool details_flag,
		      bool deprecated, const char *pmu_name);
>>>>>>> b7ba80a49124 (Commit)
bool pmu_have_event(const char *pname, const char *name);

int perf_pmu__scan_file(struct perf_pmu *pmu, const char *name, const char *fmt, ...) __scanf(3, 4);

<<<<<<< HEAD
bool perf_pmu__file_exists(struct perf_pmu *pmu, const char *name);

=======
>>>>>>> b7ba80a49124 (Commit)
int perf_pmu__test(void);

struct perf_event_attr *perf_pmu__get_default_config(struct perf_pmu *pmu);
void pmu_add_cpu_aliases_table(struct list_head *head, struct perf_pmu *pmu,
			       const struct pmu_events_table *table);

char *perf_pmu__getcpuid(struct perf_pmu *pmu);
const struct pmu_events_table *pmu_events_table__find(void);
<<<<<<< HEAD
const struct pmu_metrics_table *pmu_metrics_table__find(void);
=======
>>>>>>> b7ba80a49124 (Commit)
bool pmu_uncore_alias_match(const char *pmu_name, const char *name);
void perf_pmu_free_alias(struct perf_pmu_alias *alias);

int perf_pmu__convert_scale(const char *scale, char **end, double *sval);

int perf_pmu__caps_parse(struct perf_pmu *pmu);

void perf_pmu__warn_invalid_config(struct perf_pmu *pmu, __u64 config,
				   const char *name);
<<<<<<< HEAD
void perf_pmu__warn_invalid_formats(struct perf_pmu *pmu);
=======
>>>>>>> b7ba80a49124 (Commit)

bool perf_pmu__has_hybrid(void);
int perf_pmu__match(char *pattern, char *name, char *tok);

int perf_pmu__cpus_match(struct perf_pmu *pmu, struct perf_cpu_map *cpus,
			 struct perf_cpu_map **mcpus_ptr,
			 struct perf_cpu_map **ucpus_ptr);

char *pmu_find_real_name(const char *name);
char *pmu_find_alias_name(const char *name);
<<<<<<< HEAD
double perf_pmu__cpu_slots_per_cycle(void);
int perf_pmu__event_source_devices_scnprintf(char *pathname, size_t size);
int perf_pmu__pathname_scnprintf(char *buf, size_t size,
				 const char *pmu_name, const char *filename);
FILE *perf_pmu__open_file(struct perf_pmu *pmu, const char *name);

=======
>>>>>>> b7ba80a49124 (Commit)
#endif /* __PMU_H */
