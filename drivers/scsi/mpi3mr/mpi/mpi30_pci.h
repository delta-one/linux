/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
<<<<<<< HEAD
 *  Copyright 2016-2023 Broadcom Inc. All rights reserved.
=======
 *  Copyright 2016-2021 Broadcom Inc. All rights reserved.
>>>>>>> b7ba80a49124 (Commit)
 *
 */
#ifndef MPI30_PCI_H
#define MPI30_PCI_H     1
<<<<<<< HEAD
#ifndef MPI3_NVME_ENCAP_CMD_MAX
#define MPI3_NVME_ENCAP_CMD_MAX               (1)
#endif
=======

>>>>>>> b7ba80a49124 (Commit)
#define MPI3_NVME_FLAGS_FORCE_ADMIN_ERR_REPLY_MASK      (0x0002)
#define MPI3_NVME_FLAGS_FORCE_ADMIN_ERR_REPLY_FAIL_ONLY (0x0000)
#define MPI3_NVME_FLAGS_FORCE_ADMIN_ERR_REPLY_ALL       (0x0002)
#define MPI3_NVME_FLAGS_SUBMISSIONQ_MASK                (0x0001)
#define MPI3_NVME_FLAGS_SUBMISSIONQ_IO                  (0x0000)
#define MPI3_NVME_FLAGS_SUBMISSIONQ_ADMIN               (0x0001)

#endif
