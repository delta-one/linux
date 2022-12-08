====================
rtla-timerlat-top
====================
-------------------------------------------
Measures the operating system timer latency
-------------------------------------------

:Manual section: 1

SYNOPSIS
========
**rtla timerlat top** [*OPTIONS*] ...

DESCRIPTION
===========

.. include:: common_timerlat_description.rst

The **rtla timerlat top** displays a summary of the periodic output
from the *timerlat* tracer. It also provides information for each
operating system noise via the **osnoise:** tracepoints that can be
seem with the option **-T**.

OPTIONS
=======

.. include:: common_timerlat_options.rst

.. include:: common_top_options.rst

.. include:: common_options.rst

<<<<<<< HEAD
.. include:: common_timerlat_aa.rst

EXAMPLE
=======

In the example below, the timerlat tracer is dispatched in cpus *1-23* in the
automatic trace mode, instructing the tracer to stop if a *40 us* latency or
higher is found::

  # timerlat -a 40 -c 1-23 -q
                                     Timer Latency
    0 00:00:12   |          IRQ Timer Latency (us)        |         Thread Timer Latency (us)
  CPU COUNT      |      cur       min       avg       max |      cur       min       avg       max
    1 #12322     |        0         0         1        15 |       10         3         9        31
    2 #12322     |        3         0         1        12 |       10         3         9        23
    3 #12322     |        1         0         1        21 |        8         2         8        34
    4 #12322     |        1         0         1        17 |       10         2        11        33
    5 #12322     |        0         0         1        12 |        8         3         8        25
    6 #12322     |        1         0         1        14 |       16         3        11        35
    7 #12322     |        0         0         1        14 |        9         2         8        29
    8 #12322     |        1         0         1        22 |        9         3         9        34
    9 #12322     |        0         0         1        14 |        8         2         8        24
   10 #12322     |        1         0         0        12 |        9         3         8        24
   11 #12322     |        0         0         0        15 |        6         2         7        29
   12 #12321     |        1         0         0        13 |        5         3         8        23
   13 #12319     |        0         0         1        14 |        9         3         9        26
   14 #12321     |        1         0         0        13 |        6         2         8        24
   15 #12321     |        1         0         1        15 |       12         3        11        27
   16 #12318     |        0         0         1        13 |        7         3        10        24
   17 #12319     |        0         0         1        13 |       11         3         9        25
   18 #12318     |        0         0         0        12 |        8         2         8        20
   19 #12319     |        0         0         1        18 |       10         2         9        28
   20 #12317     |        0         0         0        20 |        9         3         8        34
   21 #12318     |        0         0         0        13 |        8         3         8        28
   22 #12319     |        0         0         1        11 |        8         3        10        22
   23 #12320     |       28         0         1        28 |       41         3        11        41
  rtla timerlat hit stop tracing
  ## CPU 23 hit stop tracing, analyzing it ##
  IRQ handler delay:                                        27.49 us (65.52 %)
  IRQ latency:                                              28.13 us
  Timerlat IRQ duration:                                     9.59 us (22.85 %)
  Blocking thread:                                           3.79 us (9.03 %)
                         objtool:49256                       3.79 us
    Blocking thread stacktrace
                -> timerlat_irq
                -> __hrtimer_run_queues
                -> hrtimer_interrupt
                -> __sysvec_apic_timer_interrupt
                -> sysvec_apic_timer_interrupt
                -> asm_sysvec_apic_timer_interrupt
                -> _raw_spin_unlock_irqrestore
                -> cgroup_rstat_flush_locked
                -> cgroup_rstat_flush_irqsafe
                -> mem_cgroup_flush_stats
                -> mem_cgroup_wb_stats
                -> balance_dirty_pages
                -> balance_dirty_pages_ratelimited_flags
                -> btrfs_buffered_write
                -> btrfs_do_write_iter
                -> vfs_write
                -> __x64_sys_pwrite64
                -> do_syscall_64
                -> entry_SYSCALL_64_after_hwframe
  ------------------------------------------------------------------------
    Thread latency:                                          41.96 us (100%)

  The system has exit from idle latency!
    Max timerlat IRQ latency from idle: 17.48 us in cpu 4
  Saving trace to timerlat_trace.txt

In this case, the major factor was the delay suffered by the *IRQ handler*
that handles **timerlat** wakeup: *65.52%*. This can be caused by the
current thread masking interrupts, which can be seen in the blocking
thread stacktrace: the current thread (*objtool:49256*) disabled interrupts
via *raw spin lock* operations inside mem cgroup, while doing write
syscall in a btrfs file system.

The raw trace is saved in the **timerlat_trace.txt** file for further analysis.
=======
EXAMPLE
=======

In the example below, the *timerlat* tracer is set to capture the stack trace at
the IRQ handler, printing it to the buffer if the *Thread* timer latency is
higher than *30 us*. It is also set to stop the session if a *Thread* timer
latency higher than *30 us* is hit. Finally, it is set to save the trace
buffer if the stop condition is hit::

  [root@alien ~]# rtla timerlat top -s 30 -t 30 -T
                   Timer Latency
    0 00:00:59   |          IRQ Timer Latency (us)        |         Thread Timer Latency (us)
  CPU COUNT      |      cur       min       avg       max |      cur       min       avg       max
    0 #58634     |        1         0         1        10 |       11         2        10        23
    1 #58634     |        1         0         1         9 |       12         2         9        23
    2 #58634     |        0         0         1        11 |       10         2         9        23
    3 #58634     |        1         0         1        11 |       11         2         9        24
    4 #58634     |        1         0         1        10 |       11         2         9        26
    5 #58634     |        1         0         1         8 |       10         2         9        25
    6 #58634     |       12         0         1        12 |       30         2        10        30 <--- CPU with spike
    7 #58634     |        1         0         1         9 |       11         2         9        23
    8 #58633     |        1         0         1         9 |       11         2         9        26
    9 #58633     |        1         0         1         9 |       10         2         9        26
   10 #58633     |        1         0         1        13 |       11         2         9        28
   11 #58633     |        1         0         1        13 |       12         2         9        24
   12 #58633     |        1         0         1         8 |       10         2         9        23
   13 #58633     |        1         0         1        10 |       10         2         9        22
   14 #58633     |        1         0         1        18 |       12         2         9        27
   15 #58633     |        1         0         1        10 |       11         2         9        28
   16 #58633     |        0         0         1        11 |        7         2         9        26
   17 #58633     |        1         0         1        13 |       10         2         9        24
   18 #58633     |        1         0         1         9 |       13         2         9        22
   19 #58633     |        1         0         1        10 |       11         2         9        23
   20 #58633     |        1         0         1        12 |       11         2         9        28
   21 #58633     |        1         0         1        14 |       11         2         9        24
   22 #58633     |        1         0         1         8 |       11         2         9        22
   23 #58633     |        1         0         1        10 |       11         2         9        27
  timerlat hit stop tracing
  saving trace to timerlat_trace.txt
  [root@alien bristot]# tail -60 timerlat_trace.txt
  [...]
      timerlat/5-79755   [005] .......   426.271226: #58634 context thread timer_latency     10823 ns
              sh-109404  [006] dnLh213   426.271247: #58634 context    irq timer_latency     12505 ns
              sh-109404  [006] dNLh313   426.271258: irq_noise: local_timer:236 start 426.271245463 duration 12553 ns
              sh-109404  [006] d...313   426.271263: thread_noise:       sh:109404 start 426.271245853 duration 4769 ns
      timerlat/6-79756   [006] .......   426.271264: #58634 context thread timer_latency     30328 ns
      timerlat/6-79756   [006] ....1..   426.271265: <stack trace>
  => timerlat_irq
  => __hrtimer_run_queues
  => hrtimer_interrupt
  => __sysvec_apic_timer_interrupt
  => sysvec_apic_timer_interrupt
  => asm_sysvec_apic_timer_interrupt
  => _raw_spin_unlock_irqrestore			<---- spinlock that disabled interrupt.
  => try_to_wake_up
  => autoremove_wake_function
  => __wake_up_common
  => __wake_up_common_lock
  => ep_poll_callback
  => __wake_up_common
  => __wake_up_common_lock
  => fsnotify_add_event
  => inotify_handle_inode_event
  => fsnotify
  => __fsnotify_parent
  => __fput
  => task_work_run
  => exit_to_user_mode_prepare
  => syscall_exit_to_user_mode
  => do_syscall_64
  => entry_SYSCALL_64_after_hwframe
  => 0x7265000001378c
  => 0x10000cea7
  => 0x25a00000204a
  => 0x12e302d00000000
  => 0x19b51010901b6
  => 0x283ce00726500
  => 0x61ea308872
  => 0x00000fe3
            bash-109109  [007] d..h...   426.271265: #58634 context    irq timer_latency      1211 ns
      timerlat/6-79756   [006] .......   426.271267: timerlat_main: stop tracing hit on cpu 6

In the trace, it is possible the notice that the *IRQ* timer latency was
already high, accounting *12505 ns*. The IRQ delay was caused by the
*bash-109109* process that disabled IRQs in the wake-up path
(*_try_to_wake_up()* function). The duration of the IRQ handler that woke
up the timerlat thread, informed with the **osnoise:irq_noise** event, was
also high and added more *12553 ns* to the Thread latency. Finally, the
**osnoise:thread_noise** added by the currently running thread (including
the scheduling overhead) added more *4769 ns*. Summing up these values,
the *Thread* timer latency accounted for *30328 ns*.

The primary reason for this high value is the wake-up path that was hit
twice during this case: when the *bash-109109* was waking up a thread
and then when the *timerlat* thread was awakened. This information can
then be used as the starting point of a more fine-grained analysis.
>>>>>>> b7ba80a49124 (Commit)

Note that **rtla timerlat** was dispatched without changing *timerlat* tracer
threads' priority. That is generally not needed because these threads hava
priority *FIFO:95* by default, which is a common priority used by real-time
kernel developers to analyze scheduling delays.

SEE ALSO
--------
**rtla-timerlat**\(1), **rtla-timerlat-hist**\(1)

*timerlat* tracer documentation: <https://www.kernel.org/doc/html/latest/trace/timerlat-tracer.html>

AUTHOR
------
Written by Daniel Bristot de Oliveira <bristot@kernel.org>

.. include:: common_appendix.rst
