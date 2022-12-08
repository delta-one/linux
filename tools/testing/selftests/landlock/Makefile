# SPDX-License-Identifier: GPL-2.0
#
# First run: make -C ../../../.. headers_install

CFLAGS += -Wall -O2 $(KHDR_INCLUDES)
<<<<<<< HEAD
=======
LDLIBS += -lcap
>>>>>>> b7ba80a49124 (Commit)

LOCAL_HDRS += common.h

src_test := $(wildcard *_test.c)

TEST_GEN_PROGS := $(src_test:.c=)

TEST_GEN_PROGS_EXTENDED := true

<<<<<<< HEAD
# Short targets:
$(TEST_GEN_PROGS): LDLIBS += -lcap
=======
# Static linking for short targets:
>>>>>>> b7ba80a49124 (Commit)
$(TEST_GEN_PROGS_EXTENDED): LDFLAGS += -static

include ../lib.mk

<<<<<<< HEAD
# Targets with $(OUTPUT)/ prefix:
$(TEST_GEN_PROGS): LDLIBS += -lcap
=======
# Static linking for targets with $(OUTPUT)/ prefix:
>>>>>>> b7ba80a49124 (Commit)
$(TEST_GEN_PROGS_EXTENDED): LDFLAGS += -static
