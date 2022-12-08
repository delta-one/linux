#if defined(__aarch64__)
#include "../../arch/arm64/include/uapi/asm/bpf_perf_event.h"
#elif defined(__arc__)
#include "../../arch/arc/include/uapi/asm/bpf_perf_event.h"
#elif defined(__s390__)
#include "../../arch/s390/include/uapi/asm/bpf_perf_event.h"
#elif defined(__riscv)
#include "../../arch/riscv/include/uapi/asm/bpf_perf_event.h"
<<<<<<< HEAD
#elif defined(__loongarch__)
#include "../../arch/loongarch/include/uapi/asm/bpf_perf_event.h"
=======
>>>>>>> b7ba80a49124 (Commit)
#else
#include <uapi/asm-generic/bpf_perf_event.h>
#endif
