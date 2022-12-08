// SPDX-License-Identifier: GPL-2.0
#define _GNU_SOURCE
#include <linux/membarrier.h>
#include <syscall.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "membarrier_test_impl.h"

int main(int argc, char **argv)
{
	ksft_print_header();
<<<<<<< HEAD
	ksft_set_plan(18);

	test_membarrier_get_registrations(/*cmd=*/0);
=======
	ksft_set_plan(13);
>>>>>>> b7ba80a49124 (Commit)

	test_membarrier_query();

	test_membarrier_fail();

	test_membarrier_success();

<<<<<<< HEAD
	test_membarrier_get_registrations(/*cmd=*/0);

=======
>>>>>>> b7ba80a49124 (Commit)
	return ksft_exit_pass();
}
