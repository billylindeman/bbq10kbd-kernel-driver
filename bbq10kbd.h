#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

#include "bbq10kbd_i2c.h"
#include "bbq10kbd_keycodes.h"

#define DEVICE_NAME           "bbq10kbd"

#define REG_VER        0x01    // FW Version Register
#define REG_CFG        0x02    // Configuration Register
#define REG_INT        0x03    // Interrupt status register
#define REG_KEY        0x04    // Key Status Register
//#define REG_BKL        0x05    // Backlight control register
//#define REG_DEB        0x06    // Debounce control Register (not implemented)
//#define REG_FRQ        0x07    // Polling frequency register (not implemented)
//#define REG_RST        0x08    // Chip Reset Register 
#define REG_FIF        0x09    // Key Press FIFO Register

#define INTERRUPT_MASK_KEYPRESS 0b00001000



#define KEY_PRESSED  1
#define KEY_RELEASED 3


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Billy Lindeman <billylindeman@gmail.com>");
MODULE_DESCRIPTION("bbq10kbd i2c keyboard driver");
MODULE_VERSION("0.1");

struct bbq10kbd_keypad {
  struct i2c_client *i2c;
  struct input_dev *input;
};

static struct of_device_id bbq10kbd_ids[] = {
    {.compatible = DEVICE_NAME},
    {}
};

static const struct i2c_device_id bbq10kbd_id[] = { 
    {DEVICE_NAME, 0}, 
    {}
};
MODULE_DEVICE_TABLE(i2c, bbq10kbd_id);



