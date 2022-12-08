#!/bin/sh
# SPDX-License-Identifier: GPL-2.0

set -e

if [ $# != 1 ]; then
	echo "Usage: $0 <modules.order>" >& 2
	exit 1
fi

exit_code=0

# Check uniqueness of module names
check_same_name_modules()
{
	for m in $(sed 's:.*/::' "$1" | sort | uniq -d)
	do
		echo "error: the following would cause module name conflict:" >&2
<<<<<<< HEAD
		sed -n "/\/$m/s:^\(.*\)\.o\$:  \1.ko:p" "$1" >&2
=======
		sed -n "/\/$m/s:^:  :p" "$1" >&2
>>>>>>> b7ba80a49124 (Commit)
		exit_code=1
	done
}

check_same_name_modules "$1"

exit $exit_code
