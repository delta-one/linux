#!/bin/bash
# SPDX-License-Identifier: GPL-2.0

source ./benchs/run_common.sh

set -eufo pipefail

nr_threads=`expr $(cat /proc/cpuinfo | grep "processor"| wc -l) - 1`
<<<<<<< HEAD
summary=$($RUN_BENCH -p $nr_threads bpf-hashmap-full-update)
=======
summary=$($RUN_BENCH -p $nr_threads bpf-hashmap-ful-update)
>>>>>>> b7ba80a49124 (Commit)
printf "$summary"
printf "\n"
