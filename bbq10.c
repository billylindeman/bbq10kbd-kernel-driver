#include "bbq10.h"

static int bbq10_init_input(struct bbq10_device* device)
{
  struct input_dev* input;
  int ret;
  const int num_keycodes = ARRAY_SIZE(bbq10_keycodes);

  printk(KERN_DEBUG "bbq10: initialising internal input...");
  input = input_allocate_device();
  if(input == NULL)
    return -ENOMEM;
 
  input->name = "bbq10-i2c-kbd";
  input->evbit[0] = BIT_MASK(EV_KEY);
  input->keycode = bbq10_keycodes;
  input->keycodesize = sizeof(unsigned short); 
  input->keycodemax = num_keycodes;

  for(ret = 0; ret < num_keycodes; ret++)
  {
    if(bbq10_keycodes[ret] != KEY_RESERVED)
      set_bit(bbq10_keycodes[ret], input->keybit);
  }

  ret = input_register_device(input);
  if(ret != 0)
  {
    printk(KERN_ERR "bbq10: unable to register input device, register returned %d\n", ret);
    input_unregister_device(input);
    return -ENODEV;
  }

  printk(KERN_DEBUG "bbq10: initialised input device with %d keycodes", num_keycodes);
  device->input = input;

  return ret;

}

static int bbq10_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id) 
{
  struct bbq10_device *device;
  int ret;

  printk(KERN_DEBUG "bbq10: probe!!!");
  if(!i2c_check_functionality(client->adapter, 
        I2C_FUNC_SMBUS_BYTE_DATA | 
        I2C_FUNC_SMBUS_WORD_DATA))
  {
    printk(KERN_ERR "bbq10: %s needed i2c functionality is not supported\n", __func__);
    return -ENODEV;
  }
  printk(KERN_DEBUG "bbq10: configuring i2c");

  printk(KERN_DEBUG "bbq10: setting up device...\n");
  device = devm_kzalloc(&client->dev, sizeof(struct bbq10_device), GFP_KERNEL);
  if(device == NULL)
    return -ENOMEM; 
  device->i2c = client;

  ret = bbq10_init_input(device);
  if(ret != 0){
    printk(KERN_ERR "bbq10: unable to initialise input device, returned %d\n", ret);
    return -ENODEV;
  }
  
  // Device setup complete, set the client's device data
  i2c_set_clientdata(client, device);
  
  printk(KERN_DEBUG "bbq10: probed.\n");
  return 0;
}

static int bbq10_i2c_remove(struct i2c_client *client) {
  struct bbq10_device *dev = i2c_get_clientdata(client);
 
  printk(KERN_DEBUG "bbq10: input_unregister_device");
  input_unregister_device(dev->input);
  printk(KERN_DEBUG "bbq10: freeing device memory");
  kfree(dev);

  printk(KERN_DEBUG "bbq10: removed\n"); 
  return 0;  
}

static struct i2c_driver bbq10_i2c_driver = {
  .driver = {
    .name = DEVICE_NAME,
    .owner = THIS_MODULE,
    .of_match_table = of_match_ptr(bbq10_ids),
  },
  .probe = bbq10_i2c_probe,
  .remove = bbq10_i2c_remove,
  .id_table = bbq10_id,
};

static int __init bbq10_init(void) {
  int res;

  printk(KERN_DEBUG "bbq10: initialising i2c...");
  res = i2c_add_driver(&bbq10_i2c_driver);
  if(res != 0) {
    printk(KERN_DEBUG "bbq10: i2c driver registration failed, module not inserted.\n");
    return res;
  }

  printk(KERN_DEBUG "bbq10: inited\n");
  return res;
}

static void __exit bbq10_exit(void) 
{
  printk(KERN_DEBUG "bbq10: exiting...");
  printk(KERN_DEBUG "bbq10: deleting i2c driver...");
  i2c_del_driver(&bbq10_i2c_driver);
  printk("bbq10: exited");
}

module_init(bbq10_init);
module_exit(bbq10_exit);
