// SPDX-License-Identifier: GPL-2.0-or-later
#include "tests/basic_api.h"
#include "tests/alloc_api.h"
#include "tests/alloc_helpers_api.h"
#include "tests/alloc_nid_api.h"
<<<<<<< HEAD
#include "tests/alloc_exact_nid_api.h"
=======
>>>>>>> b7ba80a49124 (Commit)
#include "tests/common.h"

int main(int argc, char **argv)
{
	parse_args(argc, argv);
	memblock_basic_checks();
	memblock_alloc_checks();
	memblock_alloc_helpers_checks();
	memblock_alloc_nid_checks();
<<<<<<< HEAD
	memblock_alloc_exact_nid_checks();
=======
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}
