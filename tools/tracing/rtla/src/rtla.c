// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Red Hat Inc, Daniel Bristot de Oliveira <bristot@kernel.org>
 */

#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "osnoise.h"
#include "timerlat.h"

/*
 * rtla_usage - print rtla usage
 */
<<<<<<< HEAD
static void rtla_usage(int err)
=======
static void rtla_usage(void)
>>>>>>> b7ba80a49124 (Commit)
{
	int i;

	static const char *msg[] = {
		"",
		"rtla version " VERSION,
		"",
		"  usage: rtla COMMAND ...",
		"",
		"  commands:",
		"     osnoise  - gives information about the operating system noise (osnoise)",
<<<<<<< HEAD
		"     hwnoise  - gives information about hardware-related noise",
=======
>>>>>>> b7ba80a49124 (Commit)
		"     timerlat - measures the timer irq and thread latency",
		"",
		NULL,
	};

	for (i = 0; msg[i]; i++)
		fprintf(stderr, "%s\n", msg[i]);
<<<<<<< HEAD
	exit(err);
=======
	exit(1);
>>>>>>> b7ba80a49124 (Commit)
}

/*
 * run_command - try to run a rtla tool command
 *
 * It returns 0 if it fails. The tool's main will generally not
 * return as they should call exit().
 */
int run_command(int argc, char **argv, int start_position)
{
	if (strcmp(argv[start_position], "osnoise") == 0) {
		osnoise_main(argc-start_position, &argv[start_position]);
		goto ran;
<<<<<<< HEAD
	} else if (strcmp(argv[start_position], "hwnoise") == 0) {
		hwnoise_main(argc-start_position, &argv[start_position]);
		goto ran;
=======
>>>>>>> b7ba80a49124 (Commit)
	} else if (strcmp(argv[start_position], "timerlat") == 0) {
		timerlat_main(argc-start_position, &argv[start_position]);
		goto ran;
	}

	return 0;
ran:
	return 1;
}

int main(int argc, char *argv[])
{
	int retval;

	/* is it an alias? */
	retval = run_command(argc, argv, 0);
	if (retval)
		exit(0);

	if (argc < 2)
		goto usage;

	if (strcmp(argv[1], "-h") == 0) {
<<<<<<< HEAD
		rtla_usage(0);
	} else if (strcmp(argv[1], "--help") == 0) {
		rtla_usage(0);
=======
		rtla_usage();
		exit(0);
	} else if (strcmp(argv[1], "--help") == 0) {
		rtla_usage();
		exit(0);
>>>>>>> b7ba80a49124 (Commit)
	}

	retval = run_command(argc, argv, 1);
	if (retval)
		exit(0);

usage:
<<<<<<< HEAD
	rtla_usage(1);
=======
	rtla_usage();
>>>>>>> b7ba80a49124 (Commit)
	exit(1);
}
