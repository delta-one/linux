/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _OBJTOOL_ENDIANNESS_H
#define _OBJTOOL_ENDIANNESS_H

<<<<<<< HEAD
#include <linux/kernel.h>
#include <endian.h>
#include <objtool/elf.h>

/*
 * Does a byte swap if target file endianness doesn't match the host, i.e. cross
=======
#include <arch/endianness.h>
#include <linux/kernel.h>
#include <endian.h>

#ifndef __TARGET_BYTE_ORDER
#error undefined arch __TARGET_BYTE_ORDER
#endif

#if __BYTE_ORDER != __TARGET_BYTE_ORDER
#define __NEED_BSWAP 1
#else
#define __NEED_BSWAP 0
#endif

/*
 * Does a byte swap if target endianness doesn't match the host, i.e. cross
>>>>>>> b7ba80a49124 (Commit)
 * compilation for little endian on big endian and vice versa.
 * To be used for multi-byte values conversion, which are read from / about
 * to be written to a target native endianness ELF file.
 */
<<<<<<< HEAD
static inline bool need_bswap(struct elf *elf)
{
	return (__BYTE_ORDER == __LITTLE_ENDIAN) ^
	       (elf->ehdr.e_ident[EI_DATA] == ELFDATA2LSB);
}

#define bswap_if_needed(elf, val)					\
({									\
	__typeof__(val) __ret;						\
	bool __need_bswap = need_bswap(elf);				\
	switch (sizeof(val)) {						\
	case 8:								\
		__ret = __need_bswap ? bswap_64(val) : (val); break;	\
	case 4:								\
		__ret = __need_bswap ? bswap_32(val) : (val); break;	\
	case 2:								\
		__ret = __need_bswap ? bswap_16(val) : (val); break;	\
=======
#define bswap_if_needed(val)						\
({									\
	__typeof__(val) __ret;						\
	switch (sizeof(val)) {						\
	case 8: __ret = __NEED_BSWAP ? bswap_64(val) : (val); break;	\
	case 4: __ret = __NEED_BSWAP ? bswap_32(val) : (val); break;	\
	case 2: __ret = __NEED_BSWAP ? bswap_16(val) : (val); break;	\
>>>>>>> b7ba80a49124 (Commit)
	default:							\
		BUILD_BUG(); break;					\
	}								\
	__ret;								\
})

#endif /* _OBJTOOL_ENDIANNESS_H */
