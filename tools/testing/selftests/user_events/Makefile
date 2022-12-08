# SPDX-License-Identifier: GPL-2.0
<<<<<<< HEAD
CFLAGS += -Wl,-no-as-needed -Wall $(KHDR_INCLUDES)
LDLIBS += -lrt -lpthread -lm

# Note:
# This test depends on <linux/user_events.h> exported in uapi
# The following commit removed user_events.h out of uapi:
# commit 5cfff569cab8bf544bab62c911c5d6efd5af5e05
# tracing: Move user_events.h temporarily out of include/uapi
# This test will not compile until user_events.h is added
# back to uapi.

=======
CFLAGS += -Wl,-no-as-needed -Wall -I../../../../usr/include
LDLIBS += -lrt -lpthread -lm

>>>>>>> b7ba80a49124 (Commit)
TEST_GEN_PROGS = ftrace_test dyn_test perf_test

TEST_FILES := settings

include ../lib.mk
