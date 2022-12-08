/* SPDX-License-Identifier: GPL-2.0 */
/*
<<<<<<< HEAD
 * Copyright (C) 2023 Patrick Franz <deltaone@debian.org>
=======
 * Copyright (C) 2021 Patrick Franz <deltaone@debian.org>
>>>>>>> b7ba80a49124 (Commit)
 */

#ifndef CF_RANGEFIX_H
#define CF_RANGEFIX_H

/* initialize RangeFix and return the diagnoses */
<<<<<<< HEAD
struct sfl_list *rangefix_run(PicoSAT *pico, struct cfdata *data);

/* ask user which fix to apply */
struct sfix_list *choose_fix(struct sfl_list *diag);
=======
struct sfl_list * rangefix_run(PicoSAT *pico);

/* ask user which fix to apply */
struct sfix_list * choose_fix(struct sfl_list *diag);
>>>>>>> b7ba80a49124 (Commit)

/* print a single diagnosis of type symbol_fix */
void print_diagnosis_symbol(struct sfix_list *diag_sym);

#endif
