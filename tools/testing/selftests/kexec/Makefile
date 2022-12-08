# SPDX-License-Identifier: GPL-2.0-only
# Makefile for kexec tests

<<<<<<< HEAD
ARCH ?= $(shell uname -m 2>/dev/null || echo not)
ARCH_PROCESSED := $(shell echo $(ARCH) | sed -e s/i.86/x86/ -e s/x86_64/x86/)

ifeq ($(ARCH_PROCESSED),$(filter $(ARCH_PROCESSED),x86 ppc64le))
=======
uname_M := $(shell uname -m 2>/dev/null || echo not)
ARCH ?= $(shell echo $(uname_M) | sed -e s/i.86/x86/ -e s/x86_64/x86/)

ifeq ($(ARCH),$(filter $(ARCH),x86 ppc64le))
>>>>>>> b7ba80a49124 (Commit)
TEST_PROGS := test_kexec_load.sh test_kexec_file_load.sh
TEST_FILES := kexec_common_lib.sh

include ../lib.mk

endif
