#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

#include "bbq10_i2c.h"
#include "bbq10_keycodes.h"
#include "bbq10_vfs.h"

#define DEVICE_NAME           "bbq10"

static struct of_device_id bbq10_ids[] = {{.compatible = DEVICE_NAME},{}};
static const struct i2c_device_id bbq10_id[] = { {DEVICE_NAME, 0}, {}};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Billy Lindeman <billylindeman@gmail.com>");
MODULE_DESCRIPTION("bbq10 i2c keyboard driver");
MODULE_VERSION("0.1");

struct bbq10_device {
  struct i2c_client *i2c;
  struct input_dev *input;
};

MODULE_DEVICE_TABLE(i2c, bbq10_id);
