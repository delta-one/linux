// SPDX-License-Identifier: BSD-3-Clause
/*
 *  linux/net/sunrpc/gss_krb5_mech.c
 *
 *  Copyright (c) 2001-2008 The Regents of the University of Michigan.
 *  All rights reserved.
 *
 *  Andy Adamson <andros@umich.edu>
 *  J. Bruce Fields <bfields@umich.edu>
 */

#include <crypto/hash.h>
#include <crypto/skcipher.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/sunrpc/auth.h>
#include <linux/sunrpc/gss_krb5.h>
#include <linux/sunrpc/xdr.h>
<<<<<<< HEAD
#include <kunit/visibility.h>

#include "auth_gss_internal.h"
#include "gss_krb5_internal.h"
=======
#include <linux/sunrpc/gss_krb5_enctypes.h>

#include "auth_gss_internal.h"
>>>>>>> b7ba80a49124 (Commit)

#if IS_ENABLED(CONFIG_SUNRPC_DEBUG)
# define RPCDBG_FACILITY	RPCDBG_AUTH
#endif

<<<<<<< HEAD
static struct gss_api_mech gss_kerberos_mech;

#if defined(CONFIG_RPCSEC_GSS_KRB5_SIMPLIFIED)
static int gss_krb5_import_ctx_des(struct krb5_ctx *ctx, gfp_t gfp_mask);
static int gss_krb5_import_ctx_v1(struct krb5_ctx *ctx, gfp_t gfp_mask);
#endif
#if defined(CONFIG_RPCSEC_GSS_KRB5_CRYPTOSYSTEM)
static int gss_krb5_import_ctx_v2(struct krb5_ctx *ctx, gfp_t gfp_mask);
#endif

static const struct gss_krb5_enctype supported_gss_krb5_enctypes[] = {
#if defined(CONFIG_RPCSEC_GSS_KRB5_ENCTYPES_DES)
=======
static struct gss_api_mech gss_kerberos_mech;	/* forward declaration */

static const struct gss_krb5_enctype supported_gss_krb5_enctypes[] = {
#ifndef CONFIG_SUNRPC_DISABLE_INSECURE_ENCTYPES
>>>>>>> b7ba80a49124 (Commit)
	/*
	 * DES (All DES enctypes are mapped to the same gss functionality)
	 */
	{
	  .etype = ENCTYPE_DES_CBC_RAW,
	  .ctype = CKSUMTYPE_RSA_MD5,
	  .name = "des-cbc-crc",
	  .encrypt_name = "cbc(des)",
	  .cksum_name = "md5",
<<<<<<< HEAD
	  .import_ctx = gss_krb5_import_ctx_des,
	  .get_mic = gss_krb5_get_mic_v1,
	  .verify_mic = gss_krb5_verify_mic_v1,
	  .wrap = gss_krb5_wrap_v1,
	  .unwrap = gss_krb5_unwrap_v1,
=======
	  .encrypt = krb5_encrypt,
	  .decrypt = krb5_decrypt,
	  .mk_key = NULL,
>>>>>>> b7ba80a49124 (Commit)
	  .signalg = SGN_ALG_DES_MAC_MD5,
	  .sealalg = SEAL_ALG_DES,
	  .keybytes = 7,
	  .keylength = 8,
<<<<<<< HEAD
	  .cksumlength = 8,
	  .keyed_cksum = 0,
	},
=======
	  .blocksize = 8,
	  .conflen = 8,
	  .cksumlength = 8,
	  .keyed_cksum = 0,
	},
#endif	/* CONFIG_SUNRPC_DISABLE_INSECURE_ENCTYPES */
>>>>>>> b7ba80a49124 (Commit)
	/*
	 * 3DES
	 */
	{
	  .etype = ENCTYPE_DES3_CBC_RAW,
	  .ctype = CKSUMTYPE_HMAC_SHA1_DES3,
	  .name = "des3-hmac-sha1",
	  .encrypt_name = "cbc(des3_ede)",
	  .cksum_name = "hmac(sha1)",
<<<<<<< HEAD
	  .import_ctx = gss_krb5_import_ctx_v1,
	  .derive_key = krb5_derive_key_v1,
	  .get_mic = gss_krb5_get_mic_v1,
	  .verify_mic = gss_krb5_verify_mic_v1,
	  .wrap = gss_krb5_wrap_v1,
	  .unwrap = gss_krb5_unwrap_v1,
=======
	  .encrypt = krb5_encrypt,
	  .decrypt = krb5_decrypt,
	  .mk_key = gss_krb5_des3_make_key,
>>>>>>> b7ba80a49124 (Commit)
	  .signalg = SGN_ALG_HMAC_SHA1_DES3_KD,
	  .sealalg = SEAL_ALG_DES3KD,
	  .keybytes = 21,
	  .keylength = 24,
<<<<<<< HEAD
	  .cksumlength = 20,
	  .keyed_cksum = 1,
	},
#endif

#if defined(CONFIG_RPCSEC_GSS_KRB5_ENCTYPES_AES_SHA1)
	/*
	 * AES-128 with SHA-1 (RFC 3962)
=======
	  .blocksize = 8,
	  .conflen = 8,
	  .cksumlength = 20,
	  .keyed_cksum = 1,
	},
	/*
	 * AES128
>>>>>>> b7ba80a49124 (Commit)
	 */
	{
	  .etype = ENCTYPE_AES128_CTS_HMAC_SHA1_96,
	  .ctype = CKSUMTYPE_HMAC_SHA1_96_AES128,
	  .name = "aes128-cts",
	  .encrypt_name = "cts(cbc(aes))",
<<<<<<< HEAD
	  .aux_cipher = "cbc(aes)",
	  .cksum_name = "hmac(sha1)",
	  .import_ctx = gss_krb5_import_ctx_v2,
	  .derive_key = krb5_derive_key_v2,
	  .encrypt = gss_krb5_aes_encrypt,
	  .decrypt = gss_krb5_aes_decrypt,

	  .get_mic = gss_krb5_get_mic_v2,
	  .verify_mic = gss_krb5_verify_mic_v2,
	  .wrap = gss_krb5_wrap_v2,
	  .unwrap = gss_krb5_unwrap_v2,

	  .signalg = -1,
	  .sealalg = -1,
	  .keybytes = 16,
	  .keylength = BITS2OCTETS(128),
	  .Kc_length = BITS2OCTETS(128),
	  .Ke_length = BITS2OCTETS(128),
	  .Ki_length = BITS2OCTETS(128),
	  .cksumlength = BITS2OCTETS(96),
	  .keyed_cksum = 1,
	},
	/*
	 * AES-256 with SHA-1 (RFC 3962)
=======
	  .cksum_name = "hmac(sha1)",
	  .encrypt = krb5_encrypt,
	  .decrypt = krb5_decrypt,
	  .mk_key = gss_krb5_aes_make_key,
	  .encrypt_v2 = gss_krb5_aes_encrypt,
	  .decrypt_v2 = gss_krb5_aes_decrypt,
	  .signalg = -1,
	  .sealalg = -1,
	  .keybytes = 16,
	  .keylength = 16,
	  .blocksize = 16,
	  .conflen = 16,
	  .cksumlength = 12,
	  .keyed_cksum = 1,
	},
	/*
	 * AES256
>>>>>>> b7ba80a49124 (Commit)
	 */
	{
	  .etype = ENCTYPE_AES256_CTS_HMAC_SHA1_96,
	  .ctype = CKSUMTYPE_HMAC_SHA1_96_AES256,
	  .name = "aes256-cts",
	  .encrypt_name = "cts(cbc(aes))",
<<<<<<< HEAD
	  .aux_cipher = "cbc(aes)",
	  .cksum_name = "hmac(sha1)",
	  .import_ctx = gss_krb5_import_ctx_v2,
	  .derive_key = krb5_derive_key_v2,
	  .encrypt = gss_krb5_aes_encrypt,
	  .decrypt = gss_krb5_aes_decrypt,

	  .get_mic = gss_krb5_get_mic_v2,
	  .verify_mic = gss_krb5_verify_mic_v2,
	  .wrap = gss_krb5_wrap_v2,
	  .unwrap = gss_krb5_unwrap_v2,

	  .signalg = -1,
	  .sealalg = -1,
	  .keybytes = 32,
	  .keylength = BITS2OCTETS(256),
	  .Kc_length = BITS2OCTETS(256),
	  .Ke_length = BITS2OCTETS(256),
	  .Ki_length = BITS2OCTETS(256),
	  .cksumlength = BITS2OCTETS(96),
	  .keyed_cksum = 1,
	},
#endif

#if defined(CONFIG_RPCSEC_GSS_KRB5_ENCTYPES_CAMELLIA)
	/*
	 * Camellia-128 with CMAC (RFC 6803)
	 */
	{
		.etype		= ENCTYPE_CAMELLIA128_CTS_CMAC,
		.ctype		= CKSUMTYPE_CMAC_CAMELLIA128,
		.name		= "camellia128-cts-cmac",
		.encrypt_name	= "cts(cbc(camellia))",
		.aux_cipher	= "cbc(camellia)",
		.cksum_name	= "cmac(camellia)",
		.cksumlength	= BITS2OCTETS(128),
		.keyed_cksum	= 1,
		.keylength	= BITS2OCTETS(128),
		.Kc_length	= BITS2OCTETS(128),
		.Ke_length	= BITS2OCTETS(128),
		.Ki_length	= BITS2OCTETS(128),

		.import_ctx	= gss_krb5_import_ctx_v2,
		.derive_key	= krb5_kdf_feedback_cmac,
		.encrypt	= gss_krb5_aes_encrypt,
		.decrypt	= gss_krb5_aes_decrypt,

		.get_mic	= gss_krb5_get_mic_v2,
		.verify_mic	= gss_krb5_verify_mic_v2,
		.wrap		= gss_krb5_wrap_v2,
		.unwrap		= gss_krb5_unwrap_v2,
	},
	/*
	 * Camellia-256 with CMAC (RFC 6803)
	 */
	{
		.etype		= ENCTYPE_CAMELLIA256_CTS_CMAC,
		.ctype		= CKSUMTYPE_CMAC_CAMELLIA256,
		.name		= "camellia256-cts-cmac",
		.encrypt_name	= "cts(cbc(camellia))",
		.aux_cipher	= "cbc(camellia)",
		.cksum_name	= "cmac(camellia)",
		.cksumlength	= BITS2OCTETS(128),
		.keyed_cksum	= 1,
		.keylength	= BITS2OCTETS(256),
		.Kc_length	= BITS2OCTETS(256),
		.Ke_length	= BITS2OCTETS(256),
		.Ki_length	= BITS2OCTETS(256),

		.import_ctx	= gss_krb5_import_ctx_v2,
		.derive_key	= krb5_kdf_feedback_cmac,
		.encrypt	= gss_krb5_aes_encrypt,
		.decrypt	= gss_krb5_aes_decrypt,

		.get_mic	= gss_krb5_get_mic_v2,
		.verify_mic	= gss_krb5_verify_mic_v2,
		.wrap		= gss_krb5_wrap_v2,
		.unwrap		= gss_krb5_unwrap_v2,
	},
#endif

#if defined(CONFIG_RPCSEC_GSS_KRB5_ENCTYPES_AES_SHA2)
	/*
	 * AES-128 with SHA-256 (RFC 8009)
	 */
	{
		.etype		= ENCTYPE_AES128_CTS_HMAC_SHA256_128,
		.ctype		= CKSUMTYPE_HMAC_SHA256_128_AES128,
		.name		= "aes128-cts-hmac-sha256-128",
		.encrypt_name	= "cts(cbc(aes))",
		.aux_cipher	= "cbc(aes)",
		.cksum_name	= "hmac(sha256)",
		.cksumlength	= BITS2OCTETS(128),
		.keyed_cksum	= 1,
		.keylength	= BITS2OCTETS(128),
		.Kc_length	= BITS2OCTETS(128),
		.Ke_length	= BITS2OCTETS(128),
		.Ki_length	= BITS2OCTETS(128),

		.import_ctx	= gss_krb5_import_ctx_v2,
		.derive_key	= krb5_kdf_hmac_sha2,
		.encrypt	= krb5_etm_encrypt,
		.decrypt	= krb5_etm_decrypt,

		.get_mic	= gss_krb5_get_mic_v2,
		.verify_mic	= gss_krb5_verify_mic_v2,
		.wrap		= gss_krb5_wrap_v2,
		.unwrap		= gss_krb5_unwrap_v2,
	},
	/*
	 * AES-256 with SHA-384 (RFC 8009)
	 */
	{
		.etype		= ENCTYPE_AES256_CTS_HMAC_SHA384_192,
		.ctype		= CKSUMTYPE_HMAC_SHA384_192_AES256,
		.name		= "aes256-cts-hmac-sha384-192",
		.encrypt_name	= "cts(cbc(aes))",
		.aux_cipher	= "cbc(aes)",
		.cksum_name	= "hmac(sha384)",
		.cksumlength	= BITS2OCTETS(192),
		.keyed_cksum	= 1,
		.keylength	= BITS2OCTETS(256),
		.Kc_length	= BITS2OCTETS(192),
		.Ke_length	= BITS2OCTETS(256),
		.Ki_length	= BITS2OCTETS(192),

		.import_ctx	= gss_krb5_import_ctx_v2,
		.derive_key	= krb5_kdf_hmac_sha2,
		.encrypt	= krb5_etm_encrypt,
		.decrypt	= krb5_etm_decrypt,

		.get_mic	= gss_krb5_get_mic_v2,
		.verify_mic	= gss_krb5_verify_mic_v2,
		.wrap		= gss_krb5_wrap_v2,
		.unwrap		= gss_krb5_unwrap_v2,
	},
#endif
};

/*
 * The list of advertised enctypes is specified in order of most
 * preferred to least.
 */
static char gss_krb5_enctype_priority_list[64];

static void gss_krb5_prepare_enctype_priority_list(void)
{
	static const u32 gss_krb5_enctypes[] = {
#if defined(CONFIG_RPCSEC_GSS_KRB5_ENCTYPES_AES_SHA2)
		ENCTYPE_AES256_CTS_HMAC_SHA384_192,
		ENCTYPE_AES128_CTS_HMAC_SHA256_128,
#endif
#if defined(CONFIG_RPCSEC_GSS_KRB5_ENCTYPES_CAMELLIA)
		ENCTYPE_CAMELLIA256_CTS_CMAC,
		ENCTYPE_CAMELLIA128_CTS_CMAC,
#endif
#if defined(CONFIG_RPCSEC_GSS_KRB5_ENCTYPES_AES_SHA1)
		ENCTYPE_AES256_CTS_HMAC_SHA1_96,
		ENCTYPE_AES128_CTS_HMAC_SHA1_96,
#endif
#if defined(CONFIG_RPCSEC_GSS_KRB5_ENCTYPES_DES)
		ENCTYPE_DES3_CBC_SHA1,
		ENCTYPE_DES_CBC_MD5,
		ENCTYPE_DES_CBC_CRC,
		ENCTYPE_DES_CBC_MD4,
#endif
	};
	size_t total, i;
	char buf[16];
	char *sep;
	int n;

	sep = "";
	gss_krb5_enctype_priority_list[0] = '\0';
	for (total = 0, i = 0; i < ARRAY_SIZE(gss_krb5_enctypes); i++) {
		n = sprintf(buf, "%s%u", sep, gss_krb5_enctypes[i]);
		if (n < 0)
			break;
		if (total + n >= sizeof(gss_krb5_enctype_priority_list))
			break;
		strcat(gss_krb5_enctype_priority_list, buf);
		sep = ",";
		total += n;
	}
}

/**
 * gss_krb5_lookup_enctype - Retrieve profile information for a given enctype
 * @etype: ENCTYPE value
 *
 * Returns a pointer to a gss_krb5_enctype structure, or NULL if no
 * matching etype is found.
 */
VISIBLE_IF_KUNIT
const struct gss_krb5_enctype *gss_krb5_lookup_enctype(u32 etype)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(supported_gss_krb5_enctypes); i++)
=======
	  .cksum_name = "hmac(sha1)",
	  .encrypt = krb5_encrypt,
	  .decrypt = krb5_decrypt,
	  .mk_key = gss_krb5_aes_make_key,
	  .encrypt_v2 = gss_krb5_aes_encrypt,
	  .decrypt_v2 = gss_krb5_aes_decrypt,
	  .signalg = -1,
	  .sealalg = -1,
	  .keybytes = 32,
	  .keylength = 32,
	  .blocksize = 16,
	  .conflen = 16,
	  .cksumlength = 12,
	  .keyed_cksum = 1,
	},
};

static const int num_supported_enctypes =
	ARRAY_SIZE(supported_gss_krb5_enctypes);

static int
supported_gss_krb5_enctype(int etype)
{
	int i;
	for (i = 0; i < num_supported_enctypes; i++)
		if (supported_gss_krb5_enctypes[i].etype == etype)
			return 1;
	return 0;
}

static const struct gss_krb5_enctype *
get_gss_krb5_enctype(int etype)
{
	int i;
	for (i = 0; i < num_supported_enctypes; i++)
>>>>>>> b7ba80a49124 (Commit)
		if (supported_gss_krb5_enctypes[i].etype == etype)
			return &supported_gss_krb5_enctypes[i];
	return NULL;
}
<<<<<<< HEAD
EXPORT_SYMBOL_IF_KUNIT(gss_krb5_lookup_enctype);

static struct crypto_sync_skcipher *
gss_krb5_alloc_cipher_v1(struct krb5_ctx *ctx, struct xdr_netobj *key)
{
	struct crypto_sync_skcipher *tfm;

	tfm = crypto_alloc_sync_skcipher(ctx->gk5e->encrypt_name, 0, 0);
	if (IS_ERR(tfm))
		return NULL;
	if (crypto_sync_skcipher_setkey(tfm, key->data, key->len)) {
		crypto_free_sync_skcipher(tfm);
		return NULL;
	}
	return tfm;
}
=======
>>>>>>> b7ba80a49124 (Commit)

static inline const void *
get_key(const void *p, const void *end,
	struct krb5_ctx *ctx, struct crypto_sync_skcipher **res)
{
<<<<<<< HEAD
	struct crypto_sync_skcipher *tfm;
=======
>>>>>>> b7ba80a49124 (Commit)
	struct xdr_netobj	key;
	int			alg;

	p = simple_get_bytes(p, end, &alg, sizeof(alg));
	if (IS_ERR(p))
		goto out_err;
<<<<<<< HEAD
=======

>>>>>>> b7ba80a49124 (Commit)
	switch (alg) {
	case ENCTYPE_DES_CBC_CRC:
	case ENCTYPE_DES_CBC_MD4:
	case ENCTYPE_DES_CBC_MD5:
		/* Map all these key types to ENCTYPE_DES_CBC_RAW */
		alg = ENCTYPE_DES_CBC_RAW;
		break;
	}
<<<<<<< HEAD
	if (!gss_krb5_lookup_enctype(alg)) {
		pr_warn("gss_krb5: unsupported enctype: %d\n", alg);
		goto out_err_inval;
	}

	p = simple_get_netobj(p, end, &key);
	if (IS_ERR(p))
		goto out_err;
	tfm = gss_krb5_alloc_cipher_v1(ctx, &key);
	kfree(key.data);
	if (!tfm) {
		pr_warn("gss_krb5: failed to initialize cipher '%s'\n",
			ctx->gk5e->encrypt_name);
		goto out_err_inval;
	}
	*res = tfm;

	return p;

out_err_inval:
=======

	if (!supported_gss_krb5_enctype(alg)) {
		printk(KERN_WARNING "gss_kerberos_mech: unsupported "
			"encryption key algorithm %d\n", alg);
		p = ERR_PTR(-EINVAL);
		goto out_err;
	}
	p = simple_get_netobj(p, end, &key);
	if (IS_ERR(p))
		goto out_err;

	*res = crypto_alloc_sync_skcipher(ctx->gk5e->encrypt_name, 0, 0);
	if (IS_ERR(*res)) {
		printk(KERN_WARNING "gss_kerberos_mech: unable to initialize "
			"crypto algorithm %s\n", ctx->gk5e->encrypt_name);
		*res = NULL;
		goto out_err_free_key;
	}
	if (crypto_sync_skcipher_setkey(*res, key.data, key.len)) {
		printk(KERN_WARNING "gss_kerberos_mech: error setting key for "
			"crypto algorithm %s\n", ctx->gk5e->encrypt_name);
		goto out_err_free_tfm;
	}

	kfree(key.data);
	return p;

out_err_free_tfm:
	crypto_free_sync_skcipher(*res);
out_err_free_key:
	kfree(key.data);
>>>>>>> b7ba80a49124 (Commit)
	p = ERR_PTR(-EINVAL);
out_err:
	return p;
}

static int
gss_import_v1_context(const void *p, const void *end, struct krb5_ctx *ctx)
{
	u32 seq_send;
	int tmp;
	u32 time32;

	p = simple_get_bytes(p, end, &ctx->initiate, sizeof(ctx->initiate));
	if (IS_ERR(p))
		goto out_err;

	/* Old format supports only DES!  Any other enctype uses new format */
	ctx->enctype = ENCTYPE_DES_CBC_RAW;

<<<<<<< HEAD
	ctx->gk5e = gss_krb5_lookup_enctype(ctx->enctype);
=======
	ctx->gk5e = get_gss_krb5_enctype(ctx->enctype);
>>>>>>> b7ba80a49124 (Commit)
	if (ctx->gk5e == NULL) {
		p = ERR_PTR(-EINVAL);
		goto out_err;
	}

	/* The downcall format was designed before we completely understood
	 * the uses of the context fields; so it includes some stuff we
	 * just give some minimal sanity-checking, and some we ignore
	 * completely (like the next twenty bytes): */
	if (unlikely(p + 20 > end || p + 20 < p)) {
		p = ERR_PTR(-EFAULT);
		goto out_err;
	}
	p += 20;
	p = simple_get_bytes(p, end, &tmp, sizeof(tmp));
	if (IS_ERR(p))
		goto out_err;
	if (tmp != SGN_ALG_DES_MAC_MD5) {
		p = ERR_PTR(-ENOSYS);
		goto out_err;
	}
	p = simple_get_bytes(p, end, &tmp, sizeof(tmp));
	if (IS_ERR(p))
		goto out_err;
	if (tmp != SEAL_ALG_DES) {
		p = ERR_PTR(-ENOSYS);
		goto out_err;
	}
	p = simple_get_bytes(p, end, &time32, sizeof(time32));
	if (IS_ERR(p))
		goto out_err;
	/* unsigned 32-bit time overflows in year 2106 */
	ctx->endtime = (time64_t)time32;
	p = simple_get_bytes(p, end, &seq_send, sizeof(seq_send));
	if (IS_ERR(p))
		goto out_err;
	atomic_set(&ctx->seq_send, seq_send);
	p = simple_get_netobj(p, end, &ctx->mech_used);
	if (IS_ERR(p))
		goto out_err;
	p = get_key(p, end, ctx, &ctx->enc);
	if (IS_ERR(p))
		goto out_err_free_mech;
	p = get_key(p, end, ctx, &ctx->seq);
	if (IS_ERR(p))
		goto out_err_free_key1;
	if (p != end) {
		p = ERR_PTR(-EFAULT);
		goto out_err_free_key2;
	}

	return 0;

out_err_free_key2:
	crypto_free_sync_skcipher(ctx->seq);
out_err_free_key1:
	crypto_free_sync_skcipher(ctx->enc);
out_err_free_mech:
	kfree(ctx->mech_used.data);
out_err:
	return PTR_ERR(p);
}

<<<<<<< HEAD
#if defined(CONFIG_RPCSEC_GSS_KRB5_SIMPLIFIED)
static int
gss_krb5_import_ctx_des(struct krb5_ctx *ctx, gfp_t gfp_mask)
{
	return -EINVAL;
}

static int
gss_krb5_import_ctx_v1(struct krb5_ctx *ctx, gfp_t gfp_mask)
{
	struct xdr_netobj keyin, keyout;

	keyin.data = ctx->Ksess;
	keyin.len = ctx->gk5e->keylength;

	ctx->seq = gss_krb5_alloc_cipher_v1(ctx, &keyin);
	if (ctx->seq == NULL)
		goto out_err;
	ctx->enc = gss_krb5_alloc_cipher_v1(ctx, &keyin);
=======
static struct crypto_sync_skcipher *
context_v2_alloc_cipher(struct krb5_ctx *ctx, const char *cname, u8 *key)
{
	struct crypto_sync_skcipher *cp;

	cp = crypto_alloc_sync_skcipher(cname, 0, 0);
	if (IS_ERR(cp)) {
		dprintk("gss_kerberos_mech: unable to initialize "
			"crypto algorithm %s\n", cname);
		return NULL;
	}
	if (crypto_sync_skcipher_setkey(cp, key, ctx->gk5e->keylength)) {
		dprintk("gss_kerberos_mech: error setting key for "
			"crypto algorithm %s\n", cname);
		crypto_free_sync_skcipher(cp);
		return NULL;
	}
	return cp;
}

static inline void
set_cdata(u8 cdata[GSS_KRB5_K5CLENGTH], u32 usage, u8 seed)
{
	cdata[0] = (usage>>24)&0xff;
	cdata[1] = (usage>>16)&0xff;
	cdata[2] = (usage>>8)&0xff;
	cdata[3] = usage&0xff;
	cdata[4] = seed;
}

static int
context_derive_keys_des3(struct krb5_ctx *ctx, gfp_t gfp_mask)
{
	struct xdr_netobj c, keyin, keyout;
	u8 cdata[GSS_KRB5_K5CLENGTH];
	u32 err;

	c.len = GSS_KRB5_K5CLENGTH;
	c.data = cdata;

	keyin.data = ctx->Ksess;
	keyin.len = ctx->gk5e->keylength;
	keyout.len = ctx->gk5e->keylength;

	/* seq uses the raw key */
	ctx->seq = context_v2_alloc_cipher(ctx, ctx->gk5e->encrypt_name,
					   ctx->Ksess);
	if (ctx->seq == NULL)
		goto out_err;

	ctx->enc = context_v2_alloc_cipher(ctx, ctx->gk5e->encrypt_name,
					   ctx->Ksess);
>>>>>>> b7ba80a49124 (Commit)
	if (ctx->enc == NULL)
		goto out_free_seq;

	/* derive cksum */
<<<<<<< HEAD
	keyout.data = ctx->cksum;
	keyout.len = ctx->gk5e->keylength;
	if (krb5_derive_key(ctx, &keyin, &keyout, KG_USAGE_SIGN,
			    KEY_USAGE_SEED_CHECKSUM, gfp_mask))
		goto out_free_enc;
=======
	set_cdata(cdata, KG_USAGE_SIGN, KEY_USAGE_SEED_CHECKSUM);
	keyout.data = ctx->cksum;
	err = krb5_derive_key(ctx->gk5e, &keyin, &keyout, &c, gfp_mask);
	if (err) {
		dprintk("%s: Error %d deriving cksum key\n",
			__func__, err);
		goto out_free_enc;
	}
>>>>>>> b7ba80a49124 (Commit)

	return 0;

out_free_enc:
	crypto_free_sync_skcipher(ctx->enc);
out_free_seq:
	crypto_free_sync_skcipher(ctx->seq);
out_err:
	return -EINVAL;
}
<<<<<<< HEAD
#endif

#if defined(CONFIG_RPCSEC_GSS_KRB5_CRYPTOSYSTEM)

static struct crypto_sync_skcipher *
gss_krb5_alloc_cipher_v2(const char *cname, const struct xdr_netobj *key)
{
	struct crypto_sync_skcipher *tfm;

	tfm = crypto_alloc_sync_skcipher(cname, 0, 0);
	if (IS_ERR(tfm))
		return NULL;
	if (crypto_sync_skcipher_setkey(tfm, key->data, key->len)) {
		crypto_free_sync_skcipher(tfm);
		return NULL;
	}
	return tfm;
}

static struct crypto_ahash *
gss_krb5_alloc_hash_v2(struct krb5_ctx *kctx, const struct xdr_netobj *key)
{
	struct crypto_ahash *tfm;

	tfm = crypto_alloc_ahash(kctx->gk5e->cksum_name, 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(tfm))
		return NULL;
	if (crypto_ahash_setkey(tfm, key->data, key->len)) {
		crypto_free_ahash(tfm);
		return NULL;
	}
	return tfm;
}

static int
gss_krb5_import_ctx_v2(struct krb5_ctx *ctx, gfp_t gfp_mask)
{
	struct xdr_netobj keyin = {
		.len	= ctx->gk5e->keylength,
		.data	= ctx->Ksess,
	};
	struct xdr_netobj keyout;
	int ret = -EINVAL;

	keyout.data = kmalloc(GSS_KRB5_MAX_KEYLEN, gfp_mask);
	if (!keyout.data)
		return -ENOMEM;

	/* initiator seal encryption */
	keyout.len = ctx->gk5e->Ke_length;
	if (krb5_derive_key(ctx, &keyin, &keyout, KG_USAGE_INITIATOR_SEAL,
			    KEY_USAGE_SEED_ENCRYPTION, gfp_mask))
		goto out;
	ctx->initiator_enc = gss_krb5_alloc_cipher_v2(ctx->gk5e->encrypt_name,
						      &keyout);
	if (ctx->initiator_enc == NULL)
		goto out;
	if (ctx->gk5e->aux_cipher) {
		ctx->initiator_enc_aux =
			gss_krb5_alloc_cipher_v2(ctx->gk5e->aux_cipher,
						 &keyout);
		if (ctx->initiator_enc_aux == NULL)
			goto out_free;
	}

	/* acceptor seal encryption */
	if (krb5_derive_key(ctx, &keyin, &keyout, KG_USAGE_ACCEPTOR_SEAL,
			    KEY_USAGE_SEED_ENCRYPTION, gfp_mask))
		goto out_free;
	ctx->acceptor_enc = gss_krb5_alloc_cipher_v2(ctx->gk5e->encrypt_name,
						     &keyout);
	if (ctx->acceptor_enc == NULL)
		goto out_free;
	if (ctx->gk5e->aux_cipher) {
		ctx->acceptor_enc_aux =
			gss_krb5_alloc_cipher_v2(ctx->gk5e->aux_cipher,
						 &keyout);
		if (ctx->acceptor_enc_aux == NULL)
			goto out_free;
	}

	/* initiator sign checksum */
	keyout.len = ctx->gk5e->Kc_length;
	if (krb5_derive_key(ctx, &keyin, &keyout, KG_USAGE_INITIATOR_SIGN,
			    KEY_USAGE_SEED_CHECKSUM, gfp_mask))
		goto out_free;
	ctx->initiator_sign = gss_krb5_alloc_hash_v2(ctx, &keyout);
	if (ctx->initiator_sign == NULL)
		goto out_free;

	/* acceptor sign checksum */
	if (krb5_derive_key(ctx, &keyin, &keyout, KG_USAGE_ACCEPTOR_SIGN,
			    KEY_USAGE_SEED_CHECKSUM, gfp_mask))
		goto out_free;
	ctx->acceptor_sign = gss_krb5_alloc_hash_v2(ctx, &keyout);
	if (ctx->acceptor_sign == NULL)
		goto out_free;

	/* initiator seal integrity */
	keyout.len = ctx->gk5e->Ki_length;
	if (krb5_derive_key(ctx, &keyin, &keyout, KG_USAGE_INITIATOR_SEAL,
			    KEY_USAGE_SEED_INTEGRITY, gfp_mask))
		goto out_free;
	ctx->initiator_integ = gss_krb5_alloc_hash_v2(ctx, &keyout);
	if (ctx->initiator_integ == NULL)
		goto out_free;

	/* acceptor seal integrity */
	if (krb5_derive_key(ctx, &keyin, &keyout, KG_USAGE_ACCEPTOR_SEAL,
			    KEY_USAGE_SEED_INTEGRITY, gfp_mask))
		goto out_free;
	ctx->acceptor_integ = gss_krb5_alloc_hash_v2(ctx, &keyout);
	if (ctx->acceptor_integ == NULL)
		goto out_free;

	ret = 0;
out:
	kfree_sensitive(keyout.data);
	return ret;

out_free:
	crypto_free_ahash(ctx->acceptor_integ);
	crypto_free_ahash(ctx->initiator_integ);
	crypto_free_ahash(ctx->acceptor_sign);
	crypto_free_ahash(ctx->initiator_sign);
	crypto_free_sync_skcipher(ctx->acceptor_enc_aux);
	crypto_free_sync_skcipher(ctx->acceptor_enc);
	crypto_free_sync_skcipher(ctx->initiator_enc_aux);
	crypto_free_sync_skcipher(ctx->initiator_enc);
	goto out;
}

#endif

=======

static int
context_derive_keys_new(struct krb5_ctx *ctx, gfp_t gfp_mask)
{
	struct xdr_netobj c, keyin, keyout;
	u8 cdata[GSS_KRB5_K5CLENGTH];
	u32 err;

	c.len = GSS_KRB5_K5CLENGTH;
	c.data = cdata;

	keyin.data = ctx->Ksess;
	keyin.len = ctx->gk5e->keylength;
	keyout.len = ctx->gk5e->keylength;

	/* initiator seal encryption */
	set_cdata(cdata, KG_USAGE_INITIATOR_SEAL, KEY_USAGE_SEED_ENCRYPTION);
	keyout.data = ctx->initiator_seal;
	err = krb5_derive_key(ctx->gk5e, &keyin, &keyout, &c, gfp_mask);
	if (err) {
		dprintk("%s: Error %d deriving initiator_seal key\n",
			__func__, err);
		goto out_err;
	}
	ctx->initiator_enc = context_v2_alloc_cipher(ctx,
						     ctx->gk5e->encrypt_name,
						     ctx->initiator_seal);
	if (ctx->initiator_enc == NULL)
		goto out_err;

	/* acceptor seal encryption */
	set_cdata(cdata, KG_USAGE_ACCEPTOR_SEAL, KEY_USAGE_SEED_ENCRYPTION);
	keyout.data = ctx->acceptor_seal;
	err = krb5_derive_key(ctx->gk5e, &keyin, &keyout, &c, gfp_mask);
	if (err) {
		dprintk("%s: Error %d deriving acceptor_seal key\n",
			__func__, err);
		goto out_free_initiator_enc;
	}
	ctx->acceptor_enc = context_v2_alloc_cipher(ctx,
						    ctx->gk5e->encrypt_name,
						    ctx->acceptor_seal);
	if (ctx->acceptor_enc == NULL)
		goto out_free_initiator_enc;

	/* initiator sign checksum */
	set_cdata(cdata, KG_USAGE_INITIATOR_SIGN, KEY_USAGE_SEED_CHECKSUM);
	keyout.data = ctx->initiator_sign;
	err = krb5_derive_key(ctx->gk5e, &keyin, &keyout, &c, gfp_mask);
	if (err) {
		dprintk("%s: Error %d deriving initiator_sign key\n",
			__func__, err);
		goto out_free_acceptor_enc;
	}

	/* acceptor sign checksum */
	set_cdata(cdata, KG_USAGE_ACCEPTOR_SIGN, KEY_USAGE_SEED_CHECKSUM);
	keyout.data = ctx->acceptor_sign;
	err = krb5_derive_key(ctx->gk5e, &keyin, &keyout, &c, gfp_mask);
	if (err) {
		dprintk("%s: Error %d deriving acceptor_sign key\n",
			__func__, err);
		goto out_free_acceptor_enc;
	}

	/* initiator seal integrity */
	set_cdata(cdata, KG_USAGE_INITIATOR_SEAL, KEY_USAGE_SEED_INTEGRITY);
	keyout.data = ctx->initiator_integ;
	err = krb5_derive_key(ctx->gk5e, &keyin, &keyout, &c, gfp_mask);
	if (err) {
		dprintk("%s: Error %d deriving initiator_integ key\n",
			__func__, err);
		goto out_free_acceptor_enc;
	}

	/* acceptor seal integrity */
	set_cdata(cdata, KG_USAGE_ACCEPTOR_SEAL, KEY_USAGE_SEED_INTEGRITY);
	keyout.data = ctx->acceptor_integ;
	err = krb5_derive_key(ctx->gk5e, &keyin, &keyout, &c, gfp_mask);
	if (err) {
		dprintk("%s: Error %d deriving acceptor_integ key\n",
			__func__, err);
		goto out_free_acceptor_enc;
	}

	switch (ctx->enctype) {
	case ENCTYPE_AES128_CTS_HMAC_SHA1_96:
	case ENCTYPE_AES256_CTS_HMAC_SHA1_96:
		ctx->initiator_enc_aux =
			context_v2_alloc_cipher(ctx, "cbc(aes)",
						ctx->initiator_seal);
		if (ctx->initiator_enc_aux == NULL)
			goto out_free_acceptor_enc;
		ctx->acceptor_enc_aux =
			context_v2_alloc_cipher(ctx, "cbc(aes)",
						ctx->acceptor_seal);
		if (ctx->acceptor_enc_aux == NULL) {
			crypto_free_sync_skcipher(ctx->initiator_enc_aux);
			goto out_free_acceptor_enc;
		}
	}

	return 0;

out_free_acceptor_enc:
	crypto_free_sync_skcipher(ctx->acceptor_enc);
out_free_initiator_enc:
	crypto_free_sync_skcipher(ctx->initiator_enc);
out_err:
	return -EINVAL;
}

>>>>>>> b7ba80a49124 (Commit)
static int
gss_import_v2_context(const void *p, const void *end, struct krb5_ctx *ctx,
		gfp_t gfp_mask)
{
	u64 seq_send64;
	int keylen;
	u32 time32;

	p = simple_get_bytes(p, end, &ctx->flags, sizeof(ctx->flags));
	if (IS_ERR(p))
		goto out_err;
	ctx->initiate = ctx->flags & KRB5_CTX_FLAG_INITIATOR;

	p = simple_get_bytes(p, end, &time32, sizeof(time32));
	if (IS_ERR(p))
		goto out_err;
	/* unsigned 32-bit time overflows in year 2106 */
	ctx->endtime = (time64_t)time32;
	p = simple_get_bytes(p, end, &seq_send64, sizeof(seq_send64));
	if (IS_ERR(p))
		goto out_err;
	atomic64_set(&ctx->seq_send64, seq_send64);
	/* set seq_send for use by "older" enctypes */
	atomic_set(&ctx->seq_send, seq_send64);
	if (seq_send64 != atomic_read(&ctx->seq_send)) {
		dprintk("%s: seq_send64 %llx, seq_send %x overflow?\n", __func__,
			seq_send64, atomic_read(&ctx->seq_send));
		p = ERR_PTR(-EINVAL);
		goto out_err;
	}
	p = simple_get_bytes(p, end, &ctx->enctype, sizeof(ctx->enctype));
	if (IS_ERR(p))
		goto out_err;
	/* Map ENCTYPE_DES3_CBC_SHA1 to ENCTYPE_DES3_CBC_RAW */
	if (ctx->enctype == ENCTYPE_DES3_CBC_SHA1)
		ctx->enctype = ENCTYPE_DES3_CBC_RAW;
<<<<<<< HEAD
	ctx->gk5e = gss_krb5_lookup_enctype(ctx->enctype);
=======
	ctx->gk5e = get_gss_krb5_enctype(ctx->enctype);
>>>>>>> b7ba80a49124 (Commit)
	if (ctx->gk5e == NULL) {
		dprintk("gss_kerberos_mech: unsupported krb5 enctype %u\n",
			ctx->enctype);
		p = ERR_PTR(-EINVAL);
		goto out_err;
	}
	keylen = ctx->gk5e->keylength;

	p = simple_get_bytes(p, end, ctx->Ksess, keylen);
	if (IS_ERR(p))
		goto out_err;

	if (p != end) {
		p = ERR_PTR(-EINVAL);
		goto out_err;
	}

	ctx->mech_used.data = kmemdup(gss_kerberos_mech.gm_oid.data,
				      gss_kerberos_mech.gm_oid.len, gfp_mask);
	if (unlikely(ctx->mech_used.data == NULL)) {
		p = ERR_PTR(-ENOMEM);
		goto out_err;
	}
	ctx->mech_used.len = gss_kerberos_mech.gm_oid.len;

<<<<<<< HEAD
	return ctx->gk5e->import_ctx(ctx, gfp_mask);
=======
	switch (ctx->enctype) {
	case ENCTYPE_DES3_CBC_RAW:
		return context_derive_keys_des3(ctx, gfp_mask);
	case ENCTYPE_AES128_CTS_HMAC_SHA1_96:
	case ENCTYPE_AES256_CTS_HMAC_SHA1_96:
		return context_derive_keys_new(ctx, gfp_mask);
	default:
		return -EINVAL;
	}
>>>>>>> b7ba80a49124 (Commit)

out_err:
	return PTR_ERR(p);
}

static int
<<<<<<< HEAD
gss_krb5_import_sec_context(const void *p, size_t len, struct gss_ctx *ctx_id,
			    time64_t *endtime, gfp_t gfp_mask)
=======
gss_import_sec_context_kerberos(const void *p, size_t len,
				struct gss_ctx *ctx_id,
				time64_t *endtime,
				gfp_t gfp_mask)
>>>>>>> b7ba80a49124 (Commit)
{
	const void *end = (const void *)((const char *)p + len);
	struct  krb5_ctx *ctx;
	int ret;

	ctx = kzalloc(sizeof(*ctx), gfp_mask);
	if (ctx == NULL)
		return -ENOMEM;

	if (len == 85)
		ret = gss_import_v1_context(p, end, ctx);
	else
		ret = gss_import_v2_context(p, end, ctx, gfp_mask);
<<<<<<< HEAD
	memzero_explicit(&ctx->Ksess, sizeof(ctx->Ksess));
	if (ret) {
		kfree(ctx);
		return ret;
	}

	ctx_id->internal_ctx_id = ctx;
	if (endtime)
		*endtime = ctx->endtime;
	return 0;
}

static void
gss_krb5_delete_sec_context(void *internal_ctx)
{
=======

	if (ret == 0) {
		ctx_id->internal_ctx_id = ctx;
		if (endtime)
			*endtime = ctx->endtime;
	} else
		kfree(ctx);

	dprintk("RPC:       %s: returning %d\n", __func__, ret);
	return ret;
}

static void
gss_delete_sec_context_kerberos(void *internal_ctx) {
>>>>>>> b7ba80a49124 (Commit)
	struct krb5_ctx *kctx = internal_ctx;

	crypto_free_sync_skcipher(kctx->seq);
	crypto_free_sync_skcipher(kctx->enc);
	crypto_free_sync_skcipher(kctx->acceptor_enc);
	crypto_free_sync_skcipher(kctx->initiator_enc);
	crypto_free_sync_skcipher(kctx->acceptor_enc_aux);
	crypto_free_sync_skcipher(kctx->initiator_enc_aux);
<<<<<<< HEAD
	crypto_free_ahash(kctx->acceptor_sign);
	crypto_free_ahash(kctx->initiator_sign);
	crypto_free_ahash(kctx->acceptor_integ);
	crypto_free_ahash(kctx->initiator_integ);
=======
>>>>>>> b7ba80a49124 (Commit)
	kfree(kctx->mech_used.data);
	kfree(kctx);
}

<<<<<<< HEAD
/**
 * gss_krb5_get_mic - get_mic for the Kerberos GSS mechanism
 * @gctx: GSS context
 * @text: plaintext to checksum
 * @token: buffer into which to write the computed checksum
 *
 * Return values:
 *    %GSS_S_COMPLETE - success, and @token is filled in
 *    %GSS_S_FAILURE - checksum could not be generated
 *    %GSS_S_CONTEXT_EXPIRED - Kerberos context is no longer valid
 */
static u32 gss_krb5_get_mic(struct gss_ctx *gctx, struct xdr_buf *text,
			    struct xdr_netobj *token)
{
	struct krb5_ctx *kctx = gctx->internal_ctx_id;

	return kctx->gk5e->get_mic(kctx, text, token);
}

/**
 * gss_krb5_verify_mic - verify_mic for the Kerberos GSS mechanism
 * @gctx: GSS context
 * @message_buffer: plaintext to check
 * @read_token: received checksum to check
 *
 * Return values:
 *    %GSS_S_COMPLETE - computed and received checksums match
 *    %GSS_S_DEFECTIVE_TOKEN - received checksum is not valid
 *    %GSS_S_BAD_SIG - computed and received checksums do not match
 *    %GSS_S_FAILURE - received checksum could not be checked
 *    %GSS_S_CONTEXT_EXPIRED - Kerberos context is no longer valid
 */
static u32 gss_krb5_verify_mic(struct gss_ctx *gctx,
			       struct xdr_buf *message_buffer,
			       struct xdr_netobj *read_token)
{
	struct krb5_ctx *kctx = gctx->internal_ctx_id;

	return kctx->gk5e->verify_mic(kctx, message_buffer, read_token);
}

/**
 * gss_krb5_wrap - gss_wrap for the Kerberos GSS mechanism
 * @gctx: initialized GSS context
 * @offset: byte offset in @buf to start writing the cipher text
 * @buf: OUT: send buffer
 * @pages: plaintext to wrap
 *
 * Return values:
 *    %GSS_S_COMPLETE - success, @buf has been updated
 *    %GSS_S_FAILURE - @buf could not be wrapped
 *    %GSS_S_CONTEXT_EXPIRED - Kerberos context is no longer valid
 */
static u32 gss_krb5_wrap(struct gss_ctx *gctx, int offset,
			 struct xdr_buf *buf, struct page **pages)
{
	struct krb5_ctx	*kctx = gctx->internal_ctx_id;

	return kctx->gk5e->wrap(kctx, offset, buf, pages);
}

/**
 * gss_krb5_unwrap - gss_unwrap for the Kerberos GSS mechanism
 * @gctx: initialized GSS context
 * @offset: starting byte offset into @buf
 * @len: size of ciphertext to unwrap
 * @buf: ciphertext to unwrap
 *
 * Return values:
 *    %GSS_S_COMPLETE - success, @buf has been updated
 *    %GSS_S_DEFECTIVE_TOKEN - received blob is not valid
 *    %GSS_S_BAD_SIG - computed and received checksums do not match
 *    %GSS_S_FAILURE - @buf could not be unwrapped
 *    %GSS_S_CONTEXT_EXPIRED - Kerberos context is no longer valid
 */
static u32 gss_krb5_unwrap(struct gss_ctx *gctx, int offset,
			   int len, struct xdr_buf *buf)
{
	struct krb5_ctx	*kctx = gctx->internal_ctx_id;

	return kctx->gk5e->unwrap(kctx, offset, len, buf,
				  &gctx->slack, &gctx->align);
}

static const struct gss_api_ops gss_kerberos_ops = {
	.gss_import_sec_context	= gss_krb5_import_sec_context,
	.gss_get_mic		= gss_krb5_get_mic,
	.gss_verify_mic		= gss_krb5_verify_mic,
	.gss_wrap		= gss_krb5_wrap,
	.gss_unwrap		= gss_krb5_unwrap,
	.gss_delete_sec_context	= gss_krb5_delete_sec_context,
=======
static const struct gss_api_ops gss_kerberos_ops = {
	.gss_import_sec_context	= gss_import_sec_context_kerberos,
	.gss_get_mic		= gss_get_mic_kerberos,
	.gss_verify_mic		= gss_verify_mic_kerberos,
	.gss_wrap		= gss_wrap_kerberos,
	.gss_unwrap		= gss_unwrap_kerberos,
	.gss_delete_sec_context	= gss_delete_sec_context_kerberos,
>>>>>>> b7ba80a49124 (Commit)
};

static struct pf_desc gss_kerberos_pfs[] = {
	[0] = {
		.pseudoflavor = RPC_AUTH_GSS_KRB5,
		.qop = GSS_C_QOP_DEFAULT,
		.service = RPC_GSS_SVC_NONE,
		.name = "krb5",
	},
	[1] = {
		.pseudoflavor = RPC_AUTH_GSS_KRB5I,
		.qop = GSS_C_QOP_DEFAULT,
		.service = RPC_GSS_SVC_INTEGRITY,
		.name = "krb5i",
		.datatouch = true,
	},
	[2] = {
		.pseudoflavor = RPC_AUTH_GSS_KRB5P,
		.qop = GSS_C_QOP_DEFAULT,
		.service = RPC_GSS_SVC_PRIVACY,
		.name = "krb5p",
		.datatouch = true,
	},
};

MODULE_ALIAS("rpc-auth-gss-krb5");
MODULE_ALIAS("rpc-auth-gss-krb5i");
MODULE_ALIAS("rpc-auth-gss-krb5p");
MODULE_ALIAS("rpc-auth-gss-390003");
MODULE_ALIAS("rpc-auth-gss-390004");
MODULE_ALIAS("rpc-auth-gss-390005");
MODULE_ALIAS("rpc-auth-gss-1.2.840.113554.1.2.2");

static struct gss_api_mech gss_kerberos_mech = {
	.gm_name	= "krb5",
	.gm_owner	= THIS_MODULE,
	.gm_oid		= { 9, "\x2a\x86\x48\x86\xf7\x12\x01\x02\x02" },
	.gm_ops		= &gss_kerberos_ops,
	.gm_pf_num	= ARRAY_SIZE(gss_kerberos_pfs),
	.gm_pfs		= gss_kerberos_pfs,
<<<<<<< HEAD
	.gm_upcall_enctypes = gss_krb5_enctype_priority_list,
=======
	.gm_upcall_enctypes = KRB5_SUPPORTED_ENCTYPES,
>>>>>>> b7ba80a49124 (Commit)
};

static int __init init_kerberos_module(void)
{
	int status;

<<<<<<< HEAD
	gss_krb5_prepare_enctype_priority_list();
=======
>>>>>>> b7ba80a49124 (Commit)
	status = gss_mech_register(&gss_kerberos_mech);
	if (status)
		printk("Failed to register kerberos gss mechanism!\n");
	return status;
}

static void __exit cleanup_kerberos_module(void)
{
	gss_mech_unregister(&gss_kerberos_mech);
}

MODULE_LICENSE("GPL");
module_init(init_kerberos_module);
module_exit(cleanup_kerberos_module);
