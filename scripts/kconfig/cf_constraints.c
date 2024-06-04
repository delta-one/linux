// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Patrick Franz <deltaone@debian.org>
 */

#define _GNU_SOURCE
#include <assert.h>
#include <locale.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "cf_utils.h"
#include "internal.h"
#include "cf_expr.h"
#include "cf_constraints.h"

#define KCR_CMP false
#define NPC_OPTIMISATION true

static void init_constraints(struct cfdata *data);
static void get_constraints_bool(struct cfdata *data);
static void get_constraints_select(struct cfdata *data);
static void get_constraints_nonbool(struct cfdata *data);

static void build_tristate_constraint_clause(struct symbol *sym,
					     struct cfdata *data);

static void add_selects_kcr(struct symbol *sym, struct cfdata *data);
static void add_selects(struct symbol *sym, struct cfdata *data);

static void add_dependencies_bool(struct symbol *sym, struct cfdata *data);
static void add_dependencies_bool_kcr(struct symbol *sym, struct cfdata *data);
static void add_dependencies_nonbool(struct symbol *sym, struct cfdata *data);

static void add_choice_prompt_cond(struct symbol *sym, struct cfdata *data);
static void add_choice_dependencies(struct symbol *sym, struct cfdata *data);
static void add_choice_constraints(struct symbol *sym, struct cfdata *data);
static void add_invisible_constraints(struct symbol *sym, struct cfdata *data);
static void sym_nonbool_at_least_1(struct symbol *sym, struct cfdata *data);
static void sym_nonbool_at_most_1(struct symbol *sym, struct cfdata *data);
static void sym_add_nonbool_values_from_default_range(struct symbol *sym,
						      struct cfdata *data);
static void sym_add_range_constraints(struct symbol *sym, struct cfdata *data);
static void sym_add_nonbool_prompt_constraint(struct symbol *sym,
					      struct cfdata *data);

static struct default_map *create_default_map_entry(struct fexpr *val,
						    struct pexpr *e);
static struct defm_list *get_defaults(struct symbol *sym, struct cfdata *data);
static struct pexpr *get_default_y(struct defm_list *list, struct cfdata *data);
static struct pexpr *get_default_m(struct defm_list *list, struct cfdata *data);
static struct pexpr *get_default_any(struct symbol *sym, struct cfdata *data);
static long sym_get_range_val(struct symbol *sym, int base);

/* -------------------------------------- */

/*
 * build the constraints for each symbol
 */
void get_constraints(struct cfdata *data)
{
	printd("Building constraints...");

	init_constraints(data);
	get_constraints_bool(data);
	get_constraints_select(data);
	get_constraints_nonbool(data);
}

/*
 * need to go through the constraints once to find all "known values"
 * for the non-Boolean symbols (and add them to sym->nb_vals for the given
 * symbols).
 * expr_calculate_pexpr_both and get_defaults have the side effect of creating
 * known values.
 */
static void init_constraints(struct cfdata *data)
{
	struct symbol *sym;
	struct property *p;

	for_all_symbols(sym) {
		struct property *prompt;

		if (sym->type == S_UNKNOWN)
			continue;

		if (sym_is_boolean(sym)) {
			for_all_properties(sym, p, P_SELECT)
				expr_calculate_pexpr_both(p->visible.expr,
							  data);

			for_all_properties(sym, p, P_IMPLY)
				expr_calculate_pexpr_both(p->visible.expr,
							  data);
		}

		if (sym->dir_dep.expr)
			expr_calculate_pexpr_both(sym->dir_dep.expr, data);

		prompt = sym_get_prompt(sym);
		if (prompt != NULL && prompt->visible.expr) {
			expr_calculate_pexpr_both(prompt->visible.expr, data);
			get_defaults(sym, data);
		}

		if (sym_is_nonboolean(sym)) {
			const char *curr;

			for_all_defaults(sym, p) {
				if (p == NULL)
					continue;

				sym_create_nonbool_fexpr(
					sym, p->expr->left.sym->name, data);
			}
			for_all_properties(sym, p, P_RANGE) {
				if (p == NULL)
					continue;

				sym_create_nonbool_fexpr(
					sym, p->expr->left.sym->name, data);
				sym_create_nonbool_fexpr(
					sym, p->expr->right.sym->name, data);
			}
			curr = sym_get_string_value(sym);
			if (strcmp(curr, "") != 0)
				sym_create_nonbool_fexpr(sym, (char *)curr,
							 data);
		}

		if (sym->type == S_HEX || sym->type == S_INT)
			sym_add_nonbool_values_from_default_range(sym, data);
	}
}

/*
 *  build constraints for boolean symbols
 */
static void get_constraints_bool(struct cfdata *data)
{
	struct symbol *sym;

	for_all_symbols(sym) {
		if (!sym_is_boolean(sym))
			continue;

		/* build tristate constraints */
		if (sym->type == S_TRISTATE)
			build_tristate_constraint_clause(sym, data);

		/* build constraints for select statements
		 * need to treat choice symbols seperately
		 */
		if (!KCR_CMP) {
			add_selects(sym, data);
		} else {
			if (sym->rev_dep.expr && !sym_is_choice(sym) &&
			    !sym_is_choice_value(sym))
				add_selects_kcr(sym, data);
		}

		/* build constraints for dependencies for booleans */
		if (sym->dir_dep.expr && !sym_is_choice(sym) &&
		    !sym_is_choice_value(sym)) {
			if (!KCR_CMP)
				add_dependencies_bool(sym, data);
			else
				add_dependencies_bool_kcr(sym, data);
		}

		/* build constraints for choice prompts */
		if (sym_is_choice(sym))
			add_choice_prompt_cond(sym, data);

		/* build constraints for dependencies (choice symbols and options) */
		if (sym_is_choice(sym) || sym_is_choice_value(sym))
			add_choice_dependencies(sym, data);

		/* build constraints for the choice groups */
		if (sym_is_choice(sym))
			add_choice_constraints(sym, data);

		/* build invisible constraints */
		add_invisible_constraints(sym, data);
	}
}

/*
 * build the constraints for select-variables
 * skip non-Booleans, choice symbols/options och symbols without rev_dir
 */
static void get_constraints_select(struct cfdata *data)
{
	struct symbol *sym;

	for_all_symbols(sym) {
		struct pexpr *sel_y, *sel_m;
		struct pexpr *c1, *c2;

		if (KCR_CMP)
			continue;

		if (!sym_is_boolean(sym))
			continue;

		if (sym_is_choice(sym) || sym_is_choice_value(sym))
			continue;

		if (!sym->rev_dep.expr)
			continue;

		if (sym->list_sel_y == NULL)
			continue;

		sel_y = pexpr_implies(pexf(sym->fexpr_sel_y),
					   pexf(sym->fexpr_y), data,
					   PEXPR_ARGX);
		sym_add_constraint(sym, sel_y, data);

		c1 = pexpr_implies(pexf(sym->fexpr_sel_y), sym->list_sel_y,
					data, PEXPR_ARG1);
		sym_add_constraint(sym, c1, data);

		/* only continue for tristates */
		if (sym->type == S_BOOLEAN)
			continue;

		sel_m = pexpr_implies(pexf(sym->fexpr_sel_m),
					   sym_get_fexpr_both(sym, data), data,
					   PEXPR_ARGX);
		sym_add_constraint(sym, sel_m, data);

		c2 = pexpr_implies(pexf(sym->fexpr_sel_m), sym->list_sel_m,
					data, PEXPR_ARG1);
		sym_add_constraint(sym, c2, data);
	}
}

/*
 * build constraints for non-booleans
 */
static void get_constraints_nonbool(struct cfdata *data)
{
	struct symbol *sym;

	for_all_symbols(sym) {
		if (!sym_is_nonboolean(sym))
			continue;

		/* the symbol must have a value, if there is a prompt */
		if (sym_has_prompt(sym))
			sym_add_nonbool_prompt_constraint(sym, data);

		/* build the range constraints for int/hex */
		if (sym->type == S_HEX || sym->type == S_INT)
			sym_add_range_constraints(sym, data);

		/* build constraints for dependencies for non-booleans */
		if (sym->dir_dep.expr)
			add_dependencies_nonbool(sym, data);

		/* build invisible constraints */
		add_invisible_constraints(sym, data);

		/* exactly one of the symbols must be true */
		sym_nonbool_at_least_1(sym, data);
		sym_nonbool_at_most_1(sym, data);
	}
}

/*
 * enforce tristate constraints
 */
static void build_tristate_constraint_clause(struct symbol *sym,
					     struct cfdata *data)
{
	struct pexpr *X, *X_m, *modules, *c;

	if (sym->type != S_TRISTATE)
		return;

	X = pexf(sym->fexpr_y);
	X_m = pexf(sym->fexpr_m);
	modules = pexf(modules_sym->fexpr_y);

	/* -X v -X_m */
	c = pexpr_or(pexpr_not_share(X, data), pexpr_not_share(X_m, data),
			  data, PEXPR_ARGX);
	sym_add_constraint(sym, c, data);

	/* X_m -> MODULES */
	if (modules_sym->fexpr_y != NULL) {
		struct pexpr *c2 = pexpr_implies_share(X_m, modules, data);

		sym_add_constraint(sym, c2, data);
	}
	PEXPR_PUT(X, X_m, modules);
}

/*
 * build the select constraints
 * - RDep(X) implies X
 */
static void add_selects_kcr(struct symbol *sym, struct cfdata *data)
{
	struct pexpr *rdep_y = expr_calculate_pexpr_y(sym->rev_dep.expr, data);
	struct pexpr *c1 = pexpr_implies(rdep_y, pexf(sym->fexpr_y), data,
					      PEXPR_ARGX);

	struct pexpr *rdep_both =
		expr_calculate_pexpr_both(sym->rev_dep.expr, data);
	struct pexpr *c2 = pexpr_implies(
		rdep_both, sym_get_fexpr_both(sym, data), data, PEXPR_ARGX);

	sym_add_constraint(sym, c1, data);
	sym_add_constraint(sym, c2, data);
}

/*
 * build the select constraints simplified
 * - RDep(X) implies X
 */
static void add_selects(struct symbol *sym, struct cfdata *data)
{
	struct property *p;

	if (!sym_is_boolean(sym))
		return;

	for_all_properties(sym, p, P_SELECT) {
		struct symbol *selected = p->expr->left.sym;
		struct pexpr *cond_y, *cond_both;

		if (selected->type == S_UNKNOWN)
			continue;

		if (!selected->rev_dep.expr)
			continue;

		if (p->visible.expr) {
			cond_y = expr_calculate_pexpr_y(p->visible.expr, data);
			cond_both = expr_calculate_pexpr_both(p->visible.expr,
							      data);
		} else {
			cond_y = pexf(data->constants->const_true);
			cond_both = pexf(data->constants->const_true);
		}

		if (selected->type == S_BOOLEAN) {
			/* imply that symbol is selected to y */
			struct pexpr *e1 = pexpr_and(
				cond_both, sym_get_fexpr_both(sym, data), data,
				PEXPR_ARG2);
			struct pexpr *c1 = pexpr_implies(
				e1, pexf(selected->fexpr_sel_y), data,
				PEXPR_ARG2);

			sym_add_constraint(selected, c1, data);

			if (selected->list_sel_y == NULL)
				selected->list_sel_y = e1;
			else
				selected->list_sel_y =
					pexpr_or(selected->list_sel_y, e1,
						      data, PEXPR_ARG2);
		}

		if (selected->type == S_TRISTATE) {
			struct pexpr *e2, *e3, *c2, *c3;

			/* imply that symbol is selected to y */
			e2 = pexpr_and(cond_y, pexf(sym->fexpr_y), data,
					    PEXPR_ARG2);
			c2 = pexpr_implies(e2, pexf(selected->fexpr_sel_y),
						data, PEXPR_ARG2);
			sym_add_constraint(selected, c2, data);

			if (selected->list_sel_y == NULL)
				selected->list_sel_y = e2;
			else
				selected->list_sel_y =
					pexpr_or(selected->list_sel_y, e2,
						      data, PEXPR_ARG2);

			/* imply that symbol is selected to m */
			e3 = pexpr_and(cond_both,
					    sym_get_fexpr_both(sym, data), data,
					    PEXPR_ARG2);
			c3 = pexpr_implies(e3, pexf(selected->fexpr_sel_m),
						data, PEXPR_ARG2);
			sym_add_constraint(selected, c3, data);

			if (selected->list_sel_m == NULL)
				selected->list_sel_m = e3;
			else
				selected->list_sel_m =
					pexpr_or(selected->list_sel_m, e3,
						      data, PEXPR_ARG2);
		}
		PEXPR_PUT(cond_y, cond_both);
	}
}

/*
 * build the dependency constraints for booleans
 *  - X implies Dep(X) or RDep(X)
 */
static void add_dependencies_bool(struct symbol *sym, struct cfdata *data)
{
	struct pexpr *dep_both;

	if (!sym_is_boolean(sym) || !sym->dir_dep.expr)
		return;

	dep_both = expr_calculate_pexpr_both(sym->dir_dep.expr, data);

	if (sym->type == S_TRISTATE) {
		struct pexpr *c1;
		struct pexpr *c2;
		{
			struct pexpr *dep_y =
				expr_calculate_pexpr_y(sym->dir_dep.expr, data);
			struct pexpr *sel_y =
				sym->rev_dep.expr ?
					pexf(sym->fexpr_sel_y) :
					pexf(data->constants->const_false);

			c1 = pexpr_implies(pexf(sym->fexpr_y),
						pexpr_or(dep_y, sel_y,
							      data, PEXPR_ARGX),
						data, PEXPR_ARGX);
		}
		c2 = pexpr_implies(
			pexf(sym->fexpr_m),
			pexpr_or(dep_both,
				      sym_get_fexpr_sel_both(sym, data), data,
				      PEXPR_ARG2),
			data, PEXPR_ARGX);

		sym_add_constraint(sym, c1, data);
		sym_add_constraint(sym, c2, data);
	} else if (sym->type == S_BOOLEAN) {
		struct pexpr *c = pexpr_implies(
			pexf(sym->fexpr_y),
			pexpr_or(dep_both,
				      sym_get_fexpr_sel_both(sym, data), data,
				      PEXPR_ARG2),
			data, PEXPR_ARGX);

		sym_add_constraint(sym, c, data);
	}
	pexpr_put(dep_both);
}

/*
 * build the dependency constraints for booleans (KCR)
 *  - X implies Dep(X) or RDep(X)
 */
static void add_dependencies_bool_kcr(struct symbol *sym, struct cfdata *data)
{
	struct pexpr *dep_both, *sel_both;

	if (!sym_is_boolean(sym) || !sym->dir_dep.expr)
		return;

	dep_both = expr_calculate_pexpr_both(sym->dir_dep.expr, data);

	sel_both = sym->rev_dep.expr ?
			   expr_calculate_pexpr_both(sym->rev_dep.expr, data) :
			   pexf(data->constants->const_false);

	if (sym->type == S_TRISTATE) {
		struct pexpr *c1;
		struct pexpr *c2;
		{
			struct pexpr *dep_y =
				expr_calculate_pexpr_y(sym->dir_dep.expr, data);
			struct pexpr *sel_y =
				sym->rev_dep.expr ?
					expr_calculate_pexpr_y(
						sym->rev_dep.expr, data) :
					pexf(data->constants->const_false);
			c1 = pexpr_implies(pexf(sym->fexpr_y),
						pexpr_or(dep_y, sel_y,
							      data, PEXPR_ARGX),
						data, PEXPR_ARGX);
		}
		c2 = pexpr_implies(pexf(sym->fexpr_m),
					pexpr_or_share(dep_both, sel_both,
						       data),
					data, PEXPR_ARGX);

		sym_add_constraint(sym, c1, data);
		sym_add_constraint(sym, c2, data);
	} else if (sym->type == S_BOOLEAN) {
		struct pexpr *c = pexpr_implies(
			pexf(sym->fexpr_y),
			pexpr_or_share(dep_both, sel_both, data), data,
			PEXPR_ARGX);

		sym_add_constraint(sym, c, data);
	}

	PEXPR_PUT(dep_both, sel_both);
}

/*
 * build the dependency constraints for non-booleans
 *
 * sym is not 'n' implies `sym->dir_dep`
 */
static void add_dependencies_nonbool(struct symbol *sym, struct cfdata *data)
{
	struct pexpr *dep_both;
	struct pexpr *nb_vals; // "sym is set to some value" / "sym is not 'n'"
	struct fexpr_node *node;
	struct pexpr *c;

	if (!sym_is_nonboolean(sym) || !sym->dir_dep.expr || sym->rev_dep.expr)
		return;

	dep_both = expr_calculate_pexpr_both(sym->dir_dep.expr, data);

	nb_vals = pexf(data->constants->const_false);
	/* can skip the first non-boolean value, since this is 'n' */
	fexpr_list_for_each(node, sym->nb_vals) {
		if (node->prev == NULL)
			continue;

		nb_vals = pexpr_or(nb_vals, pexf(node->elem), data,
					PEXPR_ARGX);
	}

	c = pexpr_implies(nb_vals, dep_both, data, PEXPR_ARGX);
	sym_add_constraint(sym, c, data);
}

/*
 * build the constraints for the choice prompt
 */
static void add_choice_prompt_cond(struct symbol *sym, struct cfdata *data)
{
	struct property *prompt;
	struct pexpr *promptCondition;
	struct pexpr *fe_both;
	struct pexpr *pr_cond;
	struct pexpr *req_cond;

	if (!sym_is_boolean(sym))
		return;

	prompt = sym_get_prompt(sym);
	if (prompt == NULL)
		return;

	promptCondition =
		prompt->visible.expr ?
			expr_calculate_pexpr_both(prompt->visible.expr, data) :
			pexf(data->constants->const_true);
	fe_both = sym_get_fexpr_both(sym, data);
	req_cond = pexpr_implies_share(promptCondition, fe_both, data);
	sym_add_constraint(sym, req_cond, data);
	pr_cond = pexpr_implies_share(fe_both, promptCondition, data);
	sym_add_constraint(sym, pr_cond, data);
	PEXPR_PUT(promptCondition, fe_both);
}

/*
 * build constraints for dependencies (choice symbols and options)
 */
static void add_choice_dependencies(struct symbol *sym, struct cfdata *data)
{
	struct property *prompt;
	struct expr *to_parse;
	struct pexpr *dep_both;

	if (!sym_is_choice(sym) || !sym_is_choice_value(sym))
		return;

	prompt = sym_get_prompt(sym);
	if (prompt == NULL)
		return;

	if (sym_is_choice(sym)) {
		if (!prompt->visible.expr)
			return;
		to_parse = prompt->visible.expr;
	} else {
		if (!sym->dir_dep.expr)
			return;
		to_parse = sym->dir_dep.expr;
	}

	dep_both = expr_calculate_pexpr_both(to_parse, data);

	if (sym->type == S_TRISTATE) {
		struct pexpr *dep_y = expr_calculate_pexpr_y(to_parse, data);
		struct pexpr *c1 = pexpr_implies(pexf(sym->fexpr_y), dep_y,
						      data, PEXPR_ARG1);
		struct pexpr *c2 = pexpr_implies(
			pexf(sym->fexpr_m), dep_both, data, PEXPR_ARG1);

		sym_add_constraint_eq(sym, c1, data);
		sym_add_constraint_eq(sym, c2, data);
		pexpr_put(dep_y);
	} else if (sym->type == S_BOOLEAN) {
		struct pexpr *c = pexpr_implies(
			pexf(sym->fexpr_y), dep_both, data, PEXPR_ARG1);

		sym_add_constraint_eq(sym, c, data);
	}
	pexpr_put(dep_both);
}

/*
 * build constraints for the choice groups
 */
static void add_choice_constraints(struct symbol *sym, struct cfdata *data)
{
	struct property *prompt;
	struct symbol *choice, *choice2;
	struct sym_node *node, *node2;
	struct sym_list *items, *promptItems;
	struct property *prop;
	struct pexpr *c1;

	if (!sym_is_boolean(sym))
		return;

	prompt = sym_get_prompt(sym);
	if (prompt == NULL)
		return;

	/* create list of all choice options */
	items = sym_list_init();
	/* create list of choice options with a prompt */
	promptItems = sym_list_init();

	for_all_choices(sym, prop) {
		struct expr *expr;

		expr_list_for_each_sym(prop->expr, expr, choice) {
			sym_list_add(items, choice);
			if (sym_get_prompt(choice) != NULL)
				sym_list_add(promptItems, choice);
		}
	}

	/* if the choice is set to yes, at least one child must be set to yes */
	c1 = NULL;
	sym_list_for_each(node, promptItems) {
		choice = node->elem;
		c1 = node->prev == NULL ?
			     pexf(choice->fexpr_y) :
			     pexpr_or(c1, pexf(choice->fexpr_y), data,
					   PEXPR_ARGX);
	}
	if (c1 != NULL) {
		struct pexpr *c2 = pexpr_implies(pexf(sym->fexpr_y), c1,
						      data, PEXPR_ARG1);

		sym_add_constraint(sym, c2, data);
		pexpr_put(c1);
	}

	/* every choice option (even those without a prompt) implies the choice */
	sym_list_for_each(node, items) {
		choice = node->elem;
		c1 = pexpr_implies(sym_get_fexpr_both(choice, data),
					sym_get_fexpr_both(sym, data), data,
					PEXPR_ARGX);
		sym_add_constraint(sym, c1, data);
	}

	/* choice options can only select mod, if the entire choice is mod */
	if (sym->type == S_TRISTATE) {
		sym_list_for_each(node, items) {
			choice = node->elem;
			if (choice->type == S_TRISTATE) {
				c1 = pexpr_implies(pexf(choice->fexpr_m),
							pexf(sym->fexpr_m),
							data, PEXPR_ARGX);
				sym_add_constraint(sym, c1, data);
			}
		}
	}

	/* tristate options cannot be m, if the choice symbol is boolean */
	if (sym->type == S_BOOLEAN) {
		sym_list_for_each(node, items) {
			choice = node->elem;
			if (choice->type == S_TRISTATE)
				sym_add_constraint(
					sym,
					pexpr_not(pexf(choice->fexpr_m),
						       data),
					data);
		}
	}

	/* all choice options are mutually exclusive for yes */
	sym_list_for_each(node, promptItems) {
		choice = node->elem;
		for (struct sym_node *node2 = node->next; node2 != NULL;
		     node2 = node2->next) {
			choice2 = node2->elem;
			c1 = pexpr_or(
				pexpr_not(pexf(choice->fexpr_y), data),
				pexpr_not(pexf(choice2->fexpr_y), data),
				data, PEXPR_ARGX);
			sym_add_constraint(sym, c1, data);
		}
	}

	/* if one choice option with a prompt is set to yes,
	 * then no other option may be set to mod
	 */
	if (sym->type == S_TRISTATE) {
		sym_list_for_each(node, promptItems) {
			struct sym_list *tmp;

			choice = node->elem;

			tmp = sym_list_init();
			for (struct sym_node *node2 = node->next; node2 != NULL;
			     node2 = node2->next) {
				choice2 = node2->elem;
				if (choice2->type == S_TRISTATE)
					sym_list_add(tmp, choice2);
			}
			if (tmp->size == 0)
				continue;

			sym_list_for_each(node2, tmp) {
				choice2 = node2->elem;
				if (node2->prev == NULL)
					c1 = pexpr_not(
						pexf(choice2->fexpr_m), data);
				else
					c1 = pexpr_and(
						c1,
						pexpr_not(
							pexf(choice2->fexpr_m),
							data),
						data, PEXPR_ARGX);
			}
			c1 = pexpr_implies(pexf(choice->fexpr_y), c1, data,
						PEXPR_ARGX);
			sym_add_constraint(sym, c1, data);
		}
	}
}

/*
 * build the constraints for invisible options such as defaults
 */
static void add_invisible_constraints(struct symbol *sym, struct cfdata *data)
{
	struct property *prompt = sym_get_prompt(sym);
	struct pexpr *promptCondition_both, *promptCondition_yes, *noPromptCond;
	struct pexpr *npc;
	struct defm_list *defaults;
	struct pexpr *default_y, *default_m, *default_both;

	/* no constraints for the prompt, nothing to do here */
	if (prompt != NULL && !prompt->visible.expr)
		return;

	if (prompt == NULL) {
		promptCondition_both = pexf(data->constants->const_false);
		promptCondition_yes = pexf(data->constants->const_false);
		noPromptCond = pexf(data->constants->const_true);
	} else {
		struct property *p;

		promptCondition_both = pexf(data->constants->const_false);
		promptCondition_yes = pexf(data->constants->const_false);

		/* some symbols have multiple prompts */
		for_all_prompts(sym, p) {
			promptCondition_both =
				pexpr_or(promptCondition_both,
					      expr_calculate_pexpr_both(
						      p->visible.expr, data),
					      data, PEXPR_ARGX);
			promptCondition_yes = pexpr_or(
				promptCondition_yes,
				expr_calculate_pexpr_y(p->visible.expr, data),
				data, PEXPR_ARGX);
		}
		noPromptCond = pexpr_not_share(promptCondition_both, data);
	}

	if (NPC_OPTIMISATION) {
		struct fexpr *npc_fe =
			fexpr_create(data->sat_variable_nr++, FE_NPC, "");

		if (sym_is_choice(sym))
			str_append(&npc_fe->name, "Choice_");

		str_append(&npc_fe->name, sym_get_name(sym));
		str_append(&npc_fe->name, "_NPC");
		sym->noPromptCond = npc_fe;
		fexpr_add_to_satmap(npc_fe, data);

		npc = pexf(npc_fe);

		if (!sym_is_choice_value(sym) && !sym_is_choice(sym)) {
			struct pexpr *c =
				pexpr_implies_share(noPromptCond, npc, data);
			sym_add_constraint(sym, c, data);
		}
	} else {
		pexpr_get(noPromptCond);
		npc = noPromptCond;
	}

	defaults = get_defaults(sym, data);
	default_y = get_default_y(defaults, data);
	default_m = get_default_m(defaults, data);
	default_both = pexpr_or_share(default_y, default_m, data);

	/* tristate elements are only selectable as yes, if they are visible as yes */
	if (sym->type == S_TRISTATE) {
		struct pexpr *e1 = pexpr_implies(
			promptCondition_both,
			pexpr_implies(pexf(sym->fexpr_y),
					   promptCondition_yes, data,
					   PEXPR_ARG1),
			data, PEXPR_ARG2);

		sym_add_constraint(sym, e1, data);
	}

	/* if invisible and off by default, then a symbol can only be deactivated by its reverse dependencies */
	if (sym->type == S_TRISTATE) {
		struct pexpr *sel_y, *sel_m, *sel_both;
		struct pexpr *c1, *c2, *c3;
		struct pexpr *d1, *d2, *d3;
		struct pexpr *e1, *e2, *e3;

		if (sym->fexpr_sel_y != NULL) {
			sel_y = pexpr_implies(pexf(sym->fexpr_y),
						   pexf(sym->fexpr_sel_y), data,
						   PEXPR_ARGX);
			sel_m = pexpr_implies(pexf(sym->fexpr_m),
						   pexf(sym->fexpr_sel_m), data,
						   PEXPR_ARGX);
			sel_both = pexpr_implies(
				pexf(sym->fexpr_y),
				pexpr_or(pexf(sym->fexpr_sel_m),
					      pexf(sym->fexpr_sel_y), data,
					      PEXPR_ARGX),
				data, PEXPR_ARGX);
		} else {
			sel_y = pexpr_not(pexf(sym->fexpr_y), data);
			sel_m = pexpr_not(pexf(sym->fexpr_m), data);
			pexpr_get(sel_y);
			sel_both = sel_y;
		}

		c1 = pexpr_implies(pexpr_not_share(default_y, data), sel_y,
					data, PEXPR_ARG1);
		c2 = pexpr_implies(pexf(modules_sym->fexpr_y), c1, data,
					PEXPR_ARG1);
		c3 = pexpr_implies_share(npc, c2, data);
		sym_add_constraint(sym, c3, data);

		d1 = pexpr_implies(pexpr_not_share(default_m, data), sel_m,
					data, PEXPR_ARG1);
		d2 = pexpr_implies(pexf(modules_sym->fexpr_y), d1, data,
					PEXPR_ARG1);
		d3 = pexpr_implies_share(npc, d2, data);
		sym_add_constraint(sym, d3, data);

		e1 = pexpr_implies(pexpr_not_share(default_both, data),
					sel_both, data, PEXPR_ARG1);
		e2 = pexpr_implies(
			pexpr_not(pexf(modules_sym->fexpr_y), data), e1,
			data, PEXPR_ARG1);
		e3 = pexpr_implies_share(npc, e2, data);
		sym_add_constraint(sym, e3, data);
		PEXPR_PUT(sel_y, sel_m, sel_both, c1, c2, d1, d2, e1, e2);
	} else if (sym->type == S_BOOLEAN) {
		struct pexpr *sel_y;
		struct pexpr *e1, *e2;

		if (sym->fexpr_sel_y != NULL)
			sel_y = pexpr_implies(pexf(sym->fexpr_y),
						   pexf(sym->fexpr_sel_y), data,
						   PEXPR_ARGX);
		else
			sel_y = pexpr_not(pexf(sym->fexpr_y), data);

		e1 = pexpr_implies(pexpr_not_share(default_both, data),
					sel_y, data, PEXPR_ARG1);
		e2 = pexpr_implies_share(npc, e1, data);

		sym_add_constraint_eq(sym, e2, data);
		PEXPR_PUT(sel_y, e1);
	} else {
		/* if non-boolean is invisible and no default's condition is
		 * fulfilled, then the symbol is not set */
		struct pexpr *default_any = get_default_any(sym, data);
		struct pexpr *e1 = pexf(data->constants->const_true);
		struct pexpr *e2, *e3;

		/* e1 = "sym is not set" */
		for (struct fexpr_node *node = sym->nb_vals->head->next;
		     node != NULL; node = node->next)
			e1 = pexpr_and(
				e1, pexpr_not(pexf(node->elem), data),
				data, PEXPR_ARGX);

		e2 = pexpr_implies(pexpr_not_share(default_any, data), e1,
					data, PEXPR_ARG1);
		e3 = pexpr_implies_share(npc, e2, data);

		sym_add_constraint(sym, e3, data);
		PEXPR_PUT(default_any, e1, e2);
	}

	/* if invisible and on by default, then a symbol can only be deactivated by its dependencies */
	if (sym->type == S_TRISTATE) {
		struct pexpr *e1;
		struct pexpr *e2;

		if (defaults->size == 0)
			return;

		e1 = pexpr_implies(npc,
					pexpr_implies(default_y,
							   pexf(sym->fexpr_y),
							   data, PEXPR_ARG2),
					data, PEXPR_ARG2);
		sym_add_constraint(sym, e1, data);

		e2 = pexpr_implies(
			npc,
			pexpr_implies(default_m,
					   sym_get_fexpr_both(sym, data), data,
					   PEXPR_ARG2),
			data, PEXPR_ARG2);
		sym_add_constraint(sym, e2, data);
	} else if (sym->type == S_BOOLEAN) {
		struct pexpr *c;
		struct pexpr *c2;

		if (defaults->size == 0)
			return;

		c = pexpr_implies(default_both, pexf(sym->fexpr_y), data,
				       PEXPR_ARG2);

		// TODO tristate choice hack

		c2 = pexpr_implies_share(npc, c, data);
		sym_add_constraint(sym, c2, data);
		PEXPR_PUT(c);
	} else {
		/* if non-boolean invisible, then it assumes the correct
		 * default (if any). */
		struct defm_node *node;
		struct pexpr *cond, *c;
		struct fexpr *f;

		defm_list_for_each(node, defaults) {
			f = node->elem->val;
			cond = node->elem->e;
			c = pexpr_implies(npc,
					       pexpr_implies(cond, pexf(f),
								  data,
								  PEXPR_ARG2),
					       data, PEXPR_ARG2);
			sym_add_constraint(sym, c, data);
		}
	}
	PEXPR_PUT(promptCondition_yes, promptCondition_both, noPromptCond, npc,
		  default_y, default_m, default_both);
}

/*
 * add the known values from the default and range properties
 */
static void sym_add_nonbool_values_from_default_range(struct symbol *sym,
						      struct cfdata *data)
{
	struct property *p;

	for_all_defaults(sym, p) {
		if (p == NULL)
			continue;

		/* add the value to known values, if it doesn't exist yet */
		sym_create_nonbool_fexpr(sym, p->expr->left.sym->name, data);
	}

	for_all_properties(sym, p, P_RANGE) {
		if (p == NULL)
			continue;

		/* add the values to known values, if they don't exist yet */
		sym_create_nonbool_fexpr(sym, p->expr->left.sym->name, data);
		sym_create_nonbool_fexpr(sym, p->expr->right.sym->name, data);
	}
}

/*
 * build the range constraints for int/hex:
 * For each range and each value in `sym->nb_vals` that's not in the range:
 *	If the range's condition is fulfilled, then sym can't have this value.
 */
static void sym_add_range_constraints(struct symbol *sym, struct cfdata *data)
{
	struct property *prop;
	struct pexpr *prevs;
	struct pexpr *propCond;
	struct pexpr_list *prevCond; // list of all conditions of the ranges
		// from the previous iterations
	prevCond = pexpr_list_init();

	for_all_properties(sym, prop, P_RANGE) {
		int base;
		long long range_min, range_max, tmp;
		struct fexpr_node *node;

		if (prop == NULL)
			continue;

		prevs = pexf(data->constants->const_true);
		propCond = prop_get_condition(prop, data);

		// construct prevs as "none of the previous ranges' conditions
		// were fulfilled but this range's condition is"
		if (prevCond->size == 0) {
			pexpr_get(propCond);
			prevs = propCond;
		} else {
			struct pexpr_node *node;

			pexpr_list_for_each(node, prevCond)
				prevs = pexpr_and(pexpr_not_share(node->elem,
								  data),
						  prevs, data, PEXPR_ARGX);

			prevs = pexpr_and(propCond, prevs, data,
					       PEXPR_ARG2);
		}
		pexpr_get(propCond);
		pexpr_list_add(prevCond, propCond);

		switch (sym->type) {
		case S_INT:
			base = 10;
			break;
		case S_HEX:
			base = 16;
			break;
		default:
			return;
		}

		range_min = sym_get_range_val(prop->expr->left.sym, base);
		range_max = sym_get_range_val(prop->expr->right.sym, base);

		/* can skip the first non-boolean value, since this is 'n' */
		fexpr_list_for_each(node, sym->nb_vals) {
			struct pexpr *not_nb_val;
			struct pexpr *c;

			if (node->prev == NULL)
				continue;

			tmp = strtoll(str_get(&node->elem->nb_val), NULL, base);

			/* known value is in range, nothing to do here */
			if (tmp >= range_min && tmp <= range_max)
				continue;

			not_nb_val = pexpr_not(pexf(node->elem), data);
			c = pexpr_implies_share(prevs, not_nb_val, data);
			sym_add_constraint(sym, c, data);
			pexpr_put(not_nb_val);
		}
		PEXPR_PUT(prevs, propCond);
	}

	/* free prevCond */
	struct pexpr_node *node;

	pexpr_list_for_each(node, prevCond)
		pexpr_put(node->elem);
	pexpr_list_free(prevCond);

}

/*
 * at least 1 of the known values for a non-boolean symbol must be true
 */
static void sym_nonbool_at_least_1(struct symbol *sym, struct cfdata *data)
{
	struct pexpr *e = NULL;
	struct fexpr_node *node;

	if (!sym_is_nonboolean(sym))
		return;

	fexpr_list_for_each(node, sym->nb_vals) {
		if (node->prev == NULL)
			e = pexf(node->elem);
		else
			e = pexpr_or(e, pexf(node->elem), data, PEXPR_ARGX);
	}
	sym_add_constraint(sym, e, data);
}

/*
 * at most 1 of the known values for a non-boolean symbol can be true
 */
static void sym_nonbool_at_most_1(struct symbol *sym, struct cfdata *data)
{
	struct fexpr_node *node1;

	if (!sym_is_nonboolean(sym))
		return;

	/* iterate over all subsets of sym->nb_vals of size 2 */
	fexpr_list_for_each(node1, sym->nb_vals) {
		struct pexpr *e1 = pexf(node1->elem);

		for (struct fexpr_node *node2 = node1->next; node2 != NULL;
		     node2 = node2->next) {
			struct pexpr *e2 = pexf(node2->elem);
			struct pexpr *e = pexpr_or(pexpr_not_share(e1, data),
						   pexpr_not_share(e2, data),
						   data, PEXPR_ARGX);

			sym_add_constraint(sym, e, data);
			pexpr_put(e2);
		}
		pexpr_put(e1);
	}
}

/*
 * a visible prompt for a non-boolean implies a value for the symbol
 */
static void sym_add_nonbool_prompt_constraint(struct symbol *sym,
					      struct cfdata *data)
{
	struct property *prompt;
	struct pexpr *promptCondition;
	struct pexpr *n;
	struct pexpr *c;

	prompt = sym_get_prompt(sym);
	if (prompt == NULL)
		return;

	promptCondition = prop_get_condition(prompt, data);
	n = pexf(sym_get_nonbool_fexpr(sym, "n"));

	if (n->type != PE_SYMBOL || n->left.fexpr == NULL)
		goto cleanup;

	c = pexpr_implies(promptCondition, pexpr_not_share(n, data), data,
			  PEXPR_ARG2);

	sym_add_constraint(sym, c, data);

cleanup:
	PEXPR_PUT(n, promptCondition);
}

static struct default_map *create_default_map_entry(struct fexpr *val,
						    struct pexpr *e)
{
	struct default_map *map = malloc(sizeof(struct default_map));

	map->val = val;
	map->e = e;

	return map;
}

/**
 * findDefaultEntry()
 * @val: Value that the entry must have
 * @defaults: List of defaults to search in
 * @constants: To get ``constants->const_false`` from
 *
 * Finds an entry in @defaults whose &default_map.val attribute is the same
 * pointer as the @val argument.
 *
 * Return: The condition &default_map.e of the found entry, or
 * ``pexf(constants->const_false)`` if none was found. To be pexpr_put() by the
 * caller.
 */
static struct pexpr *findDefaultEntry(struct fexpr *val,
				      struct defm_list *defaults,
				      struct constants *constants)
{
	struct defm_node *node;

	defm_list_for_each(node, defaults) {
		if (val == node->elem->val) {
			pexpr_get(node->elem->e);
			return node->elem->e;
		}
	}

	return pexf(constants->const_false);
}

/*
 * accumulated during execution of add_defaults(), a disjunction of the
 * conditions for all default props of a symbol
 */
static struct pexpr *covered;

static bool is_tri_as_num(struct symbol *sym)
{
	if (!sym->name)
		return false;

	return !strcmp(sym->name, "0")
		|| !strcmp(sym->name, "1")
		|| !strcmp(sym->name, "2");
}

/**
 * add_to_default_map() - Add to or update an entry in a default list
 */
static void add_to_default_map(struct defm_list *defaults,
			       struct default_map *entry, struct symbol *sym)
{
	/* as this is a map, the entry must be replaced if it already exists */
	if (sym_is_boolean(sym)) {
		struct default_map *map;
		struct defm_node *node;

		defm_list_for_each(node, defaults) {
			map = node->elem;
			if (map->val->sym == entry->val->sym) {
				map->e = entry->e;
				return;
			}
		}
		defm_list_add(defaults, entry);
	} else {
		struct default_map *map;
		struct defm_node *node;

		defm_list_for_each(node, defaults) {
			map = node->elem;
			if (map->val->satval == entry->val->satval) {
				map->e = entry->e;
				return;
			}
		}
		defm_list_add(defaults, entry);
	}
}

/**
 * updateDefaultList() - Update a default list with a new value-condition pair
 * @val: The value whose condition will be updated
 * @newCond: The condition of the default prop. Does not include the condition
 * that the earlier default's conditions are not fulfilled.
 * @result: the default list
 * @sym: the symbol that the defaults belong to
 *
 * Update the condition that @val will be used for @sym by considering the next
 * default property, whose condition is given by @newCond.
 */
static void updateDefaultList(struct fexpr *val, struct pexpr *newCond,
			      struct defm_list *result, struct symbol *sym,
			      struct cfdata *data)
{
	// The current condition of @val deduced from the previous default props
	struct pexpr *prevCond = findDefaultEntry(val, result, data->constants);
	// New combined condition for @val
	struct pexpr *condUseVal = pexpr_or(
		prevCond, pexpr_and(newCond, pexpr_not_share(covered, data), data, PEXPR_ARG2),
		data, PEXPR_ARG2);
	struct default_map *entry = create_default_map_entry(val, condUseVal);

	add_to_default_map(result, entry, sym);
	covered = pexpr_or(covered, newCond, data, PEXPR_ARG1);
	PEXPR_PUT(prevCond);
}

/**
 * add_defaults() - Generate list of default values and their conditions
 * @defaults: List of the default properties
 * @ctx: Additional condition that needs to be fulfilled for any default. May be
 * NULL.
 * @result: List that will be filled
 * @sym: Symbol that the defaults belong to
 *
 * Creates a map from values that @sym can assume to the conditions under which
 * they will be assumed. Without @ctx, this will only consider the conditions
 * directly associated with the defaults, e.g. sym->dir_dep would not be
 * considered.
 *
 * As a side effect, the &symbol->nb_vals of @sym will be added for
 * all default values (as well as the @symbol->nb_vals of other symbols @sym has
 * as default (recursively)).
 */
static void add_defaults(struct prop_list *defaults, struct expr *ctx,
			 struct defm_list *result, struct symbol *sym,
			 struct cfdata *data)
{
	struct prop_node *node;
	struct property *p;
	struct expr *expr;

	prop_list_for_each(node, defaults) {
		p = node->elem;
		/* calculate expr as whether the default's condition (and the
		 * one inherited from ctx) is fulfilled */
		if (p->visible.expr) {
			if (ctx == NULL)
				expr = p->visible.expr;
			else
				expr = expr_alloc_and(p->visible.expr, ctx);
		} else {
			if (ctx == NULL)
				expr = expr_alloc_symbol(&symbol_yes);
			else
				expr = expr_alloc_and(
					expr_alloc_symbol(&symbol_yes), ctx);
		}
		// TODOle free expr's

		/* if tristate and def.value = y */
		if (p->expr->type == E_SYMBOL && sym->type == S_TRISTATE &&
		    p->expr->left.sym == &symbol_yes) {
			struct pexpr *expr_y =
				expr_calculate_pexpr_y(expr, data);
			struct pexpr *expr_m =
				expr_calculate_pexpr_m(expr, data);

			updateDefaultList(data->constants->symbol_yes_fexpr,
					  expr_y, result, sym, data);
			updateDefaultList(data->constants->symbol_mod_fexpr,
					  expr_m, result, sym, data);
			PEXPR_PUT(expr_y, expr_m);
		}
		/* if def.value = n/m/y */
		else if (p->expr->type == E_SYMBOL &&
			 sym_is_tristate_constant(p->expr->left.sym) &&
			 sym_is_boolean(sym)) {
			struct fexpr *s;
			struct pexpr *expr_both =
				expr_calculate_pexpr_both(expr, data);

			if (p->expr->left.sym == &symbol_yes)
				s = data->constants->symbol_yes_fexpr;
			else if (p->expr->left.sym == &symbol_mod)
				s = data->constants->symbol_mod_fexpr;
			else
				s = data->constants->symbol_no_fexpr;

			updateDefaultList(s, expr_both, result, sym, data);
			pexpr_put(expr_both);
		}
		/* if def.value = n/m/y, but written as 0/1/2 for a boolean */
		else if (sym_is_boolean(sym) && p->expr->type == E_SYMBOL &&
			 p->expr->left.sym->type == S_UNKNOWN &&
			 is_tri_as_num(p->expr->left.sym)) {
			struct fexpr *s;
			struct pexpr *expr_both =
				expr_calculate_pexpr_both(expr, data);

			if (!strcmp(p->expr->left.sym->name, "0"))
				s = data->constants->symbol_no_fexpr;
			else if (!strcmp(p->expr->left.sym->name, "1"))
				s = data->constants->symbol_mod_fexpr;
			else
				s = data->constants->symbol_yes_fexpr;

			updateDefaultList(s, expr_both, result, sym, data);
			pexpr_put(expr_both);
		}
		/* if def.value = non-boolean constant */
		else if (expr_is_nonbool_constant(p->expr)) {
			struct fexpr *s = sym_get_or_create_nonbool_fexpr(
				sym, p->expr->left.sym->name, data);
			struct pexpr *expr_both =
				expr_calculate_pexpr_both(expr, data);

			updateDefaultList(s, expr_both, result, sym, data);
			pexpr_put(expr_both);
		}
		/* any expression which evaluates to n/m/y for a tristate */
		else if (sym->type == S_TRISTATE) {
			struct expr *e_tmp = expr_alloc_and(p->expr, expr);
			struct pexpr *expr_y =
				expr_calculate_pexpr_y(e_tmp, data);
			struct pexpr *expr_m =
				expr_calculate_pexpr_m(e_tmp, data);

			updateDefaultList(data->constants->symbol_yes_fexpr,
					  expr_y, result, sym, data);
			updateDefaultList(data->constants->symbol_mod_fexpr,
					  expr_m, result, sym, data);
			PEXPR_PUT(expr_y, expr_m);
		}
		/* if non-boolean && def.value = non-boolean symbol */
		else if (p->expr->type == E_SYMBOL && sym_is_nonboolean(sym) &&
			 sym_is_nonboolean(p->expr->left.sym)) {
			struct prop_list *nb_sym_defaults = prop_list_init();
			struct property *p_tmp;

			/* Add defaults of other symbol as possible defaults for
			 * this symbol */
			for_all_defaults(p->expr->left.sym, p_tmp)
				prop_list_add(nb_sym_defaults, p_tmp);

			add_defaults(nb_sym_defaults, expr, result, sym, data);
			prop_list_free(nb_sym_defaults);
		}
		/* any expression which evaluates to n/m/y */
		else {
			struct expr *e_tmp = expr_alloc_and(p->expr, expr);
			struct pexpr *expr_both =
				expr_calculate_pexpr_both(e_tmp, data);

			updateDefaultList(data->constants->symbol_yes_fexpr,
					  expr_both, result, sym, data);

			pexpr_put(expr_both);
		}
	}
}

/**
 * get_defaults() - Generate list of default values and their conditions
 * @sym: Symbol whose defaults we want to look at
 *
 * Creates a map from values that @sym can assume to the conditions under which
 * they will be assumed. This will only consider the conditions
 * directly associated with the defaults, e.g. sym->dir_dep would not be
 * considered.
 *
 * As a side effect, the &symbol->nb_vals of @sym will be added for
 * all default values (as well as the @symbol->nb_vals of other symbols @sym has
 * as default (recursively)).
 */
static struct defm_list *get_defaults(struct symbol *sym, struct cfdata *data)
{
	struct defm_list *result = defm_list_init();
	struct prop_list *defaults; /* list of default props of sym */
	struct property *p;

	covered = pexf(data->constants->const_false);

	defaults = prop_list_init();
	for_all_defaults(sym, p)
		prop_list_add(defaults, p);

	add_defaults(defaults, NULL, result, sym, data);
	prop_list_free(defaults);
	pexpr_put(covered);

	return result;
}

/*
 * return the condition for "y", False if it doesn't exist
 */
static struct pexpr *get_default_y(struct defm_list *list, struct cfdata *data)
{
	struct default_map *entry;
	struct defm_node *node;

	defm_list_for_each(node, list) {
		entry = node->elem;
		if (entry->val->type == FE_SYMBOL &&
		    entry->val->sym == &symbol_yes) {
			pexpr_get(entry->e);
			return entry->e;
		}
	}

	return pexf(data->constants->const_false);
}

/*
 * return the condition for "m", False if it doesn't exist
 */
static struct pexpr *get_default_m(struct defm_list *list, struct cfdata *data)
{
	struct default_map *entry;
	struct defm_node *node;

	defm_list_for_each(node, list) {
		entry = node->elem;
		if (entry->val->type == FE_SYMBOL &&
		    entry->val->sym == &symbol_mod) {
			pexpr_get(entry->e);
			return entry->e;
		}
	}

	return pexf(data->constants->const_false);
}

/*
 * return the constraint when _some_ default value will be applied
 */
static struct pexpr *get_default_any(struct symbol *sym, struct cfdata *data)
{
	struct property *prop;
	struct expr *e;
	struct pexpr *p;

	if (!sym_is_nonboolean(sym))
		return NULL;

	p = pexf(data->constants->const_false);
	for_all_defaults(sym, prop) {
		if (prop->visible.expr)
			e = prop->visible.expr;
		else
			e = expr_alloc_symbol(&symbol_yes);

		if (expr_can_evaluate_to_mod(e))
			p = pexpr_or(p, expr_calculate_pexpr_both(e, data),
				     data, PEXPR_ARGX);

		p = pexpr_or(p, expr_calculate_pexpr_y(e, data), data,
			     PEXPR_ARGX);
	}

	return p;
}

/*
 * get the value for the range
 */
static long sym_get_range_val(struct symbol *sym, int base)
{
	sym_calc_value(sym);
	switch (sym->type) {
	case S_INT:
		base = 10;
		break;
	case S_HEX:
		base = 16;
		break;
	default:
		break;
	}
	return strtol(sym->curr.val, NULL, base);
}

/*
 * count the number of all constraints
 */
unsigned int count_counstraints(void)
{
	unsigned int c = 0;
	struct symbol *sym;

	for_all_symbols(sym) {
		if (sym->type == S_UNKNOWN)
			continue;

		c += sym->constraints->size;
	}

	return c;
}

/*
 * add a constraint for a symbol
 */
void sym_add_constraint(struct symbol *sym, struct pexpr *constraint,
			struct cfdata *data)
{
	if (!constraint)
		return;

	/* no need to add that */
	if (constraint->type == PE_SYMBOL &&
	    constraint->left.fexpr == data->constants->const_true)
		return;

	/* this should never happen */
	if (constraint->type == PE_SYMBOL &&
	    constraint->left.fexpr == data->constants->const_false)
		perror("Adding const_false.");

	pexpr_list_add(sym->constraints, constraint);

	if (!pexpr_is_nnf(constraint))
		pexpr_print("Not NNF:", constraint, -1);
}

/*
 * add a constraint for a symbol, but check for duplicate constraints
 */
void sym_add_constraint_eq(struct symbol *sym, struct pexpr *constraint,
			   struct cfdata *data)
{
	struct pexpr_node *node;

	if (!constraint)
		return;

	/* no need to add that */
	if (constraint->type == PE_SYMBOL &&
	    constraint->left.fexpr == data->constants->const_true)
		return;

	/* this should never happen */
	if (constraint->type == PE_SYMBOL &&
	    constraint->left.fexpr == data->constants->const_false)
		perror("Adding const_false.");

	/* check the constraints for the same symbol */
	pexpr_list_for_each(node, sym->constraints)
		if (pexpr_eq(constraint, node->elem, data))
			return;

	pexpr_list_add(sym->constraints, constraint);

	if (!pexpr_is_nnf(constraint))
		pexpr_print("Not NNF:", constraint, -1);
}
