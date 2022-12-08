// SPDX-License-Identifier: GPL-2.0-only
/******************************************************************************
 *
 * Copyright(c) 2009 - 2014 Intel Corporation. All rights reserved.
 * Copyright (C) 2018 Intel Corporation
 *****************************************************************************/

#include <linux/module.h>

/* sparse doesn't like tracepoint macros */
#ifndef __CHECKER__
#include "iwl-trans.h"

#define CREATE_TRACE_POINTS
<<<<<<< HEAD
#ifdef CONFIG_CC_IS_GCC
#pragma GCC diagnostic ignored "-Wsuggest-attribute=format"
#endif
=======
>>>>>>> b7ba80a49124 (Commit)
#include "iwl-devtrace.h"

EXPORT_TRACEPOINT_SYMBOL(iwlwifi_dev_ucode_event);
EXPORT_TRACEPOINT_SYMBOL(iwlwifi_dev_ucode_cont_event);
EXPORT_TRACEPOINT_SYMBOL(iwlwifi_dev_ucode_wrap_event);
#endif
