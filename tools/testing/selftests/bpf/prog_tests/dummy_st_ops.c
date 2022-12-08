// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2021. Huawei Technologies Co., Ltd */
#include <test_progs.h>
<<<<<<< HEAD
#include "dummy_st_ops_success.skel.h"
#include "dummy_st_ops_fail.skel.h"
=======
#include "dummy_st_ops.skel.h"
>>>>>>> b7ba80a49124 (Commit)
#include "trace_dummy_st_ops.skel.h"

/* Need to keep consistent with definition in include/linux/bpf.h */
struct bpf_dummy_ops_state {
	int val;
};

static void test_dummy_st_ops_attach(void)
{
<<<<<<< HEAD
	struct dummy_st_ops_success *skel;
	struct bpf_link *link;

	skel = dummy_st_ops_success__open_and_load();
=======
	struct dummy_st_ops *skel;
	struct bpf_link *link;

	skel = dummy_st_ops__open_and_load();
>>>>>>> b7ba80a49124 (Commit)
	if (!ASSERT_OK_PTR(skel, "dummy_st_ops_load"))
		return;

	link = bpf_map__attach_struct_ops(skel->maps.dummy_1);
	ASSERT_EQ(libbpf_get_error(link), -EOPNOTSUPP, "dummy_st_ops_attach");

<<<<<<< HEAD
	dummy_st_ops_success__destroy(skel);
=======
	dummy_st_ops__destroy(skel);
>>>>>>> b7ba80a49124 (Commit)
}

static void test_dummy_init_ret_value(void)
{
	__u64 args[1] = {0};
	LIBBPF_OPTS(bpf_test_run_opts, attr,
		.ctx_in = args,
		.ctx_size_in = sizeof(args),
	);
<<<<<<< HEAD
	struct dummy_st_ops_success *skel;
	int fd, err;

	skel = dummy_st_ops_success__open_and_load();
=======
	struct dummy_st_ops *skel;
	int fd, err;

	skel = dummy_st_ops__open_and_load();
>>>>>>> b7ba80a49124 (Commit)
	if (!ASSERT_OK_PTR(skel, "dummy_st_ops_load"))
		return;

	fd = bpf_program__fd(skel->progs.test_1);
	err = bpf_prog_test_run_opts(fd, &attr);
	ASSERT_OK(err, "test_run");
	ASSERT_EQ(attr.retval, 0xf2f3f4f5, "test_ret");

<<<<<<< HEAD
	dummy_st_ops_success__destroy(skel);
=======
	dummy_st_ops__destroy(skel);
>>>>>>> b7ba80a49124 (Commit)
}

static void test_dummy_init_ptr_arg(void)
{
	int exp_retval = 0xbeef;
	struct bpf_dummy_ops_state in_state = {
		.val = exp_retval,
	};
	__u64 args[1] = {(unsigned long)&in_state};
	LIBBPF_OPTS(bpf_test_run_opts, attr,
		.ctx_in = args,
		.ctx_size_in = sizeof(args),
	);
	struct trace_dummy_st_ops *trace_skel;
<<<<<<< HEAD
	struct dummy_st_ops_success *skel;
	int fd, err;

	skel = dummy_st_ops_success__open_and_load();
=======
	struct dummy_st_ops *skel;
	int fd, err;

	skel = dummy_st_ops__open_and_load();
>>>>>>> b7ba80a49124 (Commit)
	if (!ASSERT_OK_PTR(skel, "dummy_st_ops_load"))
		return;

	fd = bpf_program__fd(skel->progs.test_1);

	trace_skel = trace_dummy_st_ops__open();
	if (!ASSERT_OK_PTR(trace_skel, "trace_dummy_st_ops__open"))
		goto done;

	err = bpf_program__set_attach_target(trace_skel->progs.fentry_test_1,
					     fd, "test_1");
	if (!ASSERT_OK(err, "set_attach_target(fentry_test_1)"))
		goto done;

	err = trace_dummy_st_ops__load(trace_skel);
	if (!ASSERT_OK(err, "load(trace_skel)"))
		goto done;

	err = trace_dummy_st_ops__attach(trace_skel);
	if (!ASSERT_OK(err, "attach(trace_skel)"))
		goto done;

	err = bpf_prog_test_run_opts(fd, &attr);
	ASSERT_OK(err, "test_run");
	ASSERT_EQ(in_state.val, 0x5a, "test_ptr_ret");
	ASSERT_EQ(attr.retval, exp_retval, "test_ret");
	ASSERT_EQ(trace_skel->bss->val, exp_retval, "fentry_val");

done:
<<<<<<< HEAD
	dummy_st_ops_success__destroy(skel);
=======
	dummy_st_ops__destroy(skel);
>>>>>>> b7ba80a49124 (Commit)
	trace_dummy_st_ops__destroy(trace_skel);
}

static void test_dummy_multiple_args(void)
{
	__u64 args[5] = {0, -100, 0x8a5f, 'c', 0x1234567887654321ULL};
	LIBBPF_OPTS(bpf_test_run_opts, attr,
		.ctx_in = args,
		.ctx_size_in = sizeof(args),
	);
<<<<<<< HEAD
	struct dummy_st_ops_success *skel;
=======
	struct dummy_st_ops *skel;
>>>>>>> b7ba80a49124 (Commit)
	int fd, err;
	size_t i;
	char name[8];

<<<<<<< HEAD
	skel = dummy_st_ops_success__open_and_load();
=======
	skel = dummy_st_ops__open_and_load();
>>>>>>> b7ba80a49124 (Commit)
	if (!ASSERT_OK_PTR(skel, "dummy_st_ops_load"))
		return;

	fd = bpf_program__fd(skel->progs.test_2);
	err = bpf_prog_test_run_opts(fd, &attr);
	ASSERT_OK(err, "test_run");
	for (i = 0; i < ARRAY_SIZE(args); i++) {
		snprintf(name, sizeof(name), "arg %zu", i);
		ASSERT_EQ(skel->bss->test_2_args[i], args[i], name);
	}

<<<<<<< HEAD
	dummy_st_ops_success__destroy(skel);
}

static void test_dummy_sleepable(void)
{
	__u64 args[1] = {0};
	LIBBPF_OPTS(bpf_test_run_opts, attr,
		.ctx_in = args,
		.ctx_size_in = sizeof(args),
	);
	struct dummy_st_ops_success *skel;
	int fd, err;

	skel = dummy_st_ops_success__open_and_load();
	if (!ASSERT_OK_PTR(skel, "dummy_st_ops_load"))
		return;

	fd = bpf_program__fd(skel->progs.test_sleepable);
	err = bpf_prog_test_run_opts(fd, &attr);
	ASSERT_OK(err, "test_run");

	dummy_st_ops_success__destroy(skel);
=======
	dummy_st_ops__destroy(skel);
>>>>>>> b7ba80a49124 (Commit)
}

void test_dummy_st_ops(void)
{
	if (test__start_subtest("dummy_st_ops_attach"))
		test_dummy_st_ops_attach();
	if (test__start_subtest("dummy_init_ret_value"))
		test_dummy_init_ret_value();
	if (test__start_subtest("dummy_init_ptr_arg"))
		test_dummy_init_ptr_arg();
	if (test__start_subtest("dummy_multiple_args"))
		test_dummy_multiple_args();
<<<<<<< HEAD
	if (test__start_subtest("dummy_sleepable"))
		test_dummy_sleepable();

	RUN_TESTS(dummy_st_ops_fail);
=======
>>>>>>> b7ba80a49124 (Commit)
}
