#!/bin/sh
# SPDX-License-Identifier: LGPL-2.1

[ $# -eq 1 ] && header_dir=$1 || header_dir=tools/include/uapi/asm-generic/

printf "static const char *madvise_advices[] = {\n"
regex='^[[:space:]]*#[[:space:]]*define[[:space:]]+MADV_([[:alnum:]_]+)[[:space:]]+([[:digit:]]+)[[:space:]]*.*'
<<<<<<< HEAD
grep -E $regex ${header_dir}/mman-common.h | \
=======
egrep $regex ${header_dir}/mman-common.h | \
>>>>>>> b7ba80a49124 (Commit)
	sed -r "s/$regex/\2 \1/g"	| \
	sort -n | xargs printf "\t[%s] = \"%s\",\n"
printf "};\n"
