/*
 *
 * syscall.c
 *
 * syscall: Benchmark for system call performance
 */
#include "../perf.h"
#include "../util/util.h"
#include <subcmd/parse-options.h>
#include "../builtin.h"
#include "bench.h"

#include <stdio.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/types.h>
<<<<<<< HEAD
#include <sys/wait.h>
=======
>>>>>>> b7ba80a49124 (Commit)
#include <unistd.h>
#include <stdlib.h>

#define LOOPS_DEFAULT 10000000
static	int loops = LOOPS_DEFAULT;

static const struct option options[] = {
	OPT_INTEGER('l', "loop",	&loops,		"Specify number of loops"),
	OPT_END()
};

static const char * const bench_syscall_usage[] = {
	"perf bench syscall <options>",
	NULL
};

<<<<<<< HEAD
static void test_execve(void)
{
	const char *pathname = "/bin/true";
	char *const argv[] = { (char *)pathname, NULL };
	pid_t pid = fork();

	if (pid < 0) {
		fprintf(stderr, "fork failed\n");
		exit(1);
	} else if (pid == 0) {
		execve(pathname, argv, NULL);
		fprintf(stderr, "execve /bin/true failed\n");
		exit(1);
	} else {
		if (waitpid(pid, NULL, 0) < 0) {
			fprintf(stderr, "waitpid failed\n");
			exit(1);
		}
	}
}

static int bench_syscall_common(int argc, const char **argv, int syscall)
{
	struct timeval start, stop, diff;
	unsigned long long result_usec = 0;
	const char *name = NULL;
=======
int bench_syscall_basic(int argc, const char **argv)
{
	struct timeval start, stop, diff;
	unsigned long long result_usec = 0;
>>>>>>> b7ba80a49124 (Commit)
	int i;

	argc = parse_options(argc, argv, options, bench_syscall_usage, 0);

	gettimeofday(&start, NULL);

<<<<<<< HEAD
	for (i = 0; i < loops; i++) {
		switch (syscall) {
		case __NR_getppid:
			getppid();
			break;
		case __NR_getpgid:
			getpgid(0);
			break;
		case __NR_execve:
			test_execve();
			/* Only loop 10000 times to save time */
			if (i == 10000)
				loops = 10000;
			break;
		default:
			break;
		}
	}
=======
	for (i = 0; i < loops; i++)
		getppid();
>>>>>>> b7ba80a49124 (Commit)

	gettimeofday(&stop, NULL);
	timersub(&stop, &start, &diff);

<<<<<<< HEAD
	switch (syscall) {
	case __NR_getppid:
		name = "getppid()";
		break;
	case __NR_getpgid:
		name = "getpgid()";
		break;
	case __NR_execve:
		name = "execve()";
		break;
	default:
		break;
	}

	switch (bench_format) {
	case BENCH_FORMAT_DEFAULT:
		printf("# Executed %'d %s calls\n", loops, name);
=======
	switch (bench_format) {
	case BENCH_FORMAT_DEFAULT:
		printf("# Executed %'d getppid() calls\n", loops);
>>>>>>> b7ba80a49124 (Commit)

		result_usec = diff.tv_sec * 1000000;
		result_usec += diff.tv_usec;

		printf(" %14s: %lu.%03lu [sec]\n\n", "Total time",
		       (unsigned long) diff.tv_sec,
		       (unsigned long) (diff.tv_usec/1000));

		printf(" %14lf usecs/op\n",
		       (double)result_usec / (double)loops);
		printf(" %'14d ops/sec\n",
		       (int)((double)loops /
			     ((double)result_usec / (double)1000000)));
		break;

	case BENCH_FORMAT_SIMPLE:
		printf("%lu.%03lu\n",
		       (unsigned long) diff.tv_sec,
		       (unsigned long) (diff.tv_usec / 1000));
		break;

	default:
		/* reaching here is something disaster */
		fprintf(stderr, "Unknown format:%d\n", bench_format);
		exit(1);
		break;
	}

	return 0;
}
<<<<<<< HEAD

int bench_syscall_basic(int argc, const char **argv)
{
	return bench_syscall_common(argc, argv, __NR_getppid);
}

int bench_syscall_getpgid(int argc, const char **argv)
{
	return bench_syscall_common(argc, argv, __NR_getpgid);
}

int bench_syscall_execve(int argc, const char **argv)
{
	return bench_syscall_common(argc, argv, __NR_execve);
}
=======
>>>>>>> b7ba80a49124 (Commit)
