/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_LINKAGE_H
#define __ASM_LINKAGE_H

#include <linux/stringify.h>

<<<<<<< HEAD
#define __ALIGN .balign CONFIG_FUNCTION_ALIGNMENT, 0x07
=======
#define __ALIGN .align 16, 0x07
>>>>>>> b7ba80a49124 (Commit)
#define __ALIGN_STR __stringify(__ALIGN)

#endif
