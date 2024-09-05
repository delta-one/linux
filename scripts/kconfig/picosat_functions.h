#ifndef PICOSAT_FUNCTIONS_H_
#define PICOSAT_FUNCTIONS_H_

#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PICOSAT_UNKNOWN         0
#define PICOSAT_SATISFIABLE     10
#define PICOSAT_UNSATISFIABLE   20

typedef struct PicoSAT PicoSAT;

extern PicoSAT *(*picosat_init)(void);
extern int (*picosat_add)(PicoSAT *, int lit);
extern int (*picosat_deref)(PicoSAT *, int lit);
extern void (*picosat_assume)(PicoSAT *, int lit);
extern int (*picosat_sat)(PicoSAT *, int decision_limit);
extern const int *(*picosat_failed_assumptions)(PicoSAT *);
extern int (* picosat_added_original_clauses) (PicoSAT *);
extern int (* picosat_enable_trace_generation) (PicoSAT *);
extern void (* picosat_print)(PicoSAT *, FILE *);

bool load_picosat(void);

#ifdef __cplusplus
}
#endif

#endif // PICOSAT_FUNCTIONS_H_
