#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expr.h"
#include "cf_utils.h"
#include "lkc_proto.h"
#include "list_types.h"
#include "list.h"
#include "lkc.h"
#include "cf_defs.h"
#include "picosat_functions.h"
#include <strings.h>

#define fatal(...)                            \
	do {                                  \
		fprintf(stderr, __VA_ARGS__); \
		exit(EXIT_FAILURE);           \
	} while (0)

static const char *dot_config_input_name = NULL;
static const char *dot_config_out_name = NULL;
static const char *kconfig_name = NULL;
static struct sdv_list *conflict;

struct string_list {
	struct list_head list;
};

struct string_node {
	const char *elem;
	struct list_head node;
};

static void usage(void)
{
	const char *msg = "\
  Usage:\n\
      ./cfixconf [<Kconfig>] [options]\n\
      where <Kconfig> is the root file of the Kconfig model. If not specified,\n\
      <Kconfig> is \"Kconfig\".\n\
\n\
  Options:\n\
      -i --input   (\".config\" by default) .config file to load the initial\n\
                   configuration from.\n\
      -o --output  (\".config\" by default) .config file to store the\n\
                   configuration.\n\
      -h --help    Show this help text.\n\
\n\
";
	fprintf(stderr, "%s", msg);
}

/**
 * Allocates copy of str where each upper case letter is replaced by its upper
 * case equivalent.
 */
static char *to_upper(const char *str)
{
	const size_t len = strlen(str);
	char *cpy = xmalloc(len + 1);

	for (size_t i = 0; i < len; ++i)
		cpy[i] = (char) toupper(str[i]);
	cpy[len] = '\0';
	return cpy;
}

static void add_conflict_symbol(struct symbol *sym, tristate val)
{
	struct symbol_dvalue *conflict_entry = xmalloc(sizeof *conflict_entry);
	struct sdv_node *entry, *entry2;

	conflict_entry->type = SDV_BOOLEAN;
	conflict_entry->sym = sym;
	conflict_entry->tri = val;
	list_for_each_entry_safe(entry, entry2, &conflict->list, node) {
		if (entry->elem->sym != sym)
			continue;

		if (entry->elem->tri != val)
			printf("Overwriting previous symbol value \"%s\"\n",
			       tristate_get_char(entry->elem->tri));
		list_del(&entry->node);
		free(entry);
	}
	CF_PUSH_BACK(conflict, conflict_entry, sdv);
	sym_calc_value(sym);
	printf("Added conflict symbol %s: %s -> %s\n", sym->name,
	       tristate_get_char(sym->curr.tri), tristate_get_char(val));
}

static void remove_conflict_symbol(struct symbol *sym)
{
	struct sdv_node *entry, *entry2;
	bool deleted = false;

	list_for_each_entry_safe(entry, entry2, &conflict->list, node) {
		if (entry->elem->sym == sym) {
			list_del(&entry->node);
			printf("Deleted conflict symbol %s\n", sym->name);
			free(entry);
			deleted = true;
		}
	}
	if (!deleted)
		printf("Symbol not in conflict\n");
}

/*
 * Parses an add command and sets *sym and *val to the parsed values.
 * Returns whether the line could be parsed.
 */
static bool parse_add(struct string_list *tokens, struct symbol **sym,
		      tristate *val)
{
	struct string_node *entry;
	const char *const err_msg = "%s, expected: add <symbol> <value>\n";
	const char *sym_name = NULL, *val_name = NULL;
	char *sym_name_upper;
	int i = 0;

	CF_LIST_FOR_EACH(entry, tokens, string) {
		switch (i) {
		case 0:
			break;
		case 1:
			sym_name = entry->elem;
			break;
		case 2:
			val_name = entry->elem;
			break;
		default:
			printf(err_msg, "Too many arguments");
			return false;
		}
		++i;
	}
	if (!sym_name || !val_name) {
		printf(err_msg, "Too few arguments");
		return false;
	}
	sym_name_upper = to_upper(sym_name);
	*sym = sym_find(sym_name_upper);
	if (!*sym) {
		printf("No such symbol \"%s\"\n", sym_name_upper);
		free(sym_name_upper);
		return false;
	}
	free(sym_name_upper);
	if (sym_is_nonboolean(*sym)) {
		printf("Only symbols of type tristate and bool are supported; symbol %s has type %s\n",
		       (*sym)->name, sym_type_name((*sym)->type));
		return false;
	}
	if (!strcasecmp(val_name, "yes") | !strcasecmp(val_name, "y")) {
		*val = yes;
	} else if (!strcasecmp(val_name, "mod") | !strcasecmp(val_name, "m")) {
		if ((*sym)->type == S_BOOLEAN) {
			printf("Cannot assign mod to symbol of type bool\n");
			return false;
		}
		*val = mod;
	} else if (!strcasecmp(val_name, "no") | !strcasecmp(val_name, "n")) {
		*val = no;
	} else {
		printf("Invalid value \"%s\", expected \"yes\", \"mod\" or \"no\"\n",
		       val_name);
		return false;
	}
	return true;
}

/*
 * tokens must not be empty
 */
static void handle_add(struct string_list *tokens)
{
	struct symbol *sym;
	tristate val;
	bool parse_succ;

	parse_succ = parse_add(tokens, &sym, &val);
	if (!parse_succ)
		return;
	add_conflict_symbol(sym, val);
}

/*
 * Parses a rm command and sets *sym to the parsed value.
 * Returns whether the line could be parsed.
 */
static bool parse_rm(struct string_list *tokens, struct symbol **sym)
{
	struct string_node *entry;
	const char *const err_msg = "%s, expected: rm <symbol>\n";
	const char *sym_name = NULL;
	char *sym_name_upper;
	int i = 0;

	CF_LIST_FOR_EACH(entry, tokens, string) {
		switch (i) {
		case 0:
			break;
		case 1:
			sym_name = entry->elem;
			break;
		default:
			printf(err_msg, "Too many arguments");
			return false;
		}
		++i;
	}
	if (!sym_name) {
		printf(err_msg, "Too few arguments");
		return false;
	}
	sym_name_upper = to_upper(sym_name);
	*sym = sym_find(sym_name_upper);
	if (!*sym) {
		printf("No such symbol \"%s\"\n", sym_name_upper);
		free(sym_name_upper);
		return false;
	}
	free(sym_name_upper);
	return true;
}

static void handle_rm(struct string_list *tokens)
{
	struct symbol *sym;
	bool parse_succ;

	parse_succ = parse_rm(tokens, &sym);
	if (!parse_succ)
		return;
	remove_conflict_symbol(sym);
}

static void handle_clear(struct string_list *tokens)
{
	if (list_count_nodes(&tokens->list) != 1) {
		printf("Too many arguments, expected: clear\n");
		return;
	}
	if (list_empty(&conflict->list)) {
		printf("Conflict already empty\n");
		return;
	}
	CF_LIST_FREE(conflict, sdv);
	conflict = CF_LIST_INIT(sdv);
	printf("Cleared conflict\n");
}

static void handle_show(struct string_list *tokens)
{
	struct sdv_node *entry;
	int conflict_len = 0;

	if (list_count_nodes(&tokens->list) != 1) {
		printf("Too many arguments, expected: show\n");
		return;
	}
	CF_LIST_FOR_EACH(entry, conflict, sdv)
	{
		const struct symbol_dvalue *sdv = entry->elem;

		sym_calc_value(sdv->sym);
		printf("%s: %s -> %s\n", sdv->sym->name, tristate_get_char(sdv->sym->curr.tri),
		       tristate_get_char(sdv->tri));
		++conflict_len;
	}
	if (conflict_len == 0)
		printf("No symbols in conflict\n");
}

static void handle_help(void)
{
	const char *text = "\
Commands:\n\
    add <symbol> <value>  Add symbol with value to conflict\n\
    show                  List all symbols in conflict\n\
    rm <symbol>           Remove symbol from conflict\n\
    clear                 Clear conflict\n\
    help                  Show this help text\n\
";
	printf("%s", text);
}

static void handle_line(struct string_list *tokens)
{
	if (list_empty(&tokens->list))
		return;

	const char *cmd =
		list_first_entry(&tokens->list, struct string_node, node)->elem;
	if (!strcasecmp(cmd, "add"))
		handle_add(tokens);
	else if (!strcasecmp(cmd, "show"))
		handle_show(tokens);
	else if (!strcasecmp(cmd, "help"))
		handle_help();
	else if (!strcasecmp(cmd, "rm"))
		handle_rm(tokens);
	else if (!strcasecmp(cmd, "clear"))
		handle_clear(tokens);
	else
		printf("Unknown command \"%s\", type \"help\" for a list of commands\n",
		       cmd);
}

static struct string_list *tokenize_line(char *in)
{
	char *saveptr;
	char *str = in;
	CF_DEF_LIST(tokens, string);

	while (true) {
		char *token = strtok_r(str, " \t\n", &saveptr);
		str = NULL;
		if (!token)
			break;
		CF_PUSH_BACK(tokens, token, string);
	}

	return tokens;
}

static void read_loop(void)
{
	while (true) {
		struct gstr in = str_new();
		struct string_list *tokens;

		printf("> ");
		do {
			int next_char = fgetc(stdin);
			if (next_char == EOF) {
				if (ferror(stdin))
					fatal("Error reading stdin\n");
				assert(feof(stdin));
				printf("\n");
				return;
			} else if (next_char == '\n') {
				break;
			} else {
				str_append(&in, (char[]){ next_char, '\0' });
			}
		} while (true);
		tokens = tokenize_line(str_get(&in));
		handle_line(tokens);
		CF_LIST_FREE(tokens, string);
		str_free(&in);
	}
}

static void parse_args(int argc, char *argv[])
{
	for (int i = 1; i < argc; ++i) {
		const char *const arg = argv[i];
		if (!strcmp(arg, "-h") || !strcmp(arg, "--help")) {
			usage();
			exit(EXIT_SUCCESS);
		} else if (!strcmp(arg, "-i") || !strcmp(arg, "--input")) {
			++i;
			if (i == argc)
				fatal("%s needs to be followed by a file name.\n",
				      arg);
			dot_config_input_name = argv[i];
		} else if (!strcmp(arg, "-o") || !strcmp(arg, "--output")) {
			++i;
			if (i == argc)
				fatal("%s needs to be followed by a file name.\n",
				      arg);
			dot_config_out_name = argv[i];
		} else if (i == 1) {
			kconfig_name = arg;
		} else {
			fatal("Unknown flag %s\n", arg);
		}
	}
	if (!dot_config_input_name)
		dot_config_input_name = ".config";
	if (!dot_config_out_name)
		dot_config_out_name = ".config";
	if (!kconfig_name)
		kconfig_name = "Kconfig";
}

int main(int argc, char *argv[])
{
	parse_args(argc, argv);
	if (!load_picosat())
		fatal("Could not load PicoSAT\n");
	conf_parse(kconfig_name);
	conf_read(dot_config_input_name);
	conflict = CF_LIST_INIT(sdv);
	read_loop();
	return EXIT_SUCCESS;
}
