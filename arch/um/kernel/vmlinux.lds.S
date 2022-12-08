<<<<<<< HEAD
#define RUNTIME_DISCARD_EXIT
=======

>>>>>>> b7ba80a49124 (Commit)
KERNEL_STACK_SIZE = 4096 * (1 << CONFIG_KERNEL_STACK_ORDER);

#ifdef CONFIG_LD_SCRIPT_STATIC
#include "uml.lds.S"
#else
#include "dyn.lds.S"
#endif
