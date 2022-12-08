.. SPDX-License-Identifier: GPL-2.0

<<<<<<< HEAD
.. _linux_doc:

==============================
=======

.. The Linux Kernel documentation master file, created by
   sphinx-quickstart on Fri Feb 12 13:51:46 2016.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. _linux_doc:

>>>>>>> b7ba80a49124 (Commit)
The Linux Kernel documentation
==============================

This is the top level of the kernel's documentation tree.  Kernel
documentation, like the kernel itself, is very much a work in progress;
that is especially true as we work to integrate our many scattered
documents into a coherent whole.  Please note that improvements to the
documentation are welcome; join the linux-doc list at vger.kernel.org if
you want to help out.

<<<<<<< HEAD
Working with the development community
======================================

The essential guides for interacting with the kernel's development
community and getting your work upstream.

.. toctree::
   :maxdepth: 1

   process/development-process
   process/submitting-patches
   Code of conduct <process/code-of-conduct>
   maintainer/index
   All development-process docs <process/index>


Internal API manuals
====================

Manuals for use by developers working to interface with the rest of the
kernel.

.. toctree::
   :maxdepth: 1

   core-api/index
   driver-api/index
   subsystem-apis
   Locking in the kernel <locking/index>

Development tools and processes
===============================

Various other manuals with useful information for all kernel developers.

.. toctree::
   :maxdepth: 1

   process/license-rules
   doc-guide/index
   dev-tools/index
   dev-tools/testing-overview
   kernel-hacking/index
   trace/index
=======
Licensing documentation
-----------------------

The following describes the license of the Linux kernel source code
(GPLv2), how to properly mark the license of individual files in the source
tree, as well as links to the full license text.

* :ref:`kernel_licensing`

User-oriented documentation
---------------------------

The following manuals are written for *users* of the kernel — those who are
trying to get it to work optimally on a given system.

.. toctree::
   :maxdepth: 2

   admin-guide/index
   kbuild/index

Firmware-related documentation
------------------------------
The following holds information on the kernel's expectations regarding the
platform firmwares.

.. toctree::
   :maxdepth: 2

   firmware-guide/index
   devicetree/index

Application-developer documentation
-----------------------------------

The user-space API manual gathers together documents describing aspects of
the kernel interface as seen by application developers.

.. toctree::
   :maxdepth: 2

   userspace-api/index


Introduction to kernel development
----------------------------------

These manuals contain overall information about how to develop the kernel.
The kernel community is quite large, with thousands of developers
contributing over the course of a year.  As with any large community,
knowing how things are done will make the process of getting your changes
merged much easier.

.. toctree::
   :maxdepth: 2

   process/index
   dev-tools/index
   doc-guide/index
   kernel-hacking/index
   trace/index
   maintainer/index
>>>>>>> b7ba80a49124 (Commit)
   fault-injection/index
   livepatch/index
   rust/index


<<<<<<< HEAD
User-oriented documentation
===========================

The following manuals are written for *users* of the kernel — those who are
trying to get it to work optimally on a given system and application
developers seeking information on the kernel's user-space APIs.

.. toctree::
   :maxdepth: 1

   admin-guide/index
   The kernel build system <kbuild/index>
   admin-guide/reporting-issues.rst
   User-space tools <tools/index>
   userspace-api/index

See also: the `Linux man pages <https://www.kernel.org/doc/man-pages/>`_,
which are kept separately from the kernel's own documentation.

Firmware-related documentation
==============================
The following holds information on the kernel's expectations regarding the
platform firmwares.

.. toctree::
   :maxdepth: 1

   firmware-guide/index
   devicetree/index


Architecture-specific documentation
===================================
=======
Kernel API documentation
------------------------

These books get into the details of how specific kernel subsystems work
from the point of view of a kernel developer.  Much of the information here
is taken directly from the kernel source, with supplemental material added
as needed (or at least as we managed to add it — probably *not* all that is
needed).
>>>>>>> b7ba80a49124 (Commit)

.. toctree::
   :maxdepth: 2

<<<<<<< HEAD
   arch/index


Other documentation
===================
=======
   driver-api/index
   core-api/index
   locking/index
   accounting/index
   block/index
   cdrom/index
   cpu-freq/index
   fb/index
   fpga/index
   hid/index
   i2c/index
   iio/index
   isdn/index
   infiniband/index
   leds/index
   netlabel/index
   networking/index
   pcmcia/index
   power/index
   target/index
   timers/index
   spi/index
   w1/index
   watchdog/index
   virt/index
   input/index
   hwmon/index
   gpu/index
   security/index
   sound/index
   crypto/index
   filesystems/index
   mm/index
   bpf/index
   usb/index
   PCI/index
   scsi/index
   misc-devices/index
   scheduler/index
   mhi/index
   peci/index

Architecture-agnostic documentation
-----------------------------------

.. toctree::
   :maxdepth: 2

   asm-annotations

Architecture-specific documentation
-----------------------------------

.. toctree::
   :maxdepth: 2

   arch


Other documentation
-------------------
>>>>>>> b7ba80a49124 (Commit)

There are several unsorted documents that don't seem to fit on other parts
of the documentation body, or may require some adjustments and/or conversion
to ReStructured Text format, or are simply too old.

.. toctree::
<<<<<<< HEAD
   :maxdepth: 1

=======
   :maxdepth: 2

   tools/index
>>>>>>> b7ba80a49124 (Commit)
   staging/index


Translations
<<<<<<< HEAD
============
=======
------------
>>>>>>> b7ba80a49124 (Commit)

.. toctree::
   :maxdepth: 2

   translations/index

Indices and tables
==================

* :ref:`genindex`
