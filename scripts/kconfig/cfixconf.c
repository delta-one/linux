#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define fatal(format_str, ...)                            \
	do {                                              \
		fprintf(stderr, format_str, __VA_ARGS__); \
		exit(EXIT_FAILURE);                       \
	} while (0)

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

int main(int argc, char *argv[])
{
	const char *dot_config_input_name = NULL;
	const char *dot_config_out_name = NULL;
	const char *kconfig_name = NULL;
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
	return EXIT_SUCCESS;
}
