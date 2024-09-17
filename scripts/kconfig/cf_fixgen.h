/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2023 Patrick Franz <deltaone@debian.org>
 */

#ifndef CF_FIXGEN_H
#define CF_FIXGEN_H

#include "picosat_functions.h"
#include "cf_defs.h"

/* initialize fixgen and return the diagnoses */
struct sfl_list *fixgen_run(PicoSAT *pico, struct cfdata *data);

/* ask user which fix to apply */
struct sfix_list *ask_user_choose_fix(struct sfl_list *diag);

/* print a single diagnosis of type symbol_fix */
void print_diagnosis_symbol(struct sfix_list *diag_sym);

#endif
