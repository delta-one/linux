// SPDX-License-Identifier: GPL-2.0

#include <dlfcn.h>
#include <unistd.h>

#include "array_size.h"

#include "cf_defs.h"
#include "picosat_functions.h"

const char *picosat_lib_names[] = { "libpicosat-trace.so",
				    "libpicosat-trace.so.0",
				    "libpicosat-trace.so.1" };

PicoSAT *(*picosat_init)(void);
int (*picosat_add)(PicoSAT *, int lit);
int (*picosat_deref)(PicoSAT *, int lit);
void (*picosat_assume)(PicoSAT *, int lit);
int (*picosat_sat)(PicoSAT *, int decision_limit);
const int *(*picosat_failed_assumptions)(PicoSAT *);
int (* picosat_added_original_clauses) (PicoSAT *);
int (* picosat_enable_trace_generation) (PicoSAT *);
void (* picosat_print)(PicoSAT *, FILE *);

#define PICOSAT_FUNCTION_LIST             \
	X(picosat_init)                   \
	X(picosat_add)                    \
	X(picosat_deref)                  \
	X(picosat_assume)                 \
	X(picosat_sat)                    \
	X(picosat_failed_assumptions)     \
	X(picosat_added_original_clauses) \
	X(picosat_enable_trace_generation)\
	X(picosat_print)

bool load_picosat(void)
{
	void *handle = NULL;

	/*
	 * Try different names for the .so library. This is necessary since
	 * all packages don't use the same versioning.
	 */
	for (int i = 0; i < ARRAY_SIZE(picosat_lib_names) && !handle; ++i) {
		handle = dlopen(picosat_lib_names[i], RTLD_LAZY);
	}
	if (!handle) {
		printd("%s\n", dlerror());
		return false;
	}

#define X(name)                                                     \
	name = dlsym(handle, #name);                                \
	if (!name) {                                                \
		printd("While loading " #name ": %s\n", dlerror()); \
		dlclose(handle);                                    \
		return false;                                       \
	}

	PICOSAT_FUNCTION_LIST
#undef LOAD_SYM
	return true;
}
