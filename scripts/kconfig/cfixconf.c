#include "list_types.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lkc.h"
#include "cf_defs.h"
#include "list.h"
#include "picosat_functions.h"

#define fatal(...)                            \
	do {                                  \
		fprintf(stderr, __VA_ARGS__); \
		exit(EXIT_FAILURE);           \
	} while (0)

static const char *dot_config_input_name = NULL;
static const char *dot_config_out_name = NULL;
static const char *kconfig_name = NULL;
static struct sdv_list *conflict;
struct str_list {
	const char *str;
	struct list_head list;
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

static void handle_line(struct list_head *tokens)
{
	struct str_list *entry;

	list_for_each_entry(entry, tokens, list) {
		printf("%s\n", entry->str);
	}
}

static struct list_head *tokenize_line(char *in)
{
	char *saveptr;
	char *str = in;
	struct list_head *tokens = xmalloc(sizeof *tokens);

	INIT_LIST_HEAD(tokens);
	while (true) {
		char *token = strtok_r(str, " \t\n", &saveptr);
		struct str_list *entry;

		str = NULL;
		if (!token)
			break;
		entry = xmalloc(sizeof *entry);
		entry->str = token;
		list_add_tail(&entry->list, tokens);
	}

	return tokens;
}

static void read_loop(void)
{
	while (true) {
		struct gstr in = str_new();
		struct list_head *tokens;
		struct str_list *entry, *entry2;

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
		list_for_each_entry_safe(entry, entry2, tokens, list) {
			list_del(&entry->list);
			free(entry);
		}
		free(tokens);
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
