================
MSR Trace Events
================

The x86 kernel supports tracing most MSR (Model Specific Register) accesses.
To see the definition of the MSRs on Intel systems please see the SDM
at https://www.intel.com/sdm (Volume 3)

Available trace points:

<<<<<<< HEAD
/sys/kernel/tracing/events/msr/
=======
/sys/kernel/debug/tracing/events/msr/
>>>>>>> b7ba80a49124 (Commit)

Trace MSR reads:

read_msr

  - msr: MSR number
  - val: Value written
  - failed: 1 if the access failed, otherwise 0


Trace MSR writes:

write_msr

  - msr: MSR number
  - val: Value written
  - failed: 1 if the access failed, otherwise 0


Trace RDPMC in kernel:

rdpmc

The trace data can be post processed with the postprocess/decode_msr.py script::

<<<<<<< HEAD
  cat /sys/kernel/tracing/trace | decode_msr.py /usr/src/linux/include/asm/msr-index.h
=======
  cat /sys/kernel/debug/tracing/trace | decode_msr.py /usr/src/linux/include/asm/msr-index.h
>>>>>>> b7ba80a49124 (Commit)

to add symbolic MSR names.

