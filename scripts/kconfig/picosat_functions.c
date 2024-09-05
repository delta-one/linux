#include <dlfcn.h>
#include <unistd.h>

#include "cf_defs.h"
#include "picosat_functions.h"


const char *picosat_lib_name = "libpicosat-trace.so";

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
	void *handle = dlopen(picosat_lib_name, RTLD_LAZY);
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
