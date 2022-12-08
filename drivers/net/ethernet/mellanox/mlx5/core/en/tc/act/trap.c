// SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB
// Copyright (c) 2021, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

#include "act.h"
#include "en/tc_priv.h"
<<<<<<< HEAD
#include "eswitch.h"
=======
>>>>>>> b7ba80a49124 (Commit)

static bool
tc_act_can_offload_trap(struct mlx5e_tc_act_parse_state *parse_state,
			const struct flow_action_entry *act,
			int act_index,
			struct mlx5_flow_attr *attr)
{
<<<<<<< HEAD
=======
	struct netlink_ext_ack *extack = parse_state->extack;

	if (parse_state->flow_action->num_entries != 1) {
		NL_SET_ERR_MSG_MOD(extack, "action trap is supported as a sole action only");
		return false;
	}

>>>>>>> b7ba80a49124 (Commit)
	return true;
}

static int
tc_act_parse_trap(struct mlx5e_tc_act_parse_state *parse_state,
		  const struct flow_action_entry *act,
		  struct mlx5e_priv *priv,
		  struct mlx5_flow_attr *attr)
{
	attr->action |= MLX5_FLOW_CONTEXT_ACTION_FWD_DEST;
<<<<<<< HEAD
	attr->dest_ft = mlx5_eswitch_get_slow_fdb(priv->mdev->priv.eswitch);
=======
	attr->flags |= MLX5_ATTR_FLAG_SLOW_PATH;
>>>>>>> b7ba80a49124 (Commit)

	return 0;
}

struct mlx5e_tc_act mlx5e_tc_act_trap = {
	.can_offload = tc_act_can_offload_trap,
	.parse_action = tc_act_parse_trap,
};
