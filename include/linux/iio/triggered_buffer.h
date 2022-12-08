/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_IIO_TRIGGERED_BUFFER_H_
#define _LINUX_IIO_TRIGGERED_BUFFER_H_

#include <linux/iio/buffer.h>
#include <linux/interrupt.h>

<<<<<<< HEAD
struct iio_dev;
struct iio_dev_attr;
=======
struct attribute;
struct iio_dev;
>>>>>>> b7ba80a49124 (Commit)
struct iio_buffer_setup_ops;

int iio_triggered_buffer_setup_ext(struct iio_dev *indio_dev,
	irqreturn_t (*h)(int irq, void *p),
	irqreturn_t (*thread)(int irq, void *p),
	enum iio_buffer_direction direction,
	const struct iio_buffer_setup_ops *setup_ops,
<<<<<<< HEAD
	const struct iio_dev_attr **buffer_attrs);
=======
	const struct attribute **buffer_attrs);
>>>>>>> b7ba80a49124 (Commit)
void iio_triggered_buffer_cleanup(struct iio_dev *indio_dev);

#define iio_triggered_buffer_setup(indio_dev, h, thread, setup_ops)		\
	iio_triggered_buffer_setup_ext((indio_dev), (h), (thread),		\
					IIO_BUFFER_DIRECTION_IN, (setup_ops),	\
					NULL)

int devm_iio_triggered_buffer_setup_ext(struct device *dev,
					struct iio_dev *indio_dev,
					irqreturn_t (*h)(int irq, void *p),
					irqreturn_t (*thread)(int irq, void *p),
					enum iio_buffer_direction direction,
					const struct iio_buffer_setup_ops *ops,
<<<<<<< HEAD
					const struct iio_dev_attr **buffer_attrs);
=======
					const struct attribute **buffer_attrs);
>>>>>>> b7ba80a49124 (Commit)

#define devm_iio_triggered_buffer_setup(dev, indio_dev, h, thread, setup_ops)	\
	devm_iio_triggered_buffer_setup_ext((dev), (indio_dev), (h), (thread),	\
					    IIO_BUFFER_DIRECTION_IN,		\
					    (setup_ops), NULL)

#endif
