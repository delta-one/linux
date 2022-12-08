# SPDX-License-Identifier: GPL-2.0
CFLAGS := $(CFLAGS) -Wall -D_GNU_SOURCE
LDLIBS += -lm

<<<<<<< HEAD
ARCH ?= $(shell uname -m 2>/dev/null || echo not)
ARCH_PROCESSED := $(shell echo $(ARCH) | sed -e s/i.86/x86/ -e s/x86_64/x86/)

ifeq (x86,$(ARCH_PROCESSED))
=======
uname_M := $(shell uname -m 2>/dev/null || echo not)
ARCH ?= $(shell echo $(uname_M) | sed -e s/i.86/x86/ -e s/x86_64/x86/)

ifeq (x86,$(ARCH))
>>>>>>> b7ba80a49124 (Commit)
TEST_GEN_FILES := msr aperf
endif

TEST_PROGS := run.sh

include ../lib.mk

$(TEST_GEN_FILES): $(HEADERS)
