// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copied from arch/arm64/kernel/cpufeature.c
 *
 * Copyright (C) 2015 ARM Ltd.
 * Copyright (C) 2017 SiFive
 */

#include <linux/bitmap.h>
#include <linux/ctype.h>
<<<<<<< HEAD
#include <linux/log2.h>
#include <linux/memory.h>
=======
#include <linux/libfdt.h>
>>>>>>> b7ba80a49124 (Commit)
#include <linux/module.h>
#include <linux/of.h>
#include <asm/alternative.h>
#include <asm/cacheflush.h>
<<<<<<< HEAD
#include <asm/hwcap.h>
#include <asm/patch.h>
#include <asm/processor.h>
=======
#include <asm/errata_list.h>
#include <asm/hwcap.h>
#include <asm/patch.h>
#include <asm/pgtable.h>
#include <asm/processor.h>
#include <asm/smp.h>
#include <asm/switch_to.h>
>>>>>>> b7ba80a49124 (Commit)

#define NUM_ALPHA_EXTS ('z' - 'a' + 1)

unsigned long elf_hwcap __read_mostly;

/* Host ISA bitmap */
static DECLARE_BITMAP(riscv_isa, RISCV_ISA_EXT_MAX) __read_mostly;

<<<<<<< HEAD
=======
DEFINE_STATIC_KEY_ARRAY_FALSE(riscv_isa_ext_keys, RISCV_ISA_EXT_KEY_MAX);
EXPORT_SYMBOL(riscv_isa_ext_keys);

>>>>>>> b7ba80a49124 (Commit)
/**
 * riscv_isa_extension_base() - Get base extension word
 *
 * @isa_bitmap: ISA bitmap to use
 * Return: base extension word as unsigned long value
 *
 * NOTE: If isa_bitmap is NULL then Host ISA bitmap will be used.
 */
unsigned long riscv_isa_extension_base(const unsigned long *isa_bitmap)
{
	if (!isa_bitmap)
		return riscv_isa[0];
	return isa_bitmap[0];
}
EXPORT_SYMBOL_GPL(riscv_isa_extension_base);

/**
 * __riscv_isa_extension_available() - Check whether given extension
 * is available or not
 *
 * @isa_bitmap: ISA bitmap to use
 * @bit: bit position of the desired extension
 * Return: true or false
 *
 * NOTE: If isa_bitmap is NULL then Host ISA bitmap will be used.
 */
bool __riscv_isa_extension_available(const unsigned long *isa_bitmap, int bit)
{
	const unsigned long *bmap = (isa_bitmap) ? isa_bitmap : riscv_isa;

	if (bit >= RISCV_ISA_EXT_MAX)
		return false;

	return test_bit(bit, bmap) ? true : false;
}
EXPORT_SYMBOL_GPL(__riscv_isa_extension_available);

<<<<<<< HEAD
static bool riscv_isa_extension_check(int id)
{
	switch (id) {
	case RISCV_ISA_EXT_ZICBOM:
		if (!riscv_cbom_block_size) {
			pr_err("Zicbom detected in ISA string, but no cbom-block-size found\n");
			return false;
		} else if (!is_power_of_2(riscv_cbom_block_size)) {
			pr_err("cbom-block-size present, but is not a power-of-2\n");
			return false;
		}
		return true;
	case RISCV_ISA_EXT_ZICBOZ:
		if (!riscv_cboz_block_size) {
			pr_err("Zicboz detected in ISA string, but no cboz-block-size found\n");
			return false;
		} else if (!is_power_of_2(riscv_cboz_block_size)) {
			pr_err("cboz-block-size present, but is not a power-of-2\n");
			return false;
		}
		return true;
	}

	return true;
}

=======
>>>>>>> b7ba80a49124 (Commit)
void __init riscv_fill_hwcap(void)
{
	struct device_node *node;
	const char *isa;
	char print_str[NUM_ALPHA_EXTS + 1];
	int i, j, rc;
<<<<<<< HEAD
	unsigned long isa2hwcap[26] = {0};
	unsigned long hartid;

	isa2hwcap['i' - 'a'] = COMPAT_HWCAP_ISA_I;
	isa2hwcap['m' - 'a'] = COMPAT_HWCAP_ISA_M;
	isa2hwcap['a' - 'a'] = COMPAT_HWCAP_ISA_A;
	isa2hwcap['f' - 'a'] = COMPAT_HWCAP_ISA_F;
	isa2hwcap['d' - 'a'] = COMPAT_HWCAP_ISA_D;
	isa2hwcap['c' - 'a'] = COMPAT_HWCAP_ISA_C;
=======
	static unsigned long isa2hwcap[256] = {0};
	unsigned long hartid;

	isa2hwcap['i'] = isa2hwcap['I'] = COMPAT_HWCAP_ISA_I;
	isa2hwcap['m'] = isa2hwcap['M'] = COMPAT_HWCAP_ISA_M;
	isa2hwcap['a'] = isa2hwcap['A'] = COMPAT_HWCAP_ISA_A;
	isa2hwcap['f'] = isa2hwcap['F'] = COMPAT_HWCAP_ISA_F;
	isa2hwcap['d'] = isa2hwcap['D'] = COMPAT_HWCAP_ISA_D;
	isa2hwcap['c'] = isa2hwcap['C'] = COMPAT_HWCAP_ISA_C;
>>>>>>> b7ba80a49124 (Commit)

	elf_hwcap = 0;

	bitmap_zero(riscv_isa, RISCV_ISA_EXT_MAX);

	for_each_of_cpu_node(node) {
		unsigned long this_hwcap = 0;
		DECLARE_BITMAP(this_isa, RISCV_ISA_EXT_MAX);
		const char *temp;

		rc = riscv_of_processor_hartid(node, &hartid);
		if (rc < 0)
			continue;

		if (of_property_read_string(node, "riscv,isa", &isa)) {
			pr_warn("Unable to find \"riscv,isa\" devicetree entry\n");
			continue;
		}

		temp = isa;
#if IS_ENABLED(CONFIG_32BIT)
		if (!strncmp(isa, "rv32", 4))
			isa += 4;
#elif IS_ENABLED(CONFIG_64BIT)
		if (!strncmp(isa, "rv64", 4))
			isa += 4;
#endif
		/* The riscv,isa DT property must start with rv64 or rv32 */
		if (temp == isa)
			continue;
		bitmap_zero(this_isa, RISCV_ISA_EXT_MAX);
		for (; *isa; ++isa) {
			const char *ext = isa++;
			const char *ext_end = isa;
			bool ext_long = false, ext_err = false;

			switch (*ext) {
			case 's':
				/**
				 * Workaround for invalid single-letter 's' & 'u'(QEMU).
				 * No need to set the bit in riscv_isa as 's' & 'u' are
				 * not valid ISA extensions. It works until multi-letter
				 * extension starting with "Su" appears.
				 */
				if (ext[-1] != '_' && ext[1] == 'u') {
					++isa;
					ext_err = true;
					break;
				}
				fallthrough;
			case 'x':
			case 'z':
				ext_long = true;
				/* Multi-letter extension must be delimited */
				for (; *isa && *isa != '_'; ++isa)
					if (unlikely(!islower(*isa)
						     && !isdigit(*isa)))
						ext_err = true;
				/* Parse backwards */
				ext_end = isa;
				if (unlikely(ext_err))
					break;
				if (!isdigit(ext_end[-1]))
					break;
				/* Skip the minor version */
				while (isdigit(*--ext_end))
					;
				if (ext_end[0] != 'p'
				    || !isdigit(ext_end[-1])) {
					/* Advance it to offset the pre-decrement */
					++ext_end;
					break;
				}
				/* Skip the major version */
				while (isdigit(*--ext_end))
					;
				++ext_end;
				break;
			default:
				if (unlikely(!islower(*ext))) {
					ext_err = true;
					break;
				}
				/* Find next extension */
				if (!isdigit(*isa))
					break;
				/* Skip the minor version */
				while (isdigit(*++isa))
					;
				if (*isa != 'p')
					break;
				if (!isdigit(*++isa)) {
					--isa;
					break;
				}
				/* Skip the major version */
				while (isdigit(*++isa))
					;
				break;
			}
			if (*isa != '_')
				--isa;

#define SET_ISA_EXT_MAP(name, bit)						\
			do {							\
				if ((ext_end - ext == sizeof(name) - 1) &&	\
<<<<<<< HEAD
				     !memcmp(ext, name, sizeof(name) - 1) &&	\
				     riscv_isa_extension_check(bit))		\
=======
				     !memcmp(ext, name, sizeof(name) - 1))	\
>>>>>>> b7ba80a49124 (Commit)
					set_bit(bit, this_isa);			\
			} while (false)						\

			if (unlikely(ext_err))
				continue;
			if (!ext_long) {
<<<<<<< HEAD
				int nr = *ext - 'a';

				if (riscv_isa_extension_check(nr)) {
					this_hwcap |= isa2hwcap[nr];
					set_bit(nr, this_isa);
				}
			} else {
				/* sorted alphabetically */
				SET_ISA_EXT_MAP("sscofpmf", RISCV_ISA_EXT_SSCOFPMF);
				SET_ISA_EXT_MAP("sstc", RISCV_ISA_EXT_SSTC);
				SET_ISA_EXT_MAP("svinval", RISCV_ISA_EXT_SVINVAL);
				SET_ISA_EXT_MAP("svnapot", RISCV_ISA_EXT_SVNAPOT);
				SET_ISA_EXT_MAP("svpbmt", RISCV_ISA_EXT_SVPBMT);
				SET_ISA_EXT_MAP("zbb", RISCV_ISA_EXT_ZBB);
				SET_ISA_EXT_MAP("zicbom", RISCV_ISA_EXT_ZICBOM);
				SET_ISA_EXT_MAP("zicboz", RISCV_ISA_EXT_ZICBOZ);
				SET_ISA_EXT_MAP("zihintpause", RISCV_ISA_EXT_ZIHINTPAUSE);
=======
				this_hwcap |= isa2hwcap[(unsigned char)(*ext)];
				set_bit(*ext - 'a', this_isa);
			} else {
				SET_ISA_EXT_MAP("sscofpmf", RISCV_ISA_EXT_SSCOFPMF);
				SET_ISA_EXT_MAP("svpbmt", RISCV_ISA_EXT_SVPBMT);
				SET_ISA_EXT_MAP("zicbom", RISCV_ISA_EXT_ZICBOM);
				SET_ISA_EXT_MAP("zihintpause", RISCV_ISA_EXT_ZIHINTPAUSE);
				SET_ISA_EXT_MAP("sstc", RISCV_ISA_EXT_SSTC);
>>>>>>> b7ba80a49124 (Commit)
			}
#undef SET_ISA_EXT_MAP
		}

		/*
		 * All "okay" hart should have same isa. Set HWCAP based on
		 * common capabilities of every "okay" hart, in case they don't
		 * have.
		 */
		if (elf_hwcap)
			elf_hwcap &= this_hwcap;
		else
			elf_hwcap = this_hwcap;

		if (bitmap_empty(riscv_isa, RISCV_ISA_EXT_MAX))
			bitmap_copy(riscv_isa, this_isa, RISCV_ISA_EXT_MAX);
		else
			bitmap_and(riscv_isa, riscv_isa, this_isa, RISCV_ISA_EXT_MAX);
	}

	/* We don't support systems with F but without D, so mask those out
	 * here. */
	if ((elf_hwcap & COMPAT_HWCAP_ISA_F) && !(elf_hwcap & COMPAT_HWCAP_ISA_D)) {
		pr_info("This kernel does not support systems with F but not D\n");
		elf_hwcap &= ~COMPAT_HWCAP_ISA_F;
	}

	memset(print_str, 0, sizeof(print_str));
	for (i = 0, j = 0; i < NUM_ALPHA_EXTS; i++)
		if (riscv_isa[0] & BIT_MASK(i))
			print_str[j++] = (char)('a' + i);
	pr_info("riscv: base ISA extensions %s\n", print_str);

	memset(print_str, 0, sizeof(print_str));
	for (i = 0, j = 0; i < NUM_ALPHA_EXTS; i++)
		if (elf_hwcap & BIT_MASK(i))
			print_str[j++] = (char)('a' + i);
	pr_info("riscv: ELF capabilities %s\n", print_str);
<<<<<<< HEAD
}

#ifdef CONFIG_RISCV_ALTERNATIVE
/*
 * Alternative patch sites consider 48 bits when determining when to patch
 * the old instruction sequence with the new. These bits are broken into a
 * 16-bit vendor ID and a 32-bit patch ID. A non-zero vendor ID means the
 * patch site is for an erratum, identified by the 32-bit patch ID. When
 * the vendor ID is zero, the patch site is for a cpufeature. cpufeatures
 * further break down patch ID into two 16-bit numbers. The lower 16 bits
 * are the cpufeature ID and the upper 16 bits are used for a value specific
 * to the cpufeature and patch site. If the upper 16 bits are zero, then it
 * implies no specific value is specified. cpufeatures that want to control
 * patching on a per-site basis will provide non-zero values and implement
 * checks here. The checks return true when patching should be done, and
 * false otherwise.
 */
static bool riscv_cpufeature_patch_check(u16 id, u16 value)
{
	if (!value)
		return true;

	switch (id) {
	case RISCV_ISA_EXT_ZICBOZ:
		/*
		 * Zicboz alternative applications provide the maximum
		 * supported block size order, or zero when it doesn't
		 * matter. If the current block size exceeds the maximum,
		 * then the alternative cannot be applied.
		 */
		return riscv_cboz_block_size <= (1U << value);
	}
=======

	for_each_set_bit(i, riscv_isa, RISCV_ISA_EXT_MAX) {
		j = riscv_isa_ext2key(i);
		if (j >= 0)
			static_branch_enable(&riscv_isa_ext_keys[j]);
	}
}

#ifdef CONFIG_RISCV_ALTERNATIVE
static bool __init_or_module cpufeature_probe_svpbmt(unsigned int stage)
{
#ifdef CONFIG_RISCV_ISA_SVPBMT
	switch (stage) {
	case RISCV_ALTERNATIVES_EARLY_BOOT:
		return false;
	default:
		return riscv_isa_extension_available(NULL, SVPBMT);
	}
#endif
>>>>>>> b7ba80a49124 (Commit)

	return false;
}

<<<<<<< HEAD
=======
static bool __init_or_module cpufeature_probe_zicbom(unsigned int stage)
{
#ifdef CONFIG_RISCV_ISA_ZICBOM
	switch (stage) {
	case RISCV_ALTERNATIVES_EARLY_BOOT:
		return false;
	default:
		if (riscv_isa_extension_available(NULL, ZICBOM)) {
			riscv_noncoherent_supported();
			return true;
		} else {
			return false;
		}
	}
#endif

	return false;
}

/*
 * Probe presence of individual extensions.
 *
 * This code may also be executed before kernel relocation, so we cannot use
 * addresses generated by the address-of operator as they won't be valid in
 * this context.
 */
static u32 __init_or_module cpufeature_probe(unsigned int stage)
{
	u32 cpu_req_feature = 0;

	if (cpufeature_probe_svpbmt(stage))
		cpu_req_feature |= (1U << CPUFEATURE_SVPBMT);

	if (cpufeature_probe_zicbom(stage))
		cpu_req_feature |= (1U << CPUFEATURE_ZICBOM);

	return cpu_req_feature;
}

>>>>>>> b7ba80a49124 (Commit)
void __init_or_module riscv_cpufeature_patch_func(struct alt_entry *begin,
						  struct alt_entry *end,
						  unsigned int stage)
{
<<<<<<< HEAD
	struct alt_entry *alt;
	void *oldptr, *altptr;
	u16 id, value;

	if (stage == RISCV_ALTERNATIVES_EARLY_BOOT)
		return;
=======
	u32 cpu_req_feature = cpufeature_probe(stage);
	struct alt_entry *alt;
	u32 tmp;
>>>>>>> b7ba80a49124 (Commit)

	for (alt = begin; alt < end; alt++) {
		if (alt->vendor_id != 0)
			continue;
<<<<<<< HEAD

		id = PATCH_ID_CPUFEATURE_ID(alt->patch_id);

		if (id >= RISCV_ISA_EXT_MAX) {
			WARN(1, "This extension id:%d is not in ISA extension list", id);
			continue;
		}

		if (!__riscv_isa_extension_available(NULL, id))
			continue;

		value = PATCH_ID_CPUFEATURE_VALUE(alt->patch_id);
		if (!riscv_cpufeature_patch_check(id, value))
			continue;

		oldptr = ALT_OLD_PTR(alt);
		altptr = ALT_ALT_PTR(alt);

		mutex_lock(&text_mutex);
		patch_text_nosync(oldptr, altptr, alt->alt_len);
		riscv_alternative_fix_offsets(oldptr, alt->alt_len, oldptr - altptr);
		mutex_unlock(&text_mutex);
=======
		if (alt->errata_id >= CPUFEATURE_NUMBER) {
			WARN(1, "This feature id:%d is not in kernel cpufeature list",
				alt->errata_id);
			continue;
		}

		tmp = (1U << alt->errata_id);
		if (cpu_req_feature & tmp)
			patch_text_nosync(alt->old_ptr, alt->alt_ptr, alt->alt_len);
>>>>>>> b7ba80a49124 (Commit)
	}
}
#endif
