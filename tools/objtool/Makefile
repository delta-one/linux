# SPDX-License-Identifier: GPL-2.0
include ../scripts/Makefile.include
include ../scripts/Makefile.arch

<<<<<<< HEAD
=======
# always use the host compiler
AR	 = $(HOSTAR)
CC	 = $(HOSTCC)
LD	 = $(HOSTLD)

>>>>>>> b7ba80a49124 (Commit)
ifeq ($(srctree),)
srctree := $(patsubst %/,%,$(dir $(CURDIR)))
srctree := $(patsubst %/,%,$(dir $(srctree)))
endif

<<<<<<< HEAD
LIBSUBCMD_DIR = $(srctree)/tools/lib/subcmd/
ifneq ($(OUTPUT),)
  LIBSUBCMD_OUTPUT = $(abspath $(OUTPUT))/libsubcmd
else
  LIBSUBCMD_OUTPUT = $(CURDIR)/libsubcmd
endif
LIBSUBCMD = $(LIBSUBCMD_OUTPUT)/libsubcmd.a
=======
SUBCMD_SRCDIR		= $(srctree)/tools/lib/subcmd/
LIBSUBCMD_OUTPUT	= $(or $(OUTPUT),$(CURDIR)/)
LIBSUBCMD		= $(LIBSUBCMD_OUTPUT)libsubcmd.a
>>>>>>> b7ba80a49124 (Commit)

OBJTOOL    := $(OUTPUT)objtool
OBJTOOL_IN := $(OBJTOOL)-in.o

LIBELF_FLAGS := $(shell $(HOSTPKG_CONFIG) libelf --cflags 2>/dev/null)
LIBELF_LIBS  := $(shell $(HOSTPKG_CONFIG) libelf --libs 2>/dev/null || echo -lelf)

all: $(OBJTOOL)

INCLUDES := -I$(srctree)/tools/include \
	    -I$(srctree)/tools/arch/$(HOSTARCH)/include/uapi \
	    -I$(srctree)/tools/arch/$(SRCARCH)/include	\
	    -I$(srctree)/tools/objtool/include \
<<<<<<< HEAD
	    -I$(srctree)/tools/objtool/arch/$(SRCARCH)/include \
	    -I$(LIBSUBCMD_OUTPUT)/include
# Note, EXTRA_WARNINGS here was determined for CC and not HOSTCC, it
# is passed here to match a legacy behavior.
WARNINGS := $(EXTRA_WARNINGS) -Wno-switch-default -Wno-switch-enum -Wno-packed -Wno-nested-externs
OBJTOOL_CFLAGS := -Werror $(WARNINGS) $(KBUILD_HOSTCFLAGS) -g $(INCLUDES) $(LIBELF_FLAGS)
OBJTOOL_LDFLAGS := $(LIBELF_LIBS) $(LIBSUBCMD) $(KBUILD_HOSTLDFLAGS)

# Allow old libelf to be used:
elfshdr := $(shell echo '$(pound)include <libelf.h>' | $(HOSTCC) $(OBJTOOL_CFLAGS) -x c -E - | grep elf_getshdr)
OBJTOOL_CFLAGS += $(if $(elfshdr),,-DLIBELF_USE_DEPRECATED)

# Always want host compilation.
HOST_OVERRIDES := CC="$(HOSTCC)" LD="$(HOSTLD)" AR="$(HOSTAR)"

AWK = awk
MKDIR = mkdir

ifeq ($(V),1)
  Q =
else
  Q = @
endif
=======
	    -I$(srctree)/tools/objtool/arch/$(SRCARCH)/include
WARNINGS := $(EXTRA_WARNINGS) -Wno-switch-default -Wno-switch-enum -Wno-packed -Wno-nested-externs
CFLAGS   := -Werror $(WARNINGS) $(KBUILD_HOSTCFLAGS) -g $(INCLUDES) $(LIBELF_FLAGS)
LDFLAGS  += $(LIBELF_LIBS) $(LIBSUBCMD) $(KBUILD_HOSTLDFLAGS)

# Allow old libelf to be used:
elfshdr := $(shell echo '$(pound)include <libelf.h>' | $(CC) $(CFLAGS) -x c -E - | grep elf_getshdr)
CFLAGS += $(if $(elfshdr),,-DLIBELF_USE_DEPRECATED)

AWK = awk
>>>>>>> b7ba80a49124 (Commit)

BUILD_ORC := n

ifeq ($(SRCARCH),x86)
	BUILD_ORC := y
endif

export BUILD_ORC
export srctree OUTPUT CFLAGS SRCARCH AWK
include $(srctree)/tools/build/Makefile.include

<<<<<<< HEAD
$(OBJTOOL_IN): fixdep $(LIBSUBCMD) FORCE
	$(Q)$(CONFIG_SHELL) ./sync-check.sh
	$(Q)$(MAKE) $(build)=objtool $(HOST_OVERRIDES) CFLAGS="$(OBJTOOL_CFLAGS)" \
		LDFLAGS="$(OBJTOOL_LDFLAGS)"


$(OBJTOOL): $(LIBSUBCMD) $(OBJTOOL_IN)
	$(QUIET_LINK)$(HOSTCC) $(OBJTOOL_IN) $(OBJTOOL_LDFLAGS) -o $@


$(LIBSUBCMD_OUTPUT):
	$(Q)$(MKDIR) -p $@

$(LIBSUBCMD): fixdep $(LIBSUBCMD_OUTPUT) FORCE
	$(Q)$(MAKE) -C $(LIBSUBCMD_DIR) O=$(LIBSUBCMD_OUTPUT) \
		DESTDIR=$(LIBSUBCMD_OUTPUT) prefix= subdir= \
		$(HOST_OVERRIDES) EXTRA_CFLAGS="$(OBJTOOL_CFLAGS)" \
		$@ install_headers

$(LIBSUBCMD)-clean:
	$(call QUIET_CLEAN, libsubcmd)
	$(Q)$(RM) -r -- $(LIBSUBCMD_OUTPUT)

clean: $(LIBSUBCMD)-clean
	$(call QUIET_CLEAN, objtool) $(RM) $(OBJTOOL)
	$(Q)find $(OUTPUT) -name '*.o' -delete -o -name '\.*.cmd' -delete -o -name '\.*.d' -delete
	$(Q)$(RM) $(OUTPUT)arch/x86/lib/inat-tables.c $(OUTPUT)fixdep
=======
$(OBJTOOL_IN): fixdep FORCE
	@$(CONFIG_SHELL) ./sync-check.sh
	@$(MAKE) $(build)=objtool

$(OBJTOOL): $(LIBSUBCMD) $(OBJTOOL_IN)
	$(QUIET_LINK)$(CC) $(OBJTOOL_IN) $(LDFLAGS) -o $@


$(LIBSUBCMD): fixdep FORCE
	$(Q)$(MAKE) -C $(SUBCMD_SRCDIR) OUTPUT=$(LIBSUBCMD_OUTPUT)

clean:
	$(call QUIET_CLEAN, objtool) $(RM) $(OBJTOOL)
	$(Q)find $(OUTPUT) -name '*.o' -delete -o -name '\.*.cmd' -delete -o -name '\.*.d' -delete
	$(Q)$(RM) $(OUTPUT)arch/x86/lib/inat-tables.c $(OUTPUT)fixdep $(LIBSUBCMD)
>>>>>>> b7ba80a49124 (Commit)

FORCE:

.PHONY: clean FORCE
