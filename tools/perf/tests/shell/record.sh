#!/bin/sh
# perf record tests
# SPDX-License-Identifier: GPL-2.0

set -e

<<<<<<< HEAD
shelldir=$(dirname "$0")
. "${shelldir}"/lib/waiting.sh

err=0
perfdata=$(mktemp /tmp/__perf_test.perf.data.XXXXX)
testprog="perf test -w thloop"
testsym="test_loop"

cleanup() {
  rm -rf "${perfdata}"
  rm -rf "${perfdata}".old

  trap - EXIT TERM INT
=======
err=0
perfdata=$(mktemp /tmp/__perf_test.perf.data.XXXXX)

cleanup() {
  rm -f ${perfdata}
  rm -f ${perfdata}.old
  trap - exit term int
>>>>>>> b7ba80a49124 (Commit)
}

trap_cleanup() {
  cleanup
  exit 1
}
<<<<<<< HEAD
trap trap_cleanup EXIT TERM INT

test_per_thread() {
  echo "Basic --per-thread mode test"
  if ! perf record -o /dev/null --quiet ${testprog} 2> /dev/null
  then
    echo "Per-thread record [Skipped event not supported]"
    return
  fi
  if ! perf record --per-thread -o "${perfdata}" ${testprog} 2> /dev/null
  then
    echo "Per-thread record [Failed record]"
    err=1
    return
  fi
  if ! perf report -i "${perfdata}" -q | grep -q "${testsym}"
=======
trap trap_cleanup exit term int

test_per_thread() {
  echo "Basic --per-thread mode test"
  if ! perf record -e instructions:u -o ${perfdata} --quiet true 2> /dev/null
  then
    echo "Per-thread record [Skipped instructions:u not supported]"
    if [ $err -ne 1 ]
    then
      err=2
    fi
    return
  fi
  if ! perf record -e instructions:u --per-thread -o ${perfdata} true 2> /dev/null
  then
    echo "Per-thread record of instructions:u [Failed]"
    err=1
    return
  fi
  if ! perf report -i ${perfdata} -q | egrep -q true
>>>>>>> b7ba80a49124 (Commit)
  then
    echo "Per-thread record [Failed missing output]"
    err=1
    return
  fi
<<<<<<< HEAD

  # run the test program in background (for 30 seconds)
  ${testprog} 30 &
  TESTPID=$!

  rm -f "${perfdata}"

  wait_for_threads ${TESTPID} 2
  perf record -p "${TESTPID}" --per-thread -o "${perfdata}" sleep 1 2> /dev/null
  kill ${TESTPID}

  if [ ! -e "${perfdata}" ]
  then
    echo "Per-thread record [Failed record -p]"
    err=1
    return
  fi
  if ! perf report -i "${perfdata}" -q | grep -q "${testsym}"
  then
    echo "Per-thread record [Failed -p missing output]"
    err=1
    return
  fi

=======
>>>>>>> b7ba80a49124 (Commit)
  echo "Basic --per-thread mode test [Success]"
}

test_register_capture() {
  echo "Register capture test"
<<<<<<< HEAD
  if ! perf list | grep -q 'br_inst_retired.near_call'
  then
    echo "Register capture test [Skipped missing event]"
    return
  fi
  if ! perf record --intr-regs=\? 2>&1 | grep -q 'available registers: AX BX CX DX SI DI BP SP IP FLAGS CS SS R8 R9 R10 R11 R12 R13 R14 R15'
=======
  if ! perf list | egrep -q 'br_inst_retired.near_call'
  then
    echo "Register capture test [Skipped missing instruction]"
    if [ $err -ne 1 ]
    then
      err=2
    fi
    return
  fi
  if ! perf record --intr-regs=\? 2>&1 | egrep -q 'available registers: AX BX CX DX SI DI BP SP IP FLAGS CS SS R8 R9 R10 R11 R12 R13 R14 R15'
>>>>>>> b7ba80a49124 (Commit)
  then
    echo "Register capture test [Skipped missing registers]"
    return
  fi
<<<<<<< HEAD
  if ! perf record -o - --intr-regs=di,r8,dx,cx -e br_inst_retired.near_call \
    -c 1000 --per-thread ${testprog} 2> /dev/null \
    | perf script -F ip,sym,iregs -i - 2> /dev/null \
    | grep -q "DI:"
=======
  if ! perf record -o - --intr-regs=di,r8,dx,cx -e cpu/br_inst_retired.near_call/p \
    -c 1000 --per-thread true 2> /dev/null \
    | perf script -F ip,sym,iregs -i - 2> /dev/null \
    | egrep -q "DI:"
>>>>>>> b7ba80a49124 (Commit)
  then
    echo "Register capture test [Failed missing output]"
    err=1
    return
  fi
  echo "Register capture test [Success]"
}

<<<<<<< HEAD
test_system_wide() {
  echo "Basic --system-wide mode test"
  if ! perf record -aB --synth=no -o "${perfdata}" ${testprog} 2> /dev/null
  then
    echo "System-wide record [Skipped not supported]"
    return
  fi
  if ! perf report -i "${perfdata}" -q | grep -q "${testsym}"
  then
    echo "System-wide record [Failed missing output]"
    err=1
    return
  fi
  if ! perf record -aB --synth=no -e cpu-clock,cs --threads=cpu \
    -o "${perfdata}" ${testprog} 2> /dev/null
  then
    echo "System-wide record [Failed record --threads option]"
    err=1
    return
  fi
  if ! perf report -i "${perfdata}" -q | grep -q "${testsym}"
  then
    echo "System-wide record [Failed --threads missing output]"
    err=1
    return
  fi
  echo "Basic --system-wide mode test [Success]"
}

test_workload() {
  echo "Basic target workload test"
  if ! perf record -o "${perfdata}" ${testprog} 2> /dev/null
  then
    echo "Workload record [Failed record]"
    err=1
    return
  fi
  if ! perf report -i "${perfdata}" -q | grep -q "${testsym}"
  then
    echo "Workload record [Failed missing output]"
    err=1
    return
  fi
  if ! perf record -e cpu-clock,cs --threads=package \
    -o "${perfdata}" ${testprog} 2> /dev/null
  then
    echo "Workload record [Failed record --threads option]"
    err=1
    return
  fi
  if ! perf report -i "${perfdata}" -q | grep -q "${testsym}"
  then
    echo "Workload record [Failed --threads missing output]"
    err=1
    return
  fi
  echo "Basic target workload test [Success]"
}

test_per_thread
test_register_capture
test_system_wide
test_workload
=======
test_per_thread
test_register_capture
>>>>>>> b7ba80a49124 (Commit)

cleanup
exit $err
