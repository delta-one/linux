/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_ZYNQMP_IPI_MESSAGE_H_
#define _LINUX_ZYNQMP_IPI_MESSAGE_H_

/**
 * struct zynqmp_ipi_message - ZynqMP IPI message structure
 * @len:  Length of message
 * @data: message payload
 *
 * This is the structure for data used in mbox_send_message
<<<<<<< HEAD
 * the maximum length of data buffer is fixed to 32 bytes.
=======
 * the maximum length of data buffer is fixed to 12 bytes.
>>>>>>> b7ba80a49124 (Commit)
 * Client is supposed to be aware of this.
 */
struct zynqmp_ipi_message {
	size_t len;
	u8 data[];
};

#endif /* _LINUX_ZYNQMP_IPI_MESSAGE_H_ */
