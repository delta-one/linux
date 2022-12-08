// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022 Linaro Ltd.
 * Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 */

#include <linux/errno.h>
#include <linux/mhi_ep.h>
#include "internal.h"

bool __must_check mhi_ep_check_mhi_state(struct mhi_ep_cntrl *mhi_cntrl,
					 enum mhi_state cur_mhi_state,
					 enum mhi_state mhi_state)
{
	if (mhi_state == MHI_STATE_SYS_ERR)
		return true;    /* Allowed in any state */

	if (mhi_state == MHI_STATE_READY)
		return cur_mhi_state == MHI_STATE_RESET;

	if (mhi_state == MHI_STATE_M0)
		return cur_mhi_state == MHI_STATE_M3 || cur_mhi_state == MHI_STATE_READY;

	if (mhi_state == MHI_STATE_M3)
		return cur_mhi_state == MHI_STATE_M0;

	return false;
}

int mhi_ep_set_mhi_state(struct mhi_ep_cntrl *mhi_cntrl, enum mhi_state mhi_state)
{
	struct device *dev = &mhi_cntrl->mhi_dev->dev;

	if (!mhi_ep_check_mhi_state(mhi_cntrl, mhi_cntrl->mhi_state, mhi_state)) {
		dev_err(dev, "MHI state change to %s from %s is not allowed!\n",
			mhi_state_str(mhi_state),
			mhi_state_str(mhi_cntrl->mhi_state));
		return -EACCES;
	}

	/* TODO: Add support for M1 and M2 states */
	if (mhi_state == MHI_STATE_M1 || mhi_state == MHI_STATE_M2) {
		dev_err(dev, "MHI state (%s) not supported\n", mhi_state_str(mhi_state));
		return -EOPNOTSUPP;
	}

	mhi_ep_mmio_masked_write(mhi_cntrl, EP_MHISTATUS, MHISTATUS_MHISTATE_MASK, mhi_state);
	mhi_cntrl->mhi_state = mhi_state;

	if (mhi_state == MHI_STATE_READY)
		mhi_ep_mmio_masked_write(mhi_cntrl, EP_MHISTATUS, MHISTATUS_READY_MASK, 1);

	if (mhi_state == MHI_STATE_SYS_ERR)
		mhi_ep_mmio_masked_write(mhi_cntrl, EP_MHISTATUS, MHISTATUS_SYSERR_MASK, 1);

	return 0;
}

int mhi_ep_set_m0_state(struct mhi_ep_cntrl *mhi_cntrl)
{
	struct device *dev = &mhi_cntrl->mhi_dev->dev;
	enum mhi_state old_state;
	int ret;

	/* If MHI is in M3, resume suspended channels */
<<<<<<< HEAD
	mutex_lock(&mhi_cntrl->state_lock);

=======
	spin_lock_bh(&mhi_cntrl->state_lock);
>>>>>>> b7ba80a49124 (Commit)
	old_state = mhi_cntrl->mhi_state;
	if (old_state == MHI_STATE_M3)
		mhi_ep_resume_channels(mhi_cntrl);

	ret = mhi_ep_set_mhi_state(mhi_cntrl, MHI_STATE_M0);
<<<<<<< HEAD
	if (ret) {
		mhi_ep_handle_syserr(mhi_cntrl);
		goto err_unlock;
=======
	spin_unlock_bh(&mhi_cntrl->state_lock);

	if (ret) {
		mhi_ep_handle_syserr(mhi_cntrl);
		return ret;
>>>>>>> b7ba80a49124 (Commit)
	}

	/* Signal host that the device moved to M0 */
	ret = mhi_ep_send_state_change_event(mhi_cntrl, MHI_STATE_M0);
	if (ret) {
		dev_err(dev, "Failed sending M0 state change event\n");
<<<<<<< HEAD
		goto err_unlock;
=======
		return ret;
>>>>>>> b7ba80a49124 (Commit)
	}

	if (old_state == MHI_STATE_READY) {
		/* Send AMSS EE event to host */
		ret = mhi_ep_send_ee_event(mhi_cntrl, MHI_EE_AMSS);
		if (ret) {
			dev_err(dev, "Failed sending AMSS EE event\n");
<<<<<<< HEAD
			goto err_unlock;
		}
	}

err_unlock:
	mutex_unlock(&mhi_cntrl->state_lock);

	return ret;
=======
			return ret;
		}
	}

	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

int mhi_ep_set_m3_state(struct mhi_ep_cntrl *mhi_cntrl)
{
	struct device *dev = &mhi_cntrl->mhi_dev->dev;
	int ret;

<<<<<<< HEAD
	mutex_lock(&mhi_cntrl->state_lock);

	ret = mhi_ep_set_mhi_state(mhi_cntrl, MHI_STATE_M3);
	if (ret) {
		mhi_ep_handle_syserr(mhi_cntrl);
		goto err_unlock;
=======
	spin_lock_bh(&mhi_cntrl->state_lock);
	ret = mhi_ep_set_mhi_state(mhi_cntrl, MHI_STATE_M3);
	spin_unlock_bh(&mhi_cntrl->state_lock);

	if (ret) {
		mhi_ep_handle_syserr(mhi_cntrl);
		return ret;
>>>>>>> b7ba80a49124 (Commit)
	}

	mhi_ep_suspend_channels(mhi_cntrl);

	/* Signal host that the device moved to M3 */
	ret = mhi_ep_send_state_change_event(mhi_cntrl, MHI_STATE_M3);
	if (ret) {
		dev_err(dev, "Failed sending M3 state change event\n");
<<<<<<< HEAD
		goto err_unlock;
	}

err_unlock:
	mutex_unlock(&mhi_cntrl->state_lock);

	return ret;
=======
		return ret;
	}

	return 0;
>>>>>>> b7ba80a49124 (Commit)
}

int mhi_ep_set_ready_state(struct mhi_ep_cntrl *mhi_cntrl)
{
	struct device *dev = &mhi_cntrl->mhi_dev->dev;
	enum mhi_state mhi_state;
	int ret, is_ready;

<<<<<<< HEAD
	mutex_lock(&mhi_cntrl->state_lock);

=======
	spin_lock_bh(&mhi_cntrl->state_lock);
>>>>>>> b7ba80a49124 (Commit)
	/* Ensure that the MHISTATUS is set to RESET by host */
	mhi_state = mhi_ep_mmio_masked_read(mhi_cntrl, EP_MHISTATUS, MHISTATUS_MHISTATE_MASK);
	is_ready = mhi_ep_mmio_masked_read(mhi_cntrl, EP_MHISTATUS, MHISTATUS_READY_MASK);

	if (mhi_state != MHI_STATE_RESET || is_ready) {
		dev_err(dev, "READY state transition failed. MHI host not in RESET state\n");
<<<<<<< HEAD
		ret = -EIO;
		goto err_unlock;
	}

	ret = mhi_ep_set_mhi_state(mhi_cntrl, MHI_STATE_READY);
	if (ret)
		mhi_ep_handle_syserr(mhi_cntrl);

err_unlock:
	mutex_unlock(&mhi_cntrl->state_lock);

=======
		spin_unlock_bh(&mhi_cntrl->state_lock);
		return -EIO;
	}

	ret = mhi_ep_set_mhi_state(mhi_cntrl, MHI_STATE_READY);
	spin_unlock_bh(&mhi_cntrl->state_lock);

	if (ret)
		mhi_ep_handle_syserr(mhi_cntrl);

>>>>>>> b7ba80a49124 (Commit)
	return ret;
}
