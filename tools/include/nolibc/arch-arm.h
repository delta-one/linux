/* SPDX-License-Identifier: LGPL-2.1 OR MIT */
/*
 * ARM specific definitions for NOLIBC
 * Copyright (C) 2017-2022 Willy Tarreau <w@1wt.eu>
 */

#ifndef _NOLIBC_ARCH_ARM_H
#define _NOLIBC_ARCH_ARM_H

<<<<<<< HEAD
=======
/* O_* macros for fcntl/open are architecture-specific */
#define O_RDONLY            0
#define O_WRONLY            1
#define O_RDWR              2
#define O_CREAT          0x40
#define O_EXCL           0x80
#define O_NOCTTY        0x100
#define O_TRUNC         0x200
#define O_APPEND        0x400
#define O_NONBLOCK      0x800
#define O_DIRECTORY    0x4000

>>>>>>> b7ba80a49124 (Commit)
/* The struct returned by the stat() syscall, 32-bit only, the syscall returns
 * exactly 56 bytes (stops before the unused array). In big endian, the format
 * differs as devices are returned as short only.
 */
struct sys_stat_struct {
#if defined(__ARMEB__)
	unsigned short st_dev;
	unsigned short __pad1;
#else
	unsigned long  st_dev;
#endif
	unsigned long  st_ino;
	unsigned short st_mode;
	unsigned short st_nlink;
	unsigned short st_uid;
	unsigned short st_gid;

#if defined(__ARMEB__)
	unsigned short st_rdev;
	unsigned short __pad2;
#else
	unsigned long  st_rdev;
#endif
	unsigned long  st_size;
	unsigned long  st_blksize;
	unsigned long  st_blocks;

	unsigned long  st_atime;
	unsigned long  st_atime_nsec;
	unsigned long  st_mtime;
	unsigned long  st_mtime_nsec;

	unsigned long  st_ctime;
	unsigned long  st_ctime_nsec;
	unsigned long  __unused[2];
};

/* Syscalls for ARM in ARM or Thumb modes :
 *   - registers are 32-bit
 *   - stack is 8-byte aligned
 *     ( http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka4127.html)
 *   - syscall number is passed in r7
 *   - arguments are in r0, r1, r2, r3, r4, r5
 *   - the system call is performed by calling svc #0
 *   - syscall return comes in r0.
 *   - only lr is clobbered.
 *   - the arguments are cast to long and assigned into the target registers
 *     which are then simply passed as registers to the asm code, so that we
 *     don't have to experience issues with register constraints.
 *   - the syscall number is always specified last in order to allow to force
 *     some registers before (gcc refuses a %-register at the last position).
<<<<<<< HEAD
 *   - in thumb mode without -fomit-frame-pointer, r7 is also used to store the
 *     frame pointer, and we cannot directly assign it as a register variable,
 *     nor can we clobber it. Instead we assign the r6 register and swap it
 *     with r7 before calling svc, and r6 is marked as clobbered.
 *     We're just using any regular register which we assign to r7 after saving
 *     it.
=======
>>>>>>> b7ba80a49124 (Commit)
 *
 * Also, ARM supports the old_select syscall if newselect is not available
 */
#define __ARCH_WANT_SYS_OLD_SELECT

<<<<<<< HEAD
#if (defined(__THUMBEB__) || defined(__THUMBEL__)) && \
    !defined(NOLIBC_OMIT_FRAME_POINTER)
/* swap r6,r7 needed in Thumb mode since we can't use nor clobber r7 */
#define _NOLIBC_SYSCALL_REG         "r6"
#define _NOLIBC_THUMB_SET_R7        "eor r7, r6\neor r6, r7\neor r7, r6\n"
#define _NOLIBC_THUMB_RESTORE_R7    "mov r7, r6\n"

#else  /* we're in ARM mode */
/* in Arm mode we can directly use r7 */
#define _NOLIBC_SYSCALL_REG         "r7"
#define _NOLIBC_THUMB_SET_R7        ""
#define _NOLIBC_THUMB_RESTORE_R7    ""

#endif /* end THUMB */

#define my_syscall0(num)                                                      \
({                                                                            \
	register long _num  __asm__(_NOLIBC_SYSCALL_REG) = (num);             \
	register long _arg1 __asm__ ("r0");                                   \
	                                                                      \
	__asm__  volatile (                                                   \
		_NOLIBC_THUMB_SET_R7                                          \
		"svc #0\n"                                                    \
		_NOLIBC_THUMB_RESTORE_R7                                      \
		: "=r"(_arg1), "=r"(_num)                                     \
		: "r"(_arg1),                                                 \
		  "r"(_num)                                                   \
=======
#define my_syscall0(num)                                                      \
({                                                                            \
	register long _num __asm__ ("r7") = (num);                            \
	register long _arg1 __asm__ ("r0");                                   \
	                                                                      \
	__asm__  volatile (                                                   \
		"svc #0\n"                                                    \
		: "=r"(_arg1)                                                 \
		: "r"(_num)                                                   \
>>>>>>> b7ba80a49124 (Commit)
		: "memory", "cc", "lr"                                        \
	);                                                                    \
	_arg1;                                                                \
})

#define my_syscall1(num, arg1)                                                \
({                                                                            \
<<<<<<< HEAD
	register long _num  __asm__(_NOLIBC_SYSCALL_REG) = (num);             \
	register long _arg1 __asm__ ("r0") = (long)(arg1);                    \
	                                                                      \
	__asm__  volatile (                                                   \
		_NOLIBC_THUMB_SET_R7                                          \
		"svc #0\n"                                                    \
		_NOLIBC_THUMB_RESTORE_R7                                      \
		: "=r"(_arg1), "=r" (_num)                                    \
=======
	register long _num __asm__ ("r7") = (num);                            \
	register long _arg1 __asm__ ("r0") = (long)(arg1);                    \
	                                                                      \
	__asm__  volatile (                                                   \
		"svc #0\n"                                                    \
		: "=r"(_arg1)                                                 \
>>>>>>> b7ba80a49124 (Commit)
		: "r"(_arg1),                                                 \
		  "r"(_num)                                                   \
		: "memory", "cc", "lr"                                        \
	);                                                                    \
	_arg1;                                                                \
})

#define my_syscall2(num, arg1, arg2)                                          \
({                                                                            \
<<<<<<< HEAD
	register long _num  __asm__(_NOLIBC_SYSCALL_REG) = (num);             \
=======
	register long _num __asm__ ("r7") = (num);                            \
>>>>>>> b7ba80a49124 (Commit)
	register long _arg1 __asm__ ("r0") = (long)(arg1);                    \
	register long _arg2 __asm__ ("r1") = (long)(arg2);                    \
	                                                                      \
	__asm__  volatile (                                                   \
<<<<<<< HEAD
		_NOLIBC_THUMB_SET_R7                                          \
		"svc #0\n"                                                    \
		_NOLIBC_THUMB_RESTORE_R7                                      \
		: "=r"(_arg1), "=r" (_num)                                    \
=======
		"svc #0\n"                                                    \
		: "=r"(_arg1)                                                 \
>>>>>>> b7ba80a49124 (Commit)
		: "r"(_arg1), "r"(_arg2),                                     \
		  "r"(_num)                                                   \
		: "memory", "cc", "lr"                                        \
	);                                                                    \
	_arg1;                                                                \
})

#define my_syscall3(num, arg1, arg2, arg3)                                    \
({                                                                            \
<<<<<<< HEAD
	register long _num  __asm__(_NOLIBC_SYSCALL_REG) = (num);             \
=======
	register long _num __asm__ ("r7") = (num);                            \
>>>>>>> b7ba80a49124 (Commit)
	register long _arg1 __asm__ ("r0") = (long)(arg1);                    \
	register long _arg2 __asm__ ("r1") = (long)(arg2);                    \
	register long _arg3 __asm__ ("r2") = (long)(arg3);                    \
	                                                                      \
	__asm__  volatile (                                                   \
<<<<<<< HEAD
		_NOLIBC_THUMB_SET_R7                                          \
		"svc #0\n"                                                    \
		_NOLIBC_THUMB_RESTORE_R7                                      \
		: "=r"(_arg1), "=r" (_num)                                    \
=======
		"svc #0\n"                                                    \
		: "=r"(_arg1)                                                 \
>>>>>>> b7ba80a49124 (Commit)
		: "r"(_arg1), "r"(_arg2), "r"(_arg3),                         \
		  "r"(_num)                                                   \
		: "memory", "cc", "lr"                                        \
	);                                                                    \
	_arg1;                                                                \
})

#define my_syscall4(num, arg1, arg2, arg3, arg4)                              \
({                                                                            \
<<<<<<< HEAD
	register long _num  __asm__(_NOLIBC_SYSCALL_REG) = (num);             \
=======
	register long _num __asm__ ("r7") = (num);                            \
>>>>>>> b7ba80a49124 (Commit)
	register long _arg1 __asm__ ("r0") = (long)(arg1);                    \
	register long _arg2 __asm__ ("r1") = (long)(arg2);                    \
	register long _arg3 __asm__ ("r2") = (long)(arg3);                    \
	register long _arg4 __asm__ ("r3") = (long)(arg4);                    \
	                                                                      \
	__asm__  volatile (                                                   \
<<<<<<< HEAD
		_NOLIBC_THUMB_SET_R7                                          \
		"svc #0\n"                                                    \
		_NOLIBC_THUMB_RESTORE_R7                                      \
		: "=r"(_arg1), "=r" (_num)                                    \
=======
		"svc #0\n"                                                    \
		: "=r"(_arg1)                                                 \
>>>>>>> b7ba80a49124 (Commit)
		: "r"(_arg1), "r"(_arg2), "r"(_arg3), "r"(_arg4),             \
		  "r"(_num)                                                   \
		: "memory", "cc", "lr"                                        \
	);                                                                    \
	_arg1;                                                                \
})

#define my_syscall5(num, arg1, arg2, arg3, arg4, arg5)                        \
({                                                                            \
<<<<<<< HEAD
	register long _num  __asm__(_NOLIBC_SYSCALL_REG) = (num);             \
=======
	register long _num __asm__ ("r7") = (num);                            \
>>>>>>> b7ba80a49124 (Commit)
	register long _arg1 __asm__ ("r0") = (long)(arg1);                    \
	register long _arg2 __asm__ ("r1") = (long)(arg2);                    \
	register long _arg3 __asm__ ("r2") = (long)(arg3);                    \
	register long _arg4 __asm__ ("r3") = (long)(arg4);                    \
	register long _arg5 __asm__ ("r4") = (long)(arg5);                    \
	                                                                      \
	__asm__  volatile (                                                   \
<<<<<<< HEAD
		_NOLIBC_THUMB_SET_R7                                          \
		"svc #0\n"                                                    \
		_NOLIBC_THUMB_RESTORE_R7                                      \
		: "=r"(_arg1), "=r" (_num)                                    \
=======
		"svc #0\n"                                                    \
		: "=r" (_arg1)                                                \
>>>>>>> b7ba80a49124 (Commit)
		: "r"(_arg1), "r"(_arg2), "r"(_arg3), "r"(_arg4), "r"(_arg5), \
		  "r"(_num)                                                   \
		: "memory", "cc", "lr"                                        \
	);                                                                    \
	_arg1;                                                                \
})

<<<<<<< HEAD
char **environ __attribute__((weak));
const unsigned long *_auxv __attribute__((weak));

/* startup code */
void __attribute__((weak,noreturn,optimize("omit-frame-pointer"))) _start(void)
{
	__asm__ volatile (
		"pop {%r0}\n"                 // argc was in the stack
		"mov %r1, %sp\n"              // argv = sp

		"add %r2, %r0, $1\n"          // envp = (argc + 1) ...
		"lsl %r2, %r2, $2\n"          //        * 4        ...
		"add %r2, %r2, %r1\n"         //        + argv
		"ldr %r3, 1f\n"               // r3 = &environ (see below)
		"str %r2, [r3]\n"             // store envp into environ

		"mov r4, r2\n"                // search for auxv (follows NULL after last env)
		"0:\n"
		"mov r5, r4\n"                // r5 = r4
		"add r4, r4, #4\n"            // r4 += 4
		"ldr r5,[r5]\n"               // r5 = *r5 = *(r4-4)
		"cmp r5, #0\n"                // and stop at NULL after last env
		"bne 0b\n"
		"ldr %r3, 2f\n"               // r3 = &_auxv (low bits)
		"str r4, [r3]\n"              // store r4 into _auxv

		"mov %r3, $8\n"               // AAPCS : sp must be 8-byte aligned in the
		"neg %r3, %r3\n"              //         callee, and bl doesn't push (lr=pc)
		"and %r3, %r3, %r1\n"         // so we do sp = r1(=sp) & r3(=-8);
		"mov %sp, %r3\n"              //

		"bl main\n"                   // main() returns the status code, we'll exit with it.
		"movs r7, $1\n"               // NR_exit == 1
		"svc $0x00\n"
		".align 2\n"                  // below are the pointers to a few variables
		"1:\n"
		".word environ\n"
		"2:\n"
		".word _auxv\n"
	);
	__builtin_unreachable();
}
=======
/* startup code */
__asm__ (".section .text\n"
    ".weak _start\n"
    "_start:\n"
#if defined(__THUMBEB__) || defined(__THUMBEL__)
    /* We enter here in 32-bit mode but if some previous functions were in
     * 16-bit mode, the assembler cannot know, so we need to tell it we're in
     * 32-bit now, then switch to 16-bit (is there a better way to do it than
     * adding 1 by hand ?) and tell the asm we're now in 16-bit mode so that
     * it generates correct instructions. Note that we do not support thumb1.
     */
    ".code 32\n"
    "add     r0, pc, #1\n"
    "bx      r0\n"
    ".code 16\n"
#endif
    "pop {%r0}\n"                 // argc was in the stack
    "mov %r1, %sp\n"              // argv = sp
    "add %r2, %r1, %r0, lsl #2\n" // envp = argv + 4*argc ...
    "add %r2, %r2, $4\n"          //        ... + 4
    "and %r3, %r1, $-8\n"         // AAPCS : sp must be 8-byte aligned in the
    "mov %sp, %r3\n"              //         callee, an bl doesn't push (lr=pc)
    "bl main\n"                   // main() returns the status code, we'll exit with it.
    "movs r7, $1\n"               // NR_exit == 1
    "svc $0x00\n"
    "");
>>>>>>> b7ba80a49124 (Commit)

#endif // _NOLIBC_ARCH_ARM_H
