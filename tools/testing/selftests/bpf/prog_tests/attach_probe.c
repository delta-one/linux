// SPDX-License-Identifier: GPL-2.0
#include <test_progs.h>
<<<<<<< HEAD
#include "test_attach_kprobe_sleepable.skel.h"
#include "test_attach_probe_manual.skel.h"
=======
>>>>>>> b7ba80a49124 (Commit)
#include "test_attach_probe.skel.h"

/* this is how USDT semaphore is actually defined, except volatile modifier */
volatile unsigned short uprobe_ref_ctr __attribute__((unused)) __attribute((section(".probes")));

/* uprobe attach point */
static noinline void trigger_func(void)
{
	asm volatile ("");
}

/* attach point for byname uprobe */
static noinline void trigger_func2(void)
{
	asm volatile ("");
}

/* attach point for byname sleepable uprobe */
static noinline void trigger_func3(void)
{
	asm volatile ("");
}

<<<<<<< HEAD
/* attach point for ref_ctr */
static noinline void trigger_func4(void)
{
	asm volatile ("");
}

static char test_data[] = "test_data";

/* manual attach kprobe/kretprobe/uprobe/uretprobe testings */
static void test_attach_probe_manual(enum probe_attach_mode attach_mode)
{
	DECLARE_LIBBPF_OPTS(bpf_uprobe_opts, uprobe_opts);
	DECLARE_LIBBPF_OPTS(bpf_kprobe_opts, kprobe_opts);
	struct bpf_link *kprobe_link, *kretprobe_link;
	struct bpf_link *uprobe_link, *uretprobe_link;
	struct test_attach_probe_manual *skel;
	ssize_t uprobe_offset;

	skel = test_attach_probe_manual__open_and_load();
	if (!ASSERT_OK_PTR(skel, "skel_kprobe_manual_open_and_load"))
		return;

	uprobe_offset = get_uprobe_offset(&trigger_func);
	if (!ASSERT_GE(uprobe_offset, 0, "uprobe_offset"))
		goto cleanup;

	/* manual-attach kprobe/kretprobe */
	kprobe_opts.attach_mode = attach_mode;
	kprobe_opts.retprobe = false;
	kprobe_link = bpf_program__attach_kprobe_opts(skel->progs.handle_kprobe,
						      SYS_NANOSLEEP_KPROBE_NAME,
						      &kprobe_opts);
=======
static char test_data[] = "test_data";

void test_attach_probe(void)
{
	DECLARE_LIBBPF_OPTS(bpf_uprobe_opts, uprobe_opts);
	struct bpf_link *kprobe_link, *kretprobe_link;
	struct bpf_link *uprobe_link, *uretprobe_link;
	struct test_attach_probe* skel;
	ssize_t uprobe_offset, ref_ctr_offset;
	struct bpf_link *uprobe_err_link;
	bool legacy;
	char *mem;

	/* Check if new-style kprobe/uprobe API is supported.
	 * Kernels that support new FD-based kprobe and uprobe BPF attachment
	 * through perf_event_open() syscall expose
	 * /sys/bus/event_source/devices/kprobe/type and
	 * /sys/bus/event_source/devices/uprobe/type files, respectively. They
	 * contain magic numbers that are passed as "type" field of
	 * perf_event_attr. Lack of such file in the system indicates legacy
	 * kernel with old-style kprobe/uprobe attach interface through
	 * creating per-probe event through tracefs. For such cases
	 * ref_ctr_offset feature is not supported, so we don't test it.
	 */
	legacy = access("/sys/bus/event_source/devices/kprobe/type", F_OK) != 0;

	uprobe_offset = get_uprobe_offset(&trigger_func);
	if (!ASSERT_GE(uprobe_offset, 0, "uprobe_offset"))
		return;

	ref_ctr_offset = get_rel_offset((uintptr_t)&uprobe_ref_ctr);
	if (!ASSERT_GE(ref_ctr_offset, 0, "ref_ctr_offset"))
		return;

	skel = test_attach_probe__open();
	if (!ASSERT_OK_PTR(skel, "skel_open"))
		return;

	/* sleepable kprobe test case needs flags set before loading */
	if (!ASSERT_OK(bpf_program__set_flags(skel->progs.handle_kprobe_sleepable,
		BPF_F_SLEEPABLE), "kprobe_sleepable_flags"))
		goto cleanup;

	if (!ASSERT_OK(test_attach_probe__load(skel), "skel_load"))
		goto cleanup;
	if (!ASSERT_OK_PTR(skel->bss, "check_bss"))
		goto cleanup;

	/* manual-attach kprobe/kretprobe */
	kprobe_link = bpf_program__attach_kprobe(skel->progs.handle_kprobe,
						 false /* retprobe */,
						 SYS_NANOSLEEP_KPROBE_NAME);
>>>>>>> b7ba80a49124 (Commit)
	if (!ASSERT_OK_PTR(kprobe_link, "attach_kprobe"))
		goto cleanup;
	skel->links.handle_kprobe = kprobe_link;

<<<<<<< HEAD
	kprobe_opts.retprobe = true;
	kretprobe_link = bpf_program__attach_kprobe_opts(skel->progs.handle_kretprobe,
							 SYS_NANOSLEEP_KPROBE_NAME,
							 &kprobe_opts);
=======
	kretprobe_link = bpf_program__attach_kprobe(skel->progs.handle_kretprobe,
						    true /* retprobe */,
						    SYS_NANOSLEEP_KPROBE_NAME);
>>>>>>> b7ba80a49124 (Commit)
	if (!ASSERT_OK_PTR(kretprobe_link, "attach_kretprobe"))
		goto cleanup;
	skel->links.handle_kretprobe = kretprobe_link;

<<<<<<< HEAD
	/* manual-attach uprobe/uretprobe */
	uprobe_opts.attach_mode = attach_mode;
	uprobe_opts.ref_ctr_offset = 0;
	uprobe_opts.retprobe = false;
=======
	/* auto-attachable kprobe and kretprobe */
	skel->links.handle_kprobe_auto = bpf_program__attach(skel->progs.handle_kprobe_auto);
	ASSERT_OK_PTR(skel->links.handle_kprobe_auto, "attach_kprobe_auto");

	skel->links.handle_kretprobe_auto = bpf_program__attach(skel->progs.handle_kretprobe_auto);
	ASSERT_OK_PTR(skel->links.handle_kretprobe_auto, "attach_kretprobe_auto");

	if (!legacy)
		ASSERT_EQ(uprobe_ref_ctr, 0, "uprobe_ref_ctr_before");

	uprobe_opts.retprobe = false;
	uprobe_opts.ref_ctr_offset = legacy ? 0 : ref_ctr_offset;
>>>>>>> b7ba80a49124 (Commit)
	uprobe_link = bpf_program__attach_uprobe_opts(skel->progs.handle_uprobe,
						      0 /* self pid */,
						      "/proc/self/exe",
						      uprobe_offset,
						      &uprobe_opts);
	if (!ASSERT_OK_PTR(uprobe_link, "attach_uprobe"))
		goto cleanup;
	skel->links.handle_uprobe = uprobe_link;

<<<<<<< HEAD
	uprobe_opts.retprobe = true;
=======
	if (!legacy)
		ASSERT_GT(uprobe_ref_ctr, 0, "uprobe_ref_ctr_after");

	/* if uprobe uses ref_ctr, uretprobe has to use ref_ctr as well */
	uprobe_opts.retprobe = true;
	uprobe_opts.ref_ctr_offset = legacy ? 0 : ref_ctr_offset;
>>>>>>> b7ba80a49124 (Commit)
	uretprobe_link = bpf_program__attach_uprobe_opts(skel->progs.handle_uretprobe,
							 -1 /* any pid */,
							 "/proc/self/exe",
							 uprobe_offset, &uprobe_opts);
	if (!ASSERT_OK_PTR(uretprobe_link, "attach_uretprobe"))
		goto cleanup;
	skel->links.handle_uretprobe = uretprobe_link;

<<<<<<< HEAD
	/* attach uprobe by function name manually */
=======
	/* verify auto-attach fails for old-style uprobe definition */
	uprobe_err_link = bpf_program__attach(skel->progs.handle_uprobe_byname);
	if (!ASSERT_EQ(libbpf_get_error(uprobe_err_link), -EOPNOTSUPP,
		       "auto-attach should fail for old-style name"))
		goto cleanup;

>>>>>>> b7ba80a49124 (Commit)
	uprobe_opts.func_name = "trigger_func2";
	uprobe_opts.retprobe = false;
	uprobe_opts.ref_ctr_offset = 0;
	skel->links.handle_uprobe_byname =
			bpf_program__attach_uprobe_opts(skel->progs.handle_uprobe_byname,
							0 /* this pid */,
							"/proc/self/exe",
							0, &uprobe_opts);
	if (!ASSERT_OK_PTR(skel->links.handle_uprobe_byname, "attach_uprobe_byname"))
		goto cleanup;

<<<<<<< HEAD
	/* trigger & validate kprobe && kretprobe */
	usleep(1);

	/* trigger & validate uprobe & uretprobe */
	trigger_func();

	/* trigger & validate uprobe attached by name */
	trigger_func2();

	ASSERT_EQ(skel->bss->kprobe_res, 1, "check_kprobe_res");
	ASSERT_EQ(skel->bss->kretprobe_res, 2, "check_kretprobe_res");
	ASSERT_EQ(skel->bss->uprobe_res, 3, "check_uprobe_res");
	ASSERT_EQ(skel->bss->uretprobe_res, 4, "check_uretprobe_res");
	ASSERT_EQ(skel->bss->uprobe_byname_res, 5, "check_uprobe_byname_res");

cleanup:
	test_attach_probe_manual__destroy(skel);
}

static void test_attach_probe_auto(struct test_attach_probe *skel)
{
	struct bpf_link *uprobe_err_link;

	/* auto-attachable kprobe and kretprobe */
	skel->links.handle_kprobe_auto = bpf_program__attach(skel->progs.handle_kprobe_auto);
	ASSERT_OK_PTR(skel->links.handle_kprobe_auto, "attach_kprobe_auto");

	skel->links.handle_kretprobe_auto = bpf_program__attach(skel->progs.handle_kretprobe_auto);
	ASSERT_OK_PTR(skel->links.handle_kretprobe_auto, "attach_kretprobe_auto");

	/* verify auto-attach fails for old-style uprobe definition */
	uprobe_err_link = bpf_program__attach(skel->progs.handle_uprobe_byname);
	if (!ASSERT_EQ(libbpf_get_error(uprobe_err_link), -EOPNOTSUPP,
		       "auto-attach should fail for old-style name"))
		return;

=======
>>>>>>> b7ba80a49124 (Commit)
	/* verify auto-attach works */
	skel->links.handle_uretprobe_byname =
			bpf_program__attach(skel->progs.handle_uretprobe_byname);
	if (!ASSERT_OK_PTR(skel->links.handle_uretprobe_byname, "attach_uretprobe_byname"))
<<<<<<< HEAD
		return;

	/* trigger & validate kprobe && kretprobe */
	usleep(1);

	/* trigger & validate uprobe attached by name */
	trigger_func2();

	ASSERT_EQ(skel->bss->kprobe2_res, 11, "check_kprobe_auto_res");
	ASSERT_EQ(skel->bss->kretprobe2_res, 22, "check_kretprobe_auto_res");
	ASSERT_EQ(skel->bss->uretprobe_byname_res, 6, "check_uretprobe_byname_res");
}

static void test_uprobe_lib(struct test_attach_probe *skel)
{
	DECLARE_LIBBPF_OPTS(bpf_uprobe_opts, uprobe_opts);
	FILE *devnull;
=======
		goto cleanup;
>>>>>>> b7ba80a49124 (Commit)

	/* test attach by name for a library function, using the library
	 * as the binary argument. libc.so.6 will be resolved via dlopen()/dlinfo().
	 */
<<<<<<< HEAD
	uprobe_opts.func_name = "fopen";
=======
	uprobe_opts.func_name = "malloc";
>>>>>>> b7ba80a49124 (Commit)
	uprobe_opts.retprobe = false;
	skel->links.handle_uprobe_byname2 =
			bpf_program__attach_uprobe_opts(skel->progs.handle_uprobe_byname2,
							0 /* this pid */,
							"libc.so.6",
							0, &uprobe_opts);
	if (!ASSERT_OK_PTR(skel->links.handle_uprobe_byname2, "attach_uprobe_byname2"))
<<<<<<< HEAD
		return;

	uprobe_opts.func_name = "fclose";
=======
		goto cleanup;

	uprobe_opts.func_name = "free";
>>>>>>> b7ba80a49124 (Commit)
	uprobe_opts.retprobe = true;
	skel->links.handle_uretprobe_byname2 =
			bpf_program__attach_uprobe_opts(skel->progs.handle_uretprobe_byname2,
							-1 /* any pid */,
							"libc.so.6",
							0, &uprobe_opts);
	if (!ASSERT_OK_PTR(skel->links.handle_uretprobe_byname2, "attach_uretprobe_byname2"))
<<<<<<< HEAD
		return;

	/* trigger & validate shared library u[ret]probes attached by name */
	devnull = fopen("/dev/null", "r");
	fclose(devnull);

	ASSERT_EQ(skel->bss->uprobe_byname2_res, 7, "check_uprobe_byname2_res");
	ASSERT_EQ(skel->bss->uretprobe_byname2_res, 8, "check_uretprobe_byname2_res");
}

static void test_uprobe_ref_ctr(struct test_attach_probe *skel)
{
	DECLARE_LIBBPF_OPTS(bpf_uprobe_opts, uprobe_opts);
	struct bpf_link *uprobe_link, *uretprobe_link;
	ssize_t uprobe_offset, ref_ctr_offset;

	uprobe_offset = get_uprobe_offset(&trigger_func4);
	if (!ASSERT_GE(uprobe_offset, 0, "uprobe_offset_ref_ctr"))
		return;

	ref_ctr_offset = get_rel_offset((uintptr_t)&uprobe_ref_ctr);
	if (!ASSERT_GE(ref_ctr_offset, 0, "ref_ctr_offset"))
		return;

	ASSERT_EQ(uprobe_ref_ctr, 0, "uprobe_ref_ctr_before");

	uprobe_opts.retprobe = false;
	uprobe_opts.ref_ctr_offset = ref_ctr_offset;
	uprobe_link = bpf_program__attach_uprobe_opts(skel->progs.handle_uprobe_ref_ctr,
						      0 /* self pid */,
						      "/proc/self/exe",
						      uprobe_offset,
						      &uprobe_opts);
	if (!ASSERT_OK_PTR(uprobe_link, "attach_uprobe_ref_ctr"))
		return;
	skel->links.handle_uprobe_ref_ctr = uprobe_link;

	ASSERT_GT(uprobe_ref_ctr, 0, "uprobe_ref_ctr_after");

	/* if uprobe uses ref_ctr, uretprobe has to use ref_ctr as well */
	uprobe_opts.retprobe = true;
	uprobe_opts.ref_ctr_offset = ref_ctr_offset;
	uretprobe_link = bpf_program__attach_uprobe_opts(skel->progs.handle_uretprobe_ref_ctr,
							 -1 /* any pid */,
							 "/proc/self/exe",
							 uprobe_offset, &uprobe_opts);
	if (!ASSERT_OK_PTR(uretprobe_link, "attach_uretprobe_ref_ctr"))
		return;
	skel->links.handle_uretprobe_ref_ctr = uretprobe_link;
}

static void test_kprobe_sleepable(void)
{
	struct test_attach_kprobe_sleepable *skel;

	skel = test_attach_kprobe_sleepable__open();
	if (!ASSERT_OK_PTR(skel, "skel_kprobe_sleepable_open"))
		return;

	/* sleepable kprobe test case needs flags set before loading */
	if (!ASSERT_OK(bpf_program__set_flags(skel->progs.handle_kprobe_sleepable,
		BPF_F_SLEEPABLE), "kprobe_sleepable_flags"))
		goto cleanup;

	if (!ASSERT_OK(test_attach_kprobe_sleepable__load(skel),
		       "skel_kprobe_sleepable_load"))
=======
>>>>>>> b7ba80a49124 (Commit)
		goto cleanup;

	/* sleepable kprobes should not attach successfully */
	skel->links.handle_kprobe_sleepable = bpf_program__attach(skel->progs.handle_kprobe_sleepable);
<<<<<<< HEAD
	ASSERT_ERR_PTR(skel->links.handle_kprobe_sleepable, "attach_kprobe_sleepable");

cleanup:
	test_attach_kprobe_sleepable__destroy(skel);
}

static void test_uprobe_sleepable(struct test_attach_probe *skel)
{
	/* test sleepable uprobe and uretprobe variants */
	skel->links.handle_uprobe_byname3_sleepable = bpf_program__attach(skel->progs.handle_uprobe_byname3_sleepable);
	if (!ASSERT_OK_PTR(skel->links.handle_uprobe_byname3_sleepable, "attach_uprobe_byname3_sleepable"))
		return;

	skel->links.handle_uprobe_byname3 = bpf_program__attach(skel->progs.handle_uprobe_byname3);
	if (!ASSERT_OK_PTR(skel->links.handle_uprobe_byname3, "attach_uprobe_byname3"))
		return;

	skel->links.handle_uretprobe_byname3_sleepable = bpf_program__attach(skel->progs.handle_uretprobe_byname3_sleepable);
	if (!ASSERT_OK_PTR(skel->links.handle_uretprobe_byname3_sleepable, "attach_uretprobe_byname3_sleepable"))
		return;

	skel->links.handle_uretprobe_byname3 = bpf_program__attach(skel->progs.handle_uretprobe_byname3);
	if (!ASSERT_OK_PTR(skel->links.handle_uretprobe_byname3, "attach_uretprobe_byname3"))
		return;

	skel->bss->user_ptr = test_data;

	/* trigger & validate sleepable uprobe attached by name */
	trigger_func3();

=======
	if (!ASSERT_ERR_PTR(skel->links.handle_kprobe_sleepable, "attach_kprobe_sleepable"))
		goto cleanup;

	/* test sleepable uprobe and uretprobe variants */
	skel->links.handle_uprobe_byname3_sleepable = bpf_program__attach(skel->progs.handle_uprobe_byname3_sleepable);
	if (!ASSERT_OK_PTR(skel->links.handle_uprobe_byname3_sleepable, "attach_uprobe_byname3_sleepable"))
		goto cleanup;

	skel->links.handle_uprobe_byname3 = bpf_program__attach(skel->progs.handle_uprobe_byname3);
	if (!ASSERT_OK_PTR(skel->links.handle_uprobe_byname3, "attach_uprobe_byname3"))
		goto cleanup;

	skel->links.handle_uretprobe_byname3_sleepable = bpf_program__attach(skel->progs.handle_uretprobe_byname3_sleepable);
	if (!ASSERT_OK_PTR(skel->links.handle_uretprobe_byname3_sleepable, "attach_uretprobe_byname3_sleepable"))
		goto cleanup;

	skel->links.handle_uretprobe_byname3 = bpf_program__attach(skel->progs.handle_uretprobe_byname3);
	if (!ASSERT_OK_PTR(skel->links.handle_uretprobe_byname3, "attach_uretprobe_byname3"))
		goto cleanup;

	skel->bss->user_ptr = test_data;

	/* trigger & validate kprobe && kretprobe */
	usleep(1);

	/* trigger & validate shared library u[ret]probes attached by name */
	mem = malloc(1);
	free(mem);

	/* trigger & validate uprobe & uretprobe */
	trigger_func();

	/* trigger & validate uprobe attached by name */
	trigger_func2();

	/* trigger & validate sleepable uprobe attached by name */
	trigger_func3();

	ASSERT_EQ(skel->bss->kprobe_res, 1, "check_kprobe_res");
	ASSERT_EQ(skel->bss->kprobe2_res, 11, "check_kprobe_auto_res");
	ASSERT_EQ(skel->bss->kretprobe_res, 2, "check_kretprobe_res");
	ASSERT_EQ(skel->bss->kretprobe2_res, 22, "check_kretprobe_auto_res");
	ASSERT_EQ(skel->bss->uprobe_res, 3, "check_uprobe_res");
	ASSERT_EQ(skel->bss->uretprobe_res, 4, "check_uretprobe_res");
	ASSERT_EQ(skel->bss->uprobe_byname_res, 5, "check_uprobe_byname_res");
	ASSERT_EQ(skel->bss->uretprobe_byname_res, 6, "check_uretprobe_byname_res");
	ASSERT_EQ(skel->bss->uprobe_byname2_res, 7, "check_uprobe_byname2_res");
	ASSERT_EQ(skel->bss->uretprobe_byname2_res, 8, "check_uretprobe_byname2_res");
>>>>>>> b7ba80a49124 (Commit)
	ASSERT_EQ(skel->bss->uprobe_byname3_sleepable_res, 9, "check_uprobe_byname3_sleepable_res");
	ASSERT_EQ(skel->bss->uprobe_byname3_res, 10, "check_uprobe_byname3_res");
	ASSERT_EQ(skel->bss->uretprobe_byname3_sleepable_res, 11, "check_uretprobe_byname3_sleepable_res");
	ASSERT_EQ(skel->bss->uretprobe_byname3_res, 12, "check_uretprobe_byname3_res");
<<<<<<< HEAD
}

void test_attach_probe(void)
{
	struct test_attach_probe *skel;

	skel = test_attach_probe__open();
	if (!ASSERT_OK_PTR(skel, "skel_open"))
		return;

	if (!ASSERT_OK(test_attach_probe__load(skel), "skel_load"))
		goto cleanup;
	if (!ASSERT_OK_PTR(skel->bss, "check_bss"))
		goto cleanup;

	if (test__start_subtest("manual-default"))
		test_attach_probe_manual(PROBE_ATTACH_MODE_DEFAULT);
	if (test__start_subtest("manual-legacy"))
		test_attach_probe_manual(PROBE_ATTACH_MODE_LEGACY);
	if (test__start_subtest("manual-perf"))
		test_attach_probe_manual(PROBE_ATTACH_MODE_PERF);
	if (test__start_subtest("manual-link"))
		test_attach_probe_manual(PROBE_ATTACH_MODE_LINK);

	if (test__start_subtest("auto"))
		test_attach_probe_auto(skel);
	if (test__start_subtest("kprobe-sleepable"))
		test_kprobe_sleepable();
	if (test__start_subtest("uprobe-lib"))
		test_uprobe_lib(skel);
	if (test__start_subtest("uprobe-sleepable"))
		test_uprobe_sleepable(skel);
	if (test__start_subtest("uprobe-ref_ctr"))
		test_uprobe_ref_ctr(skel);
=======
>>>>>>> b7ba80a49124 (Commit)

cleanup:
	test_attach_probe__destroy(skel);
	ASSERT_EQ(uprobe_ref_ctr, 0, "uprobe_ref_ctr_cleanup");
}
