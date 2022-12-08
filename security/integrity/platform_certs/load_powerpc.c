// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 IBM Corporation
 * Author: Nayna Jain
 *
 *      - loads keys and hashes stored and controlled by the firmware.
 */
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/err.h>
#include <linux/slab.h>
<<<<<<< HEAD
#include <asm/secure_boot.h>
#include <asm/secvar.h>
#include "keyring_handler.h"
#include "../integrity.h"

/*
 * Get a certificate list blob from the named secure variable.
 *
 * Returns:
 *  - a pointer to a kmalloc'd buffer containing the cert list on success
 *  - NULL if the key does not exist
 *  - an ERR_PTR on error
 */
static __init void *get_cert_list(u8 *key, unsigned long keylen, u64 *size)
=======
#include <linux/of.h>
#include <asm/secure_boot.h>
#include <asm/secvar.h>
#include "keyring_handler.h"

/*
 * Get a certificate list blob from the named secure variable.
 */
static __init void *get_cert_list(u8 *key, unsigned long keylen, uint64_t *size)
>>>>>>> b7ba80a49124 (Commit)
{
	int rc;
	void *db;

	rc = secvar_ops->get(key, keylen, NULL, size);
	if (rc) {
<<<<<<< HEAD
		if (rc == -ENOENT)
			return NULL;
		return ERR_PTR(rc);
=======
		pr_err("Couldn't get size: %d\n", rc);
		return NULL;
>>>>>>> b7ba80a49124 (Commit)
	}

	db = kmalloc(*size, GFP_KERNEL);
	if (!db)
<<<<<<< HEAD
		return ERR_PTR(-ENOMEM);
=======
		return NULL;
>>>>>>> b7ba80a49124 (Commit)

	rc = secvar_ops->get(key, keylen, db, size);
	if (rc) {
		kfree(db);
<<<<<<< HEAD
		return ERR_PTR(rc);
=======
		pr_err("Error reading %s var: %d\n", key, rc);
		return NULL;
>>>>>>> b7ba80a49124 (Commit)
	}

	return db;
}

/*
 * Load the certs contained in the keys databases into the platform trusted
 * keyring and the blacklisted X.509 cert SHA256 hashes into the blacklist
 * keyring.
 */
static int __init load_powerpc_certs(void)
{
	void *db = NULL, *dbx = NULL;
<<<<<<< HEAD
	u64 dbsize = 0, dbxsize = 0;
	int rc = 0;
	ssize_t len;
	char buf[32];
=======
	uint64_t dbsize = 0, dbxsize = 0;
	int rc = 0;
	struct device_node *node;
>>>>>>> b7ba80a49124 (Commit)

	if (!secvar_ops)
		return -ENODEV;

<<<<<<< HEAD
	len = secvar_ops->format(buf, sizeof(buf));
	if (len <= 0)
		return -ENODEV;

	// Check for known secure boot implementations from OPAL or PLPKS
	if (strcmp("ibm,edk2-compat-v1", buf) && strcmp("ibm,plpks-sb-v1", buf)) {
		pr_err("Unsupported secvar implementation \"%s\", not loading certs\n", buf);
		return -ENODEV;
	}

=======
	/* The following only applies for the edk2-compat backend. */
	node = of_find_compatible_node(NULL, NULL, "ibm,edk2-compat-v1");
	if (!node)
		return -ENODEV;

>>>>>>> b7ba80a49124 (Commit)
	/*
	 * Get db, and dbx. They might not exist, so it isn't an error if we
	 * can't get them.
	 */
	db = get_cert_list("db", 3, &dbsize);
	if (!db) {
<<<<<<< HEAD
		pr_info("Couldn't get db list from firmware\n");
	} else if (IS_ERR(db)) {
		rc = PTR_ERR(db);
		pr_err("Error reading db from firmware: %d\n", rc);
		return rc;
=======
		pr_err("Couldn't get db list from firmware\n");
>>>>>>> b7ba80a49124 (Commit)
	} else {
		rc = parse_efi_signature_list("powerpc:db", db, dbsize,
					      get_handler_for_db);
		if (rc)
			pr_err("Couldn't parse db signatures: %d\n", rc);
		kfree(db);
	}

	dbx = get_cert_list("dbx", 4,  &dbxsize);
	if (!dbx) {
		pr_info("Couldn't get dbx list from firmware\n");
<<<<<<< HEAD
	} else if (IS_ERR(dbx)) {
		rc = PTR_ERR(dbx);
		pr_err("Error reading dbx from firmware: %d\n", rc);
		return rc;
=======
>>>>>>> b7ba80a49124 (Commit)
	} else {
		rc = parse_efi_signature_list("powerpc:dbx", dbx, dbxsize,
					      get_handler_for_dbx);
		if (rc)
			pr_err("Couldn't parse dbx signatures: %d\n", rc);
		kfree(dbx);
	}

<<<<<<< HEAD
=======
	of_node_put(node);

>>>>>>> b7ba80a49124 (Commit)
	return rc;
}
late_initcall(load_powerpc_certs);
