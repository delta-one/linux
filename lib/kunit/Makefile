obj-$(CONFIG_KUNIT) +=			kunit.o

kunit-objs +=				test.o \
					resource.o \
<<<<<<< HEAD
					static_stub.o \
=======
>>>>>>> b7ba80a49124 (Commit)
					string-stream.o \
					assert.o \
					try-catch.o \
					executor.o

ifeq ($(CONFIG_KUNIT_DEBUGFS),y)
kunit-objs +=				debugfs.o
endif

<<<<<<< HEAD
# KUnit 'hooks' are built-in even when KUnit is built as a module.
obj-y +=				hooks.o

=======
>>>>>>> b7ba80a49124 (Commit)
obj-$(CONFIG_KUNIT_TEST) +=		kunit-test.o

# string-stream-test compiles built-in only.
ifeq ($(CONFIG_KUNIT_TEST),y)
obj-$(CONFIG_KUNIT_TEST) +=		string-stream-test.o
endif

obj-$(CONFIG_KUNIT_EXAMPLE_TEST) +=	kunit-example-test.o
