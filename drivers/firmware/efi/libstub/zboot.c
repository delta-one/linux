// SPDX-License-Identifier: GPL-2.0

#include <linux/efi.h>
#include <linux/pe.h>
#include <asm/efi.h>
#include <asm/unaligned.h>

#include "efistub.h"

static unsigned char zboot_heap[SZ_256K] __aligned(64);
static unsigned long free_mem_ptr, free_mem_end_ptr;

#define STATIC static
#if defined(CONFIG_KERNEL_GZIP)
#include "../../../../lib/decompress_inflate.c"
#elif defined(CONFIG_KERNEL_LZ4)
#include "../../../../lib/decompress_unlz4.c"
#elif defined(CONFIG_KERNEL_LZMA)
#include "../../../../lib/decompress_unlzma.c"
#elif defined(CONFIG_KERNEL_LZO)
#include "../../../../lib/decompress_unlzo.c"
#elif defined(CONFIG_KERNEL_XZ)
#undef memcpy
#define memcpy memcpy
#undef memmove
#define memmove memmove
#include "../../../../lib/decompress_unxz.c"
#elif defined(CONFIG_KERNEL_ZSTD)
#include "../../../../lib/decompress_unzstd.c"
#endif

extern char efi_zboot_header[];
extern char _gzdata_start[], _gzdata_end[];

<<<<<<< HEAD
static void error(char *x)
{
	efi_err("EFI decompressor: %s\n", x);
}

static unsigned long alloc_preferred_address(unsigned long alloc_size)
{
#ifdef EFI_KIMG_PREFERRED_ADDRESS
	efi_physical_addr_t efi_addr = EFI_KIMG_PREFERRED_ADDRESS;

	if (efi_bs_call(allocate_pages, EFI_ALLOCATE_ADDRESS, EFI_LOADER_DATA,
			alloc_size / EFI_PAGE_SIZE, &efi_addr) == EFI_SUCCESS)
		return efi_addr;
#endif
	return ULONG_MAX;
}

void __weak efi_cache_sync_image(unsigned long image_base,
				 unsigned long alloc_size,
				 unsigned long code_size)
{
	// Provided by the arch to perform the cache maintenance necessary for
	// executable code loaded into memory to be safe for execution.
}

struct screen_info *alloc_screen_info(void)
{
	return __alloc_screen_info();
=======
static void log(efi_char16_t str[])
{
	efi_call_proto(efi_table_attr(efi_system_table, con_out),
		       output_string, L"EFI decompressor: ");
	efi_call_proto(efi_table_attr(efi_system_table, con_out),
		       output_string, str);
	efi_call_proto(efi_table_attr(efi_system_table, con_out),
		       output_string, L"\n");
}

static void error(char *x)
{
	log(L"error() called from decompressor library\n");
}

// Local version to avoid pulling in memcmp()
static bool guids_eq(const efi_guid_t *a, const efi_guid_t *b)
{
	const u32 *l = (u32 *)a;
	const u32 *r = (u32 *)b;

	return l[0] == r[0] && l[1] == r[1] && l[2] == r[2] && l[3] == r[3];
}

static efi_status_t __efiapi
load_file(efi_load_file_protocol_t *this, efi_device_path_protocol_t *rem,
	  bool boot_policy, unsigned long *bufsize, void *buffer)
{
	unsigned long compressed_size = _gzdata_end - _gzdata_start;
	struct efi_vendor_dev_path *vendor_dp;
	bool decompress = false;
	unsigned long size;
	int ret;

	if (rem == NULL || bufsize == NULL)
		return EFI_INVALID_PARAMETER;

	if (boot_policy)
		return EFI_UNSUPPORTED;

	// Look for our vendor media device node in the remaining file path
	if (rem->type == EFI_DEV_MEDIA &&
	    rem->sub_type == EFI_DEV_MEDIA_VENDOR) {
		vendor_dp = container_of(rem, struct efi_vendor_dev_path, header);
		if (!guids_eq(&vendor_dp->vendorguid, &LINUX_EFI_ZBOOT_MEDIA_GUID))
			return EFI_NOT_FOUND;

		decompress = true;
		rem = (void *)(vendor_dp + 1);
	}

	if (rem->type != EFI_DEV_END_PATH ||
	    rem->sub_type != EFI_DEV_END_ENTIRE)
		return EFI_NOT_FOUND;

	// The uncompressed size of the payload is appended to the raw bit
	// stream, and may therefore appear misaligned in memory
	size = decompress ? get_unaligned_le32(_gzdata_end - 4)
			  : compressed_size;
	if (buffer == NULL || *bufsize < size) {
		*bufsize = size;
		return EFI_BUFFER_TOO_SMALL;
	}

	if (decompress) {
		ret = __decompress(_gzdata_start, compressed_size, NULL, NULL,
				   buffer, size, NULL, error);
		if (ret	< 0) {
			log(L"Decompression failed");
			return EFI_DEVICE_ERROR;
		}
	} else {
		memcpy(buffer, _gzdata_start, compressed_size);
	}

	return EFI_SUCCESS;
}

// Return the length in bytes of the device path up to the first end node.
static int device_path_length(const efi_device_path_protocol_t *dp)
{
	int len = 0;

	while (dp->type != EFI_DEV_END_PATH) {
		len += dp->length;
		dp = (void *)((u8 *)dp + dp->length);
	}
	return len;
}

static void append_rel_offset_node(efi_device_path_protocol_t **dp,
				   unsigned long start, unsigned long end)
{
	struct efi_rel_offset_dev_path *rodp = (void *)*dp;

	rodp->header.type	= EFI_DEV_MEDIA;
	rodp->header.sub_type	= EFI_DEV_MEDIA_REL_OFFSET;
	rodp->header.length	= sizeof(struct efi_rel_offset_dev_path);
	rodp->reserved		= 0;
	rodp->starting_offset	= start;
	rodp->ending_offset	= end;

	*dp = (void *)(rodp + 1);
}

static void append_ven_media_node(efi_device_path_protocol_t **dp,
				  efi_guid_t *guid)
{
	struct efi_vendor_dev_path *vmdp = (void *)*dp;

	vmdp->header.type	= EFI_DEV_MEDIA;
	vmdp->header.sub_type	= EFI_DEV_MEDIA_VENDOR;
	vmdp->header.length	= sizeof(struct efi_vendor_dev_path);
	vmdp->vendorguid	= *guid;

	*dp = (void *)(vmdp + 1);
}

static void append_end_node(efi_device_path_protocol_t **dp)
{
	(*dp)->type		= EFI_DEV_END_PATH;
	(*dp)->sub_type		= EFI_DEV_END_ENTIRE;
	(*dp)->length		= sizeof(struct efi_generic_dev_path);

	++*dp;
>>>>>>> b7ba80a49124 (Commit)
}

asmlinkage efi_status_t __efiapi
efi_zboot_entry(efi_handle_t handle, efi_system_table_t *systab)
{
<<<<<<< HEAD
	unsigned long compressed_size = _gzdata_end - _gzdata_start;
	unsigned long image_base, alloc_size, code_size;
	efi_loaded_image_t *image;
	efi_status_t status;
	char *cmdline_ptr;
	int ret;
=======
	struct efi_mem_mapped_dev_path mmdp = {
		.header.type		= EFI_DEV_HW,
		.header.sub_type	= EFI_DEV_MEM_MAPPED,
		.header.length		= sizeof(struct efi_mem_mapped_dev_path)
	};
	efi_device_path_protocol_t *parent_dp, *dpp, *lf2_dp, *li_dp;
	efi_load_file2_protocol_t zboot_load_file2;
	efi_loaded_image_t *parent, *child;
	unsigned long exit_data_size;
	efi_handle_t child_handle;
	efi_handle_t zboot_handle;
	efi_char16_t *exit_data;
	efi_status_t status;
	void *dp_alloc;
	int dp_len;
>>>>>>> b7ba80a49124 (Commit)

	WRITE_ONCE(efi_system_table, systab);

	free_mem_ptr = (unsigned long)&zboot_heap;
	free_mem_end_ptr = free_mem_ptr + sizeof(zboot_heap);

<<<<<<< HEAD
	status = efi_bs_call(handle_protocol, handle,
			     &LOADED_IMAGE_PROTOCOL_GUID, (void **)&image);
	if (status != EFI_SUCCESS) {
		error("Failed to locate parent's loaded image protocol");
		return status;
	}

	status = efi_handle_cmdline(image, &cmdline_ptr);
	if (status != EFI_SUCCESS)
		return status;

	efi_info("Decompressing Linux Kernel...\n");

	// SizeOfImage from the compressee's PE/COFF header
	alloc_size = round_up(get_unaligned_le32(_gzdata_end - 4),
			      EFI_ALLOC_ALIGN);

	// SizeOfHeaders and SizeOfCode from the compressee's PE/COFF header
	code_size = get_unaligned_le32(_gzdata_end - 8) +
		    get_unaligned_le32(_gzdata_end - 12);

	 // If the architecture has a preferred address for the image,
	 // try that first.
	image_base = alloc_preferred_address(alloc_size);
	if (image_base == ULONG_MAX) {
		unsigned long min_kimg_align = efi_get_kimg_min_align();
		u32 seed = U32_MAX;

		if (!IS_ENABLED(CONFIG_RANDOMIZE_BASE)) {
			// Setting the random seed to 0x0 is the same as
			// allocating as low as possible
			seed = 0;
		} else if (efi_nokaslr) {
			efi_info("KASLR disabled on kernel command line\n");
		} else {
			status = efi_get_random_bytes(sizeof(seed), (u8 *)&seed);
			if (status == EFI_NOT_FOUND) {
				efi_info("EFI_RNG_PROTOCOL unavailable\n");
				efi_nokaslr = true;
			} else if (status != EFI_SUCCESS) {
				efi_err("efi_get_random_bytes() failed (0x%lx)\n",
					status);
				efi_nokaslr = true;
			}
		}

		status = efi_random_alloc(alloc_size, min_kimg_align, &image_base,
					  seed, EFI_LOADER_CODE);
		if (status != EFI_SUCCESS) {
			efi_err("Failed to allocate memory\n");
			goto free_cmdline;
		}
	}

	// Decompress the payload into the newly allocated buffer.
	ret = __decompress(_gzdata_start, compressed_size, NULL, NULL,
			   (void *)image_base, alloc_size, NULL, error);
	if (ret	< 0) {
		error("Decompression failed");
		status = EFI_DEVICE_ERROR;
		goto free_image;
	}

	efi_cache_sync_image(image_base, alloc_size, code_size);

	efi_remap_image(image_base, alloc_size, code_size);

	status = efi_stub_common(handle, image, image_base, cmdline_ptr);

free_image:
	efi_free(alloc_size, image_base);
free_cmdline:
	efi_bs_call(free_pool, cmdline_ptr);
=======
	exit_data = NULL;
	exit_data_size = 0;

	status = efi_bs_call(handle_protocol, handle,
			     &LOADED_IMAGE_PROTOCOL_GUID, (void **)&parent);
	if (status != EFI_SUCCESS) {
		log(L"Failed to locate parent's loaded image protocol");
		return status;
	}

	status = efi_bs_call(handle_protocol, handle,
			     &LOADED_IMAGE_DEVICE_PATH_PROTOCOL_GUID,
			     (void **)&parent_dp);
	if (status != EFI_SUCCESS || parent_dp == NULL) {
		// Create a MemoryMapped() device path node to describe
		// the parent image if no device path was provided.
		mmdp.memory_type	= parent->image_code_type;
		mmdp.starting_addr	= (unsigned long)parent->image_base;
		mmdp.ending_addr	= (unsigned long)parent->image_base +
					  parent->image_size - 1;
		parent_dp = &mmdp.header;
		dp_len = sizeof(mmdp);
	} else {
		dp_len = device_path_length(parent_dp);
	}

	// Allocate some pool memory for device path protocol data
	status = efi_bs_call(allocate_pool, EFI_LOADER_DATA,
			     2 * (dp_len + sizeof(struct efi_rel_offset_dev_path) +
			          sizeof(struct efi_generic_dev_path)) +
			     sizeof(struct efi_vendor_dev_path),
			     (void **)&dp_alloc);
	if (status != EFI_SUCCESS) {
		log(L"Failed to allocate device path pool memory");
		return status;
	}

	// Create a device path describing the compressed payload in this image
	// <...parent_dp...>/Offset(<start>, <end>)
	lf2_dp = memcpy(dp_alloc, parent_dp, dp_len);
	dpp = (void *)((u8 *)lf2_dp + dp_len);
	append_rel_offset_node(&dpp,
			       (unsigned long)(_gzdata_start - efi_zboot_header),
			       (unsigned long)(_gzdata_end - efi_zboot_header - 1));
	append_end_node(&dpp);

	// Create a device path describing the decompressed payload in this image
	// <...parent_dp...>/Offset(<start>, <end>)/VenMedia(ZBOOT_MEDIA_GUID)
	dp_len += sizeof(struct efi_rel_offset_dev_path);
	li_dp = memcpy(dpp, lf2_dp, dp_len);
	dpp = (void *)((u8 *)li_dp + dp_len);
	append_ven_media_node(&dpp, &LINUX_EFI_ZBOOT_MEDIA_GUID);
	append_end_node(&dpp);

	zboot_handle = NULL;
	zboot_load_file2.load_file = load_file;
	status = efi_bs_call(install_multiple_protocol_interfaces,
			     &zboot_handle,
			     &EFI_DEVICE_PATH_PROTOCOL_GUID, lf2_dp,
			     &EFI_LOAD_FILE2_PROTOCOL_GUID, &zboot_load_file2,
			     NULL);
	if (status != EFI_SUCCESS) {
		log(L"Failed to install LoadFile2 protocol and device path");
		goto free_dpalloc;
	}

	status = efi_bs_call(load_image, false, handle, li_dp, NULL, 0,
			     &child_handle);
	if (status != EFI_SUCCESS) {
		log(L"Failed to load image");
		goto uninstall_lf2;
	}

	status = efi_bs_call(handle_protocol, child_handle,
			     &LOADED_IMAGE_PROTOCOL_GUID, (void **)&child);
	if (status != EFI_SUCCESS) {
		log(L"Failed to locate child's loaded image protocol");
		goto unload_image;
	}

	// Copy the kernel command line
	child->load_options = parent->load_options;
	child->load_options_size = parent->load_options_size;

	status = efi_bs_call(start_image, child_handle, &exit_data_size,
			     &exit_data);
	if (status != EFI_SUCCESS) {
		log(L"StartImage() returned with error");
		if (exit_data_size > 0)
			log(exit_data);

		// If StartImage() returns EFI_SECURITY_VIOLATION, the image is
		// not unloaded so we need to do it by hand.
		if (status == EFI_SECURITY_VIOLATION)
unload_image:
			efi_bs_call(unload_image, child_handle);
	}

uninstall_lf2:
	efi_bs_call(uninstall_multiple_protocol_interfaces,
		    zboot_handle,
		    &EFI_DEVICE_PATH_PROTOCOL_GUID, lf2_dp,
		    &EFI_LOAD_FILE2_PROTOCOL_GUID, &zboot_load_file2,
		    NULL);

free_dpalloc:
	efi_bs_call(free_pool, dp_alloc);

	efi_bs_call(exit, handle, status, exit_data_size, exit_data);

	// Free ExitData in case Exit() returned with a failure code,
	// but return the original status code.
	log(L"Exit() returned with failure code");
	if (exit_data != NULL)
		efi_bs_call(free_pool, exit_data);
>>>>>>> b7ba80a49124 (Commit)
	return status;
}
