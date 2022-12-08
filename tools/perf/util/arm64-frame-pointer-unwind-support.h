/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __PERF_ARM_FRAME_POINTER_UNWIND_SUPPORT_H
#define __PERF_ARM_FRAME_POINTER_UNWIND_SUPPORT_H

<<<<<<< HEAD
#include <linux/types.h>

struct perf_sample;
struct thread;
=======
#include "event.h"
#include "thread.h"
>>>>>>> b7ba80a49124 (Commit)

u64 get_leaf_frame_caller_aarch64(struct perf_sample *sample, struct thread *thread, int user_idx);

#endif /* __PERF_ARM_FRAME_POINTER_UNWIND_SUPPORT_H */
