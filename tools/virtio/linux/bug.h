/* SPDX-License-Identifier: GPL-2.0 */
<<<<<<< HEAD
#ifndef _LINUX_BUG_H
#define _LINUX_BUG_H
=======
#ifndef BUG_H
#define BUG_H
>>>>>>> b7ba80a49124 (Commit)

#include <asm/bug.h>

#define BUG_ON(__BUG_ON_cond) assert(!(__BUG_ON_cond))

<<<<<<< HEAD
#define BUG() abort()

#endif /* _LINUX_BUG_H */
=======
#define BUILD_BUG_ON(x)

#define BUG() abort()

#endif /* BUG_H */
>>>>>>> b7ba80a49124 (Commit)
