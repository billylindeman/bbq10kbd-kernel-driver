#include <linux/i2c.h>

static int i2c_read_byte(struct i2c_client *client, unsigned char command) {
  return i2c_smbus_read_byte_data(client, command);
}
static int i2c_write_byte(struct i2c_client *client, int reg, int value) {
  return i2c_smbus_write_byte_data(client, reg, value);
}


