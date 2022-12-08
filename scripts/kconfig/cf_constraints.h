/* SPDX-License-Identifier: GPL-2.0 */
/*
<<<<<<< HEAD
 * Copyright (C) 2023 Patrick Franz <deltaone@debian.org>
=======
 * Copyright (C) 2021 Patrick Franz <deltaone@debian.org>
>>>>>>> b7ba80a49124 (Commit)
 */

#ifndef CF_CONSTRAINTS_H
#define CF_CONSTRAINTS_H

/* build the constraints for each symbol */
<<<<<<< HEAD
void get_constraints(struct cfdata *data);
=======
void get_constraints(void);
>>>>>>> b7ba80a49124 (Commit)

/* count the number of all constraints */
unsigned int count_counstraints(void);

/* add a constraint for a symbol */
<<<<<<< HEAD
void sym_add_constraint(struct symbol *sym, struct pexpr *constraint, struct cfdata *data);
=======
void sym_add_constraint(struct symbol *sym, struct pexpr *constraint);
>>>>>>> b7ba80a49124 (Commit)

void sym_add_constraint_fexpr(struct symbol *sym, struct fexpr *constraint);

/* add a constraint for a symbol, but check for duplicate constraints */
<<<<<<< HEAD
void sym_add_constraint_eq(struct symbol *sym, struct pexpr *constraint, struct cfdata *data);
=======
void sym_add_constraint_eq(struct symbol *sym, struct pexpr *constraint);
>>>>>>> b7ba80a49124 (Commit)

#endif
