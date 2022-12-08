# SPDX-License-Identifier: GPL-2.0-only
include ../../scripts/Makefile.include
include ../../scripts/Makefile.arch

srctree := $(abspath $(CURDIR)/../../../)

ifeq ($(V),1)
  Q =
  msg =
else
  Q = @
  ifeq ($(silent),1)
    msg =
  else
    msg = @printf '  %-8s %s%s\n' "$(1)" "$(notdir $(2))" "$(if $(3), $(3))";
  endif
  MAKEFLAGS=--no-print-directory
endif

<<<<<<< HEAD
# Overrides for the prepare step libraries.
HOST_OVERRIDES := AR="$(HOSTAR)" CC="$(HOSTCC)" LD="$(HOSTLD)" ARCH="$(HOSTARCH)" \
		  CROSS_COMPILE="" EXTRA_CFLAGS="$(HOSTCFLAGS)"

RM      ?= rm
HOSTCC  ?= gcc
HOSTLD  ?= ld
HOSTAR  ?= ar
CROSS_COMPILE =
=======
# always use the host compiler
AR       = $(HOSTAR)
CC       = $(HOSTCC)
LD       = $(HOSTLD)
ARCH     = $(HOSTARCH)
RM      ?= rm
CROSS_COMPILE =
CFLAGS  := $(KBUILD_HOSTCFLAGS)
LDFLAGS := $(KBUILD_HOSTLDFLAGS)
>>>>>>> b7ba80a49124 (Commit)

OUTPUT ?= $(srctree)/tools/bpf/resolve_btfids/

LIBBPF_SRC := $(srctree)/tools/lib/bpf/
SUBCMD_SRC := $(srctree)/tools/lib/subcmd/

BPFOBJ     := $(OUTPUT)/libbpf/libbpf.a
LIBBPF_OUT := $(abspath $(dir $(BPFOBJ)))/
SUBCMDOBJ  := $(OUTPUT)/libsubcmd/libsubcmd.a
<<<<<<< HEAD
SUBCMD_OUT := $(abspath $(dir $(SUBCMDOBJ)))/
=======
>>>>>>> b7ba80a49124 (Commit)

LIBBPF_DESTDIR := $(LIBBPF_OUT)
LIBBPF_INCLUDE := $(LIBBPF_DESTDIR)include

<<<<<<< HEAD
SUBCMD_DESTDIR := $(SUBCMD_OUT)
SUBCMD_INCLUDE := $(SUBCMD_DESTDIR)include

=======
>>>>>>> b7ba80a49124 (Commit)
BINARY     := $(OUTPUT)/resolve_btfids
BINARY_IN  := $(BINARY)-in.o

all: $(BINARY)

<<<<<<< HEAD
prepare: $(BPFOBJ) $(SUBCMDOBJ)

=======
>>>>>>> b7ba80a49124 (Commit)
$(OUTPUT) $(OUTPUT)/libsubcmd $(LIBBPF_OUT):
	$(call msg,MKDIR,,$@)
	$(Q)mkdir -p $(@)

$(SUBCMDOBJ): fixdep FORCE | $(OUTPUT)/libsubcmd
<<<<<<< HEAD
	$(Q)$(MAKE) -C $(SUBCMD_SRC) OUTPUT=$(SUBCMD_OUT) \
		    DESTDIR=$(SUBCMD_DESTDIR) $(HOST_OVERRIDES) prefix= subdir= \
		    $(abspath $@) install_headers

$(BPFOBJ): $(wildcard $(LIBBPF_SRC)/*.[ch] $(LIBBPF_SRC)/Makefile) | $(LIBBPF_OUT)
	$(Q)$(MAKE) $(submake_extras) -C $(LIBBPF_SRC) OUTPUT=$(LIBBPF_OUT)    \
		    DESTDIR=$(LIBBPF_DESTDIR) $(HOST_OVERRIDES) prefix= subdir= \
		    $(abspath $@) install_headers

LIBELF_FLAGS := $(shell $(HOSTPKG_CONFIG) libelf --cflags 2>/dev/null)
LIBELF_LIBS  := $(shell $(HOSTPKG_CONFIG) libelf --libs 2>/dev/null || echo -lelf)

HOSTCFLAGS += -g \
          -I$(srctree)/tools/include \
          -I$(srctree)/tools/include/uapi \
          -I$(LIBBPF_INCLUDE) \
          -I$(SUBCMD_INCLUDE) \
          $(LIBELF_FLAGS)

LIBS = $(LIBELF_LIBS) -lz

export srctree OUTPUT HOSTCFLAGS Q HOSTCC HOSTLD HOSTAR
include $(srctree)/tools/build/Makefile.include

$(BINARY_IN): fixdep FORCE prepare | $(OUTPUT)
=======
	$(Q)$(MAKE) -C $(SUBCMD_SRC) OUTPUT=$(abspath $(dir $@))/ $(abspath $@)

$(BPFOBJ): $(wildcard $(LIBBPF_SRC)/*.[ch] $(LIBBPF_SRC)/Makefile) | $(LIBBPF_OUT)
	$(Q)$(MAKE) $(submake_extras) -C $(LIBBPF_SRC) OUTPUT=$(LIBBPF_OUT)    \
		    DESTDIR=$(LIBBPF_DESTDIR) prefix= EXTRA_CFLAGS="$(CFLAGS)" \
		    $(abspath $@) install_headers

CFLAGS += -g \
          -I$(srctree)/tools/include \
          -I$(srctree)/tools/include/uapi \
          -I$(LIBBPF_INCLUDE) \
          -I$(SUBCMD_SRC)

LIBS = -lelf -lz

export srctree OUTPUT CFLAGS Q
include $(srctree)/tools/build/Makefile.include

$(BINARY_IN): $(BPFOBJ) fixdep FORCE | $(OUTPUT)
>>>>>>> b7ba80a49124 (Commit)
	$(Q)$(MAKE) $(build)=resolve_btfids

$(BINARY): $(BPFOBJ) $(SUBCMDOBJ) $(BINARY_IN)
	$(call msg,LINK,$@)
<<<<<<< HEAD
	$(Q)$(HOSTCC) $(BINARY_IN) $(KBUILD_HOSTLDFLAGS) -o $@ $(BPFOBJ) $(SUBCMDOBJ) $(LIBS)
=======
	$(Q)$(CC) $(BINARY_IN) $(LDFLAGS) -o $@ $(BPFOBJ) $(SUBCMDOBJ) $(LIBS)
>>>>>>> b7ba80a49124 (Commit)

clean_objects := $(wildcard $(OUTPUT)/*.o                \
                            $(OUTPUT)/.*.o.cmd           \
                            $(OUTPUT)/.*.o.d             \
                            $(LIBBPF_OUT)                \
                            $(LIBBPF_DESTDIR)            \
<<<<<<< HEAD
                            $(SUBCMD_OUT)                \
                            $(SUBCMD_DESTDIR)            \
=======
                            $(OUTPUT)/libsubcmd          \
>>>>>>> b7ba80a49124 (Commit)
                            $(OUTPUT)/resolve_btfids)

ifneq ($(clean_objects),)
clean: fixdep-clean
	$(call msg,CLEAN,$(BINARY))
	$(Q)$(RM) -rf $(clean_objects)
else
clean:
endif

tags:
	$(call msg,GEN,,tags)
	$(Q)ctags -R . $(LIBBPF_SRC) $(SUBCMD_SRC)

FORCE:

<<<<<<< HEAD
.PHONY: all FORCE clean tags prepare
=======
.PHONY: all FORCE clean tags
>>>>>>> b7ba80a49124 (Commit)
