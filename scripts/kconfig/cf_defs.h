/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2023 Patrick Franz <deltaone@debian.org>
 */

#ifndef DEFS_H
#define DEFS_H

/* global variables */
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>

#include "lkc.h"
#include "expr.h"
#include "list_types.h"

extern bool CFDEBUG;
extern bool stop_rangefix;

#define printd(fmt...) do { \
	if (CFDEBUG) \
		printf(fmt); \
} while (0)

/*
 * For functions that construct nested pexpr expressions.
 */
enum pexpr_move {
	PEXPR_ARG1,	/* put reference to first pexpr */
	PEXPR_ARG2,	/* put reference to second pexpr */
	PEXPR_ARGX	/* put all references to pexpr's */
};


/* different types for f_expr */
enum fexpr_type {
	FE_SYMBOL,
	FE_NPC, /* no prompt condition */
	FE_TRUE,  /* constant of value True */
	FE_FALSE,  /* constant of value False */
	FE_NONBOOL,  /* for all non-(boolean/tristate) known values */
	FE_CHOICE, /* symbols of type choice */
	FE_SELECT, /* auxiliary variable for selected symbols */
	FE_TMPSATVAR /* temporary sat-variable (Tseytin) */
};

/* struct for a propositional logic formula */
struct fexpr {
	/* name of the feature expr */
	struct gstr name;

	/* associated symbol */
	struct symbol *sym;

	/* integer value for the SAT solver */
	int satval;

	/* assumption in the last call to PicoSAT */
	bool assumption;

	/* type of the fexpr */
	enum fexpr_type type;

	union {
		/* symbol */
		struct {
			tristate tri;
		};
		/* EQUALS */
		struct {
			struct symbol *eqsym;
			struct symbol *eqvalue;
		};
		/* HEX, INTEGER, STRING */
		struct {
			struct gstr nb_val;
		};
	};
};

/*
 * CF_ALLOC_NODE - Utility macro for allocating, initializing and returning an 
 * object of a type like struct fexpr_node
 *
 * @node_type: type of the object to create a pointer to (e.g. struct fexpr_node)
 * @el: the value to set field .element to
 */
#define CF_ALLOC_NODE(node_type, el)                                          \
	({                                                                    \
		struct node_type *__node = xmalloc(sizeof(struct node_type)); \
		__node->elem = el;                                            \
		INIT_LIST_HEAD(&__node->node);                                \
		__node;                                                       \
	})
/*
 * constructs an object using CF_ALLOC_NODE(node_type, el) and then adds to the
 * end of list->list
 */
#define CF_EMPLACE_BACK(list_, node_type, el)                                 \
	do {                                                                  \
		struct node_type *__cf_emplace_back_node =                    \
			CF_ALLOC_NODE(node_type, el);                         \
		list_add_tail(&__cf_emplace_back_node->node, &(list_)->list); \
	} while (0)

/*
 * frees all nodes and then list_
 */
#define CF_LIST_FREE(list_, node_type)                                   \
	do {                                                             \
		struct node_type *__node, *__next;                       \
		list_for_each_entry_safe(__node, __next, &(list_)->list, \
					 node) {                         \
			list_del(&__node->node);                         \
			free(__node);                                    \
		}                                                        \
		free(list_);                                             \
	} while (0)

/*
 * declares and initializes a list
 */
#define CF_DEF_LIST(name, list_type)                  \
	struct list_type *name = ({                        \
		struct list_type *__cf_list =              \
			xmalloc(sizeof(struct list_type)); \
		INIT_LIST_HEAD(&__cf_list->list);          \
		__cf_list;                                 \
	});

struct fexpr_node {
	struct fexpr *elem;
	struct list_head node;
};

struct fexpr_list {
	struct list_head list;
};

struct fexl_list {
	struct fexl_node *head, *tail;
	unsigned int size;
};

struct fexl_node {
	struct fexpr_list *elem;
	struct fexl_node *next, *prev;
};

enum pexpr_type {
	PE_SYMBOL,
	PE_AND,
	PE_OR,
	PE_NOT
};

union pexpr_data {
	struct pexpr *pexpr;
	struct fexpr *fexpr;
};

/**
 * struct pexpr - a node in a tree representing a propositional formula
 * @type: Type of the node
 * @left: left-hand-side for AND and OR, the unique operand for NOT, and for
 * SYMBOL it contains the fpexpr.
 * @right: right-hand-side for AND and OR
 * @ref_count: Number of calls to pexpr_put() that need to effectuated with this
 * pexpr for it to get free'd.
 *
 * Functions that return new struct pexpr instances (like pexpr_or(),
 * pexpr_or_share(), pexf(), ...) set @ref_count in a way that accounts for the
 * new reference that they return (e.g. pexf() will always set it to 1).
 * Functions with arguments of type ``struct pexpr *`` will generally keep the
 * reference intact, so that for example
 * ``e = pexf(sym); not_e = pexpr_not_share(e)`` would require
 * ``pexpr_put(not_e)`` before not_e can be free'd and additionally
 * ``pexpr_put(e)`` for e to get free'd. Some functions take an argument of type
 * ``enum pexpr_move`` which function as a wrapper of sorts that first executes
 * a function and then pexpr_put's the argument(s) specified by the
 * ``enum pexpr_move`` argument (e.g. the normal function for OR is
 * pexpr_or_share() and the wrapper is pexpr_or()).
 */
struct pexpr {
	enum pexpr_type type;
	union pexpr_data left, right;
	unsigned int ref_count;
};

struct pexpr_list {
	struct pexpr_node *head, *tail;
	unsigned int size;
};

struct pexpr_node {
	struct pexpr *elem;
	struct pexpr_node *next, *prev;
};

/**
 * struct default_map - Map entry from default values to their condition
 * @val: value of the default property. Not 'owned' by this struct and
 * therefore shouldn't be free'd.
 * @e: condition that implies that the symbol assumes the @val. Needs to be
 * pexpr_put when free'ing.
 */
struct default_map {
	struct fexpr *val;
	struct pexpr *e;
};

/**
 * struct defm_list - Map from values of default properties of a symbol to their
 * (accumulated) conditions
 */
struct defm_list {
	struct defm_node *head, *tail;
	unsigned int size;
};

struct defm_node {
	struct default_map *elem;
	struct defm_node *next, *prev;
};

enum symboldv_type {
	SDV_BOOLEAN,	/* boolean/tristate */
	SDV_NONBOOLEAN	/* string/int/hex */
};

struct symbol_dvalue {
	struct symbol *sym;

	enum symboldv_type type;

	union {
		/* boolean/tristate */
		tristate tri;

		/* string/int/hex */
		struct gstr nb_val;
	};
};

struct sdv_list {
	struct sdv_node *head, *tail;
	unsigned int size;
};

struct sdv_node {
	struct symbol_dvalue *elem;
	struct sdv_node *next, *prev;
};

enum symbolfix_type {
	SF_BOOLEAN,	/* boolean/tristate */
	SF_NONBOOLEAN,	/* string/int/hex */
	SF_DISALLOWED	/* disallowed non-boolean values */
};

struct symbol_fix {
	struct symbol *sym;

	enum symbolfix_type type;

	union {
		/* boolean/tristate */
		tristate tri;

		/* string/int/hex */
		struct gstr nb_val;

		/* disallowed non-boolean values */
		struct gstr disallowed;
	};
};

struct sfix_list {
	struct sfix_node *head, *tail;
	unsigned int size;
};

struct sfix_node {
	struct symbol_fix *elem;
	struct sfix_node *next, *prev;
};

struct sfl_list {
	struct sfl_node *head, *tail;
	unsigned int size;
};

struct sfl_node {
	struct sfix_list *elem;
	struct sfl_node *next, *prev;
};

struct sym_list {
	struct sym_node *head, *tail;
	unsigned int size;
};

struct sym_node {
	struct symbol *elem;
	struct sym_node *next, *prev;
};

struct prop_list {
	struct prop_node *head, *tail;
	unsigned int size;
};

struct prop_node {
	struct property *elem;
	struct prop_node *next, *prev;
};

struct constants {
	struct fexpr *const_false;
	struct fexpr *const_true;
	struct fexpr *symbol_yes_fexpr;
	struct fexpr *symbol_mod_fexpr;
	struct fexpr *symbol_no_fexpr;
};

struct cfdata {
	unsigned int sat_variable_nr;
	unsigned int tmp_variable_nr;
	struct fexpr **satmap; // map SAT variables to fexpr
	size_t satmap_size;
	struct constants *constants;
	struct sdv_list *sdv_symbols; // array with conflict-symbols
};

#endif
