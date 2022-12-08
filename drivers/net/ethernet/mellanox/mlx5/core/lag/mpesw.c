// SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB
/* Copyright (c) 2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved. */

#include <linux/netdevice.h>
#include <net/nexthop.h>
#include "lag/lag.h"
#include "eswitch.h"
<<<<<<< HEAD
#include "esw/acl/ofld.h"
#include "lib/mlx5.h"

static void mlx5_mpesw_metadata_cleanup(struct mlx5_lag *ldev)
{
	struct mlx5_core_dev *dev;
	struct mlx5_eswitch *esw;
	u32 pf_metadata;
	int i;

	for (i = 0; i < ldev->ports; i++) {
		dev = ldev->pf[i].dev;
		esw = dev->priv.eswitch;
		pf_metadata = ldev->lag_mpesw.pf_metadata[i];
		if (!pf_metadata)
			continue;
		mlx5_esw_acl_ingress_vport_metadata_update(esw, MLX5_VPORT_UPLINK, 0);
		mlx5_notifier_call_chain(dev->priv.events, MLX5_DEV_EVENT_MULTIPORT_ESW,
					 (void *)0);
		mlx5_esw_match_metadata_free(esw, pf_metadata);
		ldev->lag_mpesw.pf_metadata[i] = 0;
	}
}

static int mlx5_mpesw_metadata_set(struct mlx5_lag *ldev)
{
	struct mlx5_core_dev *dev;
	struct mlx5_eswitch *esw;
	u32 pf_metadata;
	int i, err;

	for (i = 0; i < ldev->ports; i++) {
		dev = ldev->pf[i].dev;
		esw = dev->priv.eswitch;
		pf_metadata = mlx5_esw_match_metadata_alloc(esw);
		if (!pf_metadata) {
			err = -ENOSPC;
			goto err_metadata;
		}

		ldev->lag_mpesw.pf_metadata[i] = pf_metadata;
		err = mlx5_esw_acl_ingress_vport_metadata_update(esw, MLX5_VPORT_UPLINK,
								 pf_metadata);
		if (err)
			goto err_metadata;
	}

	for (i = 0; i < ldev->ports; i++) {
		dev = ldev->pf[i].dev;
		mlx5_notifier_call_chain(dev->priv.events, MLX5_DEV_EVENT_MULTIPORT_ESW,
					 (void *)0);
	}

	return 0;

err_metadata:
	mlx5_mpesw_metadata_cleanup(ldev);
	return err;
}

static int enable_mpesw(struct mlx5_lag *ldev)
{
	struct mlx5_core_dev *dev0 = ldev->pf[MLX5_LAG_P1].dev;
	struct mlx5_core_dev *dev1 = ldev->pf[MLX5_LAG_P2].dev;
	int err;

	if (ldev->mode != MLX5_LAG_MODE_NONE)
		return -EINVAL;

	if (mlx5_eswitch_mode(dev0) != MLX5_ESWITCH_OFFLOADS ||
	    !MLX5_CAP_PORT_SELECTION(dev0, port_select_flow_table) ||
	    !MLX5_CAP_GEN(dev0, create_lag_when_not_master_up) ||
	    !mlx5_lag_check_prereq(ldev))
		return -EOPNOTSUPP;

	err = mlx5_mpesw_metadata_set(ldev);
	if (err)
		return err;

	mlx5_lag_remove_devices(ldev);

	err = mlx5_activate_lag(ldev, NULL, MLX5_LAG_MODE_MPESW, true);
	if (err) {
		mlx5_core_warn(dev0, "Failed to create LAG in MPESW mode (%d)\n", err);
		goto err_add_devices;
	}

	dev0->priv.flags &= ~MLX5_PRIV_FLAGS_DISABLE_IB_ADEV;
	mlx5_rescan_drivers_locked(dev0);
	err = mlx5_eswitch_reload_reps(dev0->priv.eswitch);
	if (!err)
		err = mlx5_eswitch_reload_reps(dev1->priv.eswitch);
	if (err)
		goto err_rescan_drivers;

	return 0;

err_rescan_drivers:
	dev0->priv.flags |= MLX5_PRIV_FLAGS_DISABLE_IB_ADEV;
	mlx5_rescan_drivers_locked(dev0);
	mlx5_deactivate_lag(ldev);
err_add_devices:
	mlx5_lag_add_devices(ldev);
	mlx5_eswitch_reload_reps(dev0->priv.eswitch);
	mlx5_eswitch_reload_reps(dev1->priv.eswitch);
	mlx5_mpesw_metadata_cleanup(ldev);
	return err;
}

static void disable_mpesw(struct mlx5_lag *ldev)
{
	if (ldev->mode == MLX5_LAG_MODE_MPESW) {
		mlx5_mpesw_metadata_cleanup(ldev);
		mlx5_disable_lag(ldev);
	}
}

static void mlx5_mpesw_work(struct work_struct *work)
{
	struct mlx5_mpesw_work_st *mpesww = container_of(work, struct mlx5_mpesw_work_st, work);
	struct mlx5_lag *ldev = mpesww->lag;

	mlx5_dev_list_lock();
	mutex_lock(&ldev->lock);
	if (ldev->mode_changes_in_progress) {
		mpesww->result = -EAGAIN;
		goto unlock;
	}

	if (mpesww->op == MLX5_MPESW_OP_ENABLE)
		mpesww->result = enable_mpesw(ldev);
	else if (mpesww->op == MLX5_MPESW_OP_DISABLE)
		disable_mpesw(ldev);
unlock:
	mutex_unlock(&ldev->lock);
	mlx5_dev_list_unlock();
	complete(&mpesww->comp);
}

static int mlx5_lag_mpesw_queue_work(struct mlx5_core_dev *dev,
				     enum mpesw_op op)
{
	struct mlx5_lag *ldev = mlx5_lag_dev(dev);
	struct mlx5_mpesw_work_st *work;
=======
#include "lib/mlx5.h"

void mlx5_mpesw_work(struct work_struct *work)
{
	struct mlx5_lag *ldev = container_of(work, struct mlx5_lag, mpesw_work);

	mutex_lock(&ldev->lock);
	mlx5_disable_lag(ldev);
	mutex_unlock(&ldev->lock);
}

static void mlx5_lag_disable_mpesw(struct mlx5_core_dev *dev)
{
	struct mlx5_lag *ldev = dev->priv.lag;

	if (!queue_work(ldev->wq, &ldev->mpesw_work))
		mlx5_core_warn(dev, "failed to queue work\n");
}

void mlx5_lag_del_mpesw_rule(struct mlx5_core_dev *dev)
{
	struct mlx5_lag *ldev = dev->priv.lag;

	if (!ldev)
		return;

	mutex_lock(&ldev->lock);
	if (!atomic_dec_return(&ldev->lag_mpesw.mpesw_rule_count) &&
	    ldev->mode == MLX5_LAG_MODE_MPESW)
		mlx5_lag_disable_mpesw(dev);
	mutex_unlock(&ldev->lock);
}

int mlx5_lag_add_mpesw_rule(struct mlx5_core_dev *dev)
{
	struct mlx5_lag *ldev = dev->priv.lag;
>>>>>>> b7ba80a49124 (Commit)
	int err = 0;

	if (!ldev)
		return 0;

<<<<<<< HEAD
	work = kzalloc(sizeof(*work), GFP_KERNEL);
	if (!work)
		return -ENOMEM;

	INIT_WORK(&work->work, mlx5_mpesw_work);
	init_completion(&work->comp);
	work->op = op;
	work->lag = ldev;

	if (!queue_work(ldev->wq, &work->work)) {
		mlx5_core_warn(dev, "failed to queue mpesw work\n");
		err = -EINVAL;
		goto out;
	}
	wait_for_completion(&work->comp);
	err = work->result;
out:
	kfree(work);
	return err;
}

void mlx5_lag_mpesw_disable(struct mlx5_core_dev *dev)
{
	mlx5_lag_mpesw_queue_work(dev, MLX5_MPESW_OP_DISABLE);
}

int mlx5_lag_mpesw_enable(struct mlx5_core_dev *dev)
{
	return mlx5_lag_mpesw_queue_work(dev, MLX5_MPESW_OP_ENABLE);
}

int mlx5_lag_mpesw_do_mirred(struct mlx5_core_dev *mdev,
			     struct net_device *out_dev,
			     struct netlink_ext_ack *extack)
{
	struct mlx5_lag *ldev = mlx5_lag_dev(mdev);
=======
	mutex_lock(&ldev->lock);
	if (atomic_add_return(1, &ldev->lag_mpesw.mpesw_rule_count) != 1)
		goto out;

	if (ldev->mode != MLX5_LAG_MODE_NONE) {
		err = -EINVAL;
		goto out;
	}

	err = mlx5_activate_lag(ldev, NULL, MLX5_LAG_MODE_MPESW, false);
	if (err)
		mlx5_core_warn(dev, "Failed to create LAG in MPESW mode (%d)\n", err);

out:
	mutex_unlock(&ldev->lock);
	return err;
}

int mlx5_lag_do_mirred(struct mlx5_core_dev *mdev, struct net_device *out_dev)
{
	struct mlx5_lag *ldev = mdev->priv.lag;
>>>>>>> b7ba80a49124 (Commit)

	if (!netif_is_bond_master(out_dev) || !ldev)
		return 0;

<<<<<<< HEAD
	if (ldev->mode != MLX5_LAG_MODE_MPESW)
		return 0;

	NL_SET_ERR_MSG_MOD(extack, "can't forward to bond in mpesw mode");
	return -EOPNOTSUPP;
}

bool mlx5_lag_is_mpesw(struct mlx5_core_dev *dev)
{
	struct mlx5_lag *ldev = mlx5_lag_dev(dev);

	return ldev && ldev->mode == MLX5_LAG_MODE_MPESW;
}
EXPORT_SYMBOL(mlx5_lag_is_mpesw);
=======
	mutex_lock(&ldev->lock);
	if (ldev->mode == MLX5_LAG_MODE_MPESW) {
		mutex_unlock(&ldev->lock);
		return -EOPNOTSUPP;
	}
	mutex_unlock(&ldev->lock);
	return 0;
}

bool mlx5_lag_mpesw_is_activated(struct mlx5_core_dev *dev)
{
	bool ret;

	ret = dev->priv.lag && dev->priv.lag->mode == MLX5_LAG_MODE_MPESW;
	return ret;
}

void mlx5_lag_mpesw_init(struct mlx5_lag *ldev)
{
	INIT_WORK(&ldev->mpesw_work, mlx5_mpesw_work);
	atomic_set(&ldev->lag_mpesw.mpesw_rule_count, 0);
}

void mlx5_lag_mpesw_cleanup(struct mlx5_lag *ldev)
{
	cancel_delayed_work_sync(&ldev->bond_work);
}
>>>>>>> b7ba80a49124 (Commit)
