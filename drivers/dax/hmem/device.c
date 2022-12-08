// SPDX-License-Identifier: GPL-2.0
#include <linux/platform_device.h>
#include <linux/memregion.h>
#include <linux/module.h>
#include <linux/dax.h>
#include <linux/mm.h>

static bool nohmem;
module_param_named(disable, nohmem, bool, 0444);

<<<<<<< HEAD
static bool platform_initialized;
static DEFINE_MUTEX(hmem_resource_lock);
static struct resource hmem_active = {
	.name = "HMEM devices",
	.start = 0,
	.end = -1,
	.flags = IORESOURCE_MEM,
};

int walk_hmem_resources(struct device *host, walk_hmem_fn fn)
{
	struct resource *res;
	int rc = 0;

	mutex_lock(&hmem_resource_lock);
	for (res = hmem_active.child; res; res = res->sibling) {
		rc = fn(host, (int) res->desc, res);
		if (rc)
			break;
	}
	mutex_unlock(&hmem_resource_lock);
	return rc;
}
EXPORT_SYMBOL_GPL(walk_hmem_resources);

static void __hmem_register_resource(int target_nid, struct resource *res)
{
	struct platform_device *pdev;
	struct resource *new;
	int rc;

	new = __request_region(&hmem_active, res->start, resource_size(res), "",
			       0);
	if (!new) {
		pr_debug("hmem range %pr already active\n", res);
		return;
	}

	new->desc = target_nid;

	if (platform_initialized)
		return;

	pdev = platform_device_alloc("hmem_platform", 0);
	if (!pdev) {
		pr_err_once("failed to register device-dax hmem_platform device\n");
		return;
	}

	rc = platform_device_add(pdev);
	if (rc)
		platform_device_put(pdev);
	else
		platform_initialized = true;
}

void hmem_register_resource(int target_nid, struct resource *res)
{
	if (nohmem)
		return;

	mutex_lock(&hmem_resource_lock);
	__hmem_register_resource(target_nid, res);
	mutex_unlock(&hmem_resource_lock);
=======
void hmem_register_device(int target_nid, struct resource *r)
{
	/* define a clean / non-busy resource for the platform device */
	struct resource res = {
		.start = r->start,
		.end = r->end,
		.flags = IORESOURCE_MEM,
	};
	struct platform_device *pdev;
	struct memregion_info info;
	int rc, id;

	if (nohmem)
		return;

	rc = region_intersects(res.start, resource_size(&res), IORESOURCE_MEM,
			IORES_DESC_SOFT_RESERVED);
	if (rc != REGION_INTERSECTS)
		return;

	id = memregion_alloc(GFP_KERNEL);
	if (id < 0) {
		pr_err("memregion allocation failure for %pr\n", &res);
		return;
	}

	pdev = platform_device_alloc("hmem", id);
	if (!pdev) {
		pr_err("hmem device allocation failure for %pr\n", &res);
		goto out_pdev;
	}

	pdev->dev.numa_node = numa_map_to_online_node(target_nid);
	info = (struct memregion_info) {
		.target_node = target_nid,
	};
	rc = platform_device_add_data(pdev, &info, sizeof(info));
	if (rc < 0) {
		pr_err("hmem memregion_info allocation failure for %pr\n", &res);
		goto out_pdev;
	}

	rc = platform_device_add_resources(pdev, &res, 1);
	if (rc < 0) {
		pr_err("hmem resource allocation failure for %pr\n", &res);
		goto out_resource;
	}

	rc = platform_device_add(pdev);
	if (rc < 0) {
		dev_err(&pdev->dev, "device add failed for %pr\n", &res);
		goto out_resource;
	}

	return;

out_resource:
	put_device(&pdev->dev);
out_pdev:
	memregion_free(id);
>>>>>>> b7ba80a49124 (Commit)
}

static __init int hmem_register_one(struct resource *res, void *data)
{
<<<<<<< HEAD
	hmem_register_resource(phys_to_target_node(res->start), res);
=======
	/*
	 * If the resource is not a top-level resource it was already
	 * assigned to a device by the HMAT parsing.
	 */
	if (res->parent != &iomem_resource) {
		pr_info("HMEM: skip %pr, already claimed\n", res);
		return 0;
	}

	hmem_register_device(phys_to_target_node(res->start), res);
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

static __init int hmem_init(void)
{
	walk_iomem_res_desc(IORES_DESC_SOFT_RESERVED,
			IORESOURCE_MEM, 0, -1, NULL, hmem_register_one);
	return 0;
}

/*
 * As this is a fallback for address ranges unclaimed by the ACPI HMAT
 * parsing it must be at an initcall level greater than hmat_init().
 */
<<<<<<< HEAD
device_initcall(hmem_init);
=======
late_initcall(hmem_init);
>>>>>>> b7ba80a49124 (Commit)
