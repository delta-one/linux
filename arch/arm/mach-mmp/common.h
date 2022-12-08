/* SPDX-License-Identifier: GPL-2.0 */
#include <linux/reboot.h>
<<<<<<< HEAD
=======
#define ARRAY_AND_SIZE(x)	(x), ARRAY_SIZE(x)
>>>>>>> b7ba80a49124 (Commit)

extern void mmp_timer_init(int irq, unsigned long rate);

extern void __init mmp_map_io(void);
extern void __init mmp2_map_io(void);
<<<<<<< HEAD
=======
extern void mmp_restart(enum reboot_mode, const char *);
>>>>>>> b7ba80a49124 (Commit)
