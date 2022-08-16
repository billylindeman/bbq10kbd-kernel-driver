#include "bbq10kbd.h"

#define INTERRUPT_GPIO 451

static int bbq10kbd_init_input(struct bbq10kbd_keypad* keypad_data)
{
  struct input_dev* input;
  int ret;
  const int num_keycodes = ARRAY_SIZE(bbq10kbd_keycodes);

  printk(KERN_DEBUG "bbq10kbd: initialising internal input...");
  input = input_allocate_device();
  if(input == NULL)
    return -ENOMEM;
 
  input->name = "bbq10kbd-i2c-kbd";
  input->evbit[0] = BIT_MASK(EV_KEY);
  input->keycode = bbq10kbd_keycodes;
  input->keycodesize = sizeof(unsigned short); 
  input->keycodemax = num_keycodes;

  for(ret = 0; ret < num_keycodes; ret++)
  {
    if(bbq10kbd_keycodes[ret] != KEY_RESERVED)
      set_bit(bbq10kbd_keycodes[ret], input->keybit);
  }

  ret = input_register_device(input);
  if(ret != 0)
  {
    printk(KERN_ERR "bbq10kbd: unable to register input device, register returned %d\n", ret);
    input_unregister_device(input);
    return -ENODEV;
  }

  printk(KERN_DEBUG "bbq10kbd: initialised input device with %d keycodes", num_keycodes);
  keypad_data->input = input;

  return ret;

}


// IRQ Handler to read i2c fifo and generate events 
static irqreturn_t bbq10kbd_irq_handler(int irq, void *dev_id)
{
    printk(KERN_DEBUG "bbq10kbd: irq handler");

	struct bbq10kbd_keypad *keypad_data = dev_id;
	uint8_t reg;
    uint32_t fifo_read;
	int error;



    // Read the FIFO until it's empty
    while(reg = i2c_smbus_read_word_data(keypad_data->i2c, REG_FIF) && reg) {
        uint8_t key = reg & 0x11110000;
        uint8_t state = reg & 0x00001111;

        printk(KERN_DEBUG "bbq10kbd: fifo-read key: %d, state: %d", key, state);


        if(state == KEY_PRESSED || state == KEY_RELEASED) {
            int keycode = bbq10kbd_keycodes[key];
            input_event(keypad_data->input, EV_KEY, keycode, (state == KEY_PRESSED));  
        }
    }


    error = i2c_write_byte(keypad_data->i2c, REG_INT, 0x00);
    if(error) {
        dev_err(&keypad_data->i2c->dev, "unable to clear REG_INT\n");
    }



//	error = i2c_read_byte(keypad_data, REG_INT, &reg);
//	if (error) {
//		dev_err(&keypad_data->client->dev,
//			"unable to read REG_INT_STAT\n");
//		return IRQ_NONE;
//	}
//
//	if (!reg)
//		return IRQ_NONE;
//
//	if (reg & INT_STAT_OVR_FLOW_INT)
//		dev_warn(&keypad_data->client->dev, "overflow occurred\n");
//
//	if (reg & INT_STAT_K_INT)
//		tca8418_read_keypad(keypad_data);
//
//	/* Clear all interrupts, even IRQs we didn't check (GPI, CAD, LCK) */
//	reg = 0xff;
//	error = tca8418_write_byte(keypad_data, REG_INT_STAT, reg);
//	if (error)
//		dev_err(&keypad_data->client->dev,
//			"unable to clear REG_INT_STAT\n");

	return IRQ_HANDLED;
}
static int bbq10kbd_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id) 
{
  struct bbq10kbd_keypad *keypad_data;
  int ret, error;
  int irq;

  printk(KERN_DEBUG "bbq10kbd: probe!!!");
  if(!i2c_check_functionality(client->adapter, 
        I2C_FUNC_SMBUS_BYTE_DATA | 
        I2C_FUNC_SMBUS_WORD_DATA))
  {
    printk(KERN_ERR "bbq10kbd: %s needed i2c functionality is not supported\n", __func__);
    return -ENODEV;
  }
  printk(KERN_DEBUG "bbq10kbd: configuring i2c");


  printk(KERN_DEBUG "bbq10kbd: setting up device...\n");
  keypad_data = devm_kzalloc(&client->dev, sizeof(struct bbq10kbd_keypad), GFP_KERNEL);
  if(keypad_data == NULL)
    return -ENOMEM; 
  keypad_data->i2c = client;

  // check fw version
  ret = i2c_read_byte(keypad_data->i2c, REG_VER);
  printk(KERN_DEBUG "bb10kbd: firmware version = %d", ret);

  //configure interrupt
  ret = i2c_write_byte(keypad_data->i2c, REG_CFG, 0b01011110);

  error = devm_gpio_request_one(&client->dev, INTERRUPT_GPIO, GPIOF_IN, "bbq10kbd_irq");
  if(error) {
      printk(KERN_ERR "bbq10kbd unable to claim interrupt gpio");
      return -ENODEV;
  }

  //gpio_direction_input(INTERRUPT_GPIO);
  irq = gpiod_to_irq(INTERRUPT_GPIO);
  printk(KERN_DEBUG "bbq10kbd: irq from gpio: %d, irq from device: %d", irq, client->irq);
 

  printk(KERN_DEBUG "bbq10kbd: requesting irq handler %d", client->irq);
  error = devm_request_irq(&client->dev, client->irq, bbq10kbd_irq_handler,
        IRQF_SHARED | IRQF_TRIGGER_FALLING,
        client->name, keypad_data);
  

  //error = devm_request_irq(&client->dev, client->irq,
  //      bbq10kbd_irq_handler,
  //      IRQF_TRIGGER_FALLING,
  //      client->name, keypad_data);

  if (error) {
    dev_err(&client->dev, "Unable to claim irq %d; error %d\n",
    client->irq, error);
    return error;
  }

  
  ret = bbq10kbd_init_input(keypad_data);
  if(ret != 0){
    printk(KERN_ERR "bbq10kbd: unable to initialise input device, returned %d\n", ret);
    return -ENODEV;
  }
  
  // Device setup complete, set the client's device data
  i2c_set_clientdata(client, keypad_data);
  
  printk(KERN_DEBUG "bbq10kbd: probed.\n");
  return 0;
}

static int bbq10kbd_i2c_remove(struct i2c_client *client) {
  struct bbq10kbd_keypad *keypad_data = i2c_get_clientdata(client);
 
  printk(KERN_DEBUG "bbq10kbd: input_unregister_device");
  input_unregister_device(keypad_data->input);
  printk(KERN_DEBUG "bbq10kbd: freeing device memory");
  kfree(keypad_data);

  printk(KERN_DEBUG "bbq10kbd: removed\n"); 
  return 0;  
}

static struct i2c_driver bbq10kbd_i2c_driver = {
  .driver = {
    .name = DEVICE_NAME,
    .owner = THIS_MODULE,
    .of_match_table = bbq10kbd_ids,
  },
  .probe = bbq10kbd_i2c_probe,
  .remove = bbq10kbd_i2c_remove,
  .id_table = bbq10kbd_id,
};

static int __init bbq10kbd_init(void) {
  int res;

  printk(KERN_DEBUG "bbq10kbd: initialising i2c...");
  res = i2c_add_driver(&bbq10kbd_i2c_driver);
  if(res != 0) {
    printk(KERN_DEBUG "bbq10kbd: i2c driver registration failed, module not inserted.\n");
    return res;
  }

  printk(KERN_DEBUG "bbq10kbd: inited\n");
  return res;
}

static void __exit bbq10kbd_exit(void) 
{
  printk(KERN_DEBUG "bbq10kbd: exiting...");
  printk(KERN_DEBUG "bbq10kbd: deleting i2c driver...");
  i2c_del_driver(&bbq10kbd_i2c_driver);

  printk("bbq10kbd: exited");
}

module_init(bbq10kbd_init);
module_exit(bbq10kbd_exit);
